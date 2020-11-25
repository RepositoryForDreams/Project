#include "JGMemory.h"
#include "JGMath.h"
namespace JG
{
	static JGAllocatorManager* gAllocatorManager = nullptr;


	void* JGMemoryHandle::Get() const
	{
		if (mPtr == nullptr) return nullptr;
		else return (void*)(*mPtr);
	}

	void* JGStackAllocator::Alloc(u64 size)
	{
		size = JGMath::AlignUp(size, sizeof(ptraddr));

		


		size += sizeof(u64);
		*((u64*)mTopAddress) = size;

		mUseMemSize += size;
		if(mUseMemSize > mTotalMemSize)
			JASSERT("JGStackAllocator::Alloc : mUseMemSize > mTotalMemSize");



		void* p = (void*)(mTopAddress + sizeof(u64));
		mTopAddress += size;


		return p;
	}
	void JGStackAllocator::DeAlloc(void** ptr)
	{
		if ((*ptr) == nullptr) return;
		if (mTopAddress == mStartAddress) return;


		u64 size = *(u64*)((ptraddr)(*ptr) - sizeof(u64));



		mUseMemSize   -= size;
		mTopAddress   -= size;

		if((void*)(mTopAddress + sizeof(u64)) != *ptr)
			JASSERT("JGStackAllocator::DeAlloc : mTopAddress != ptr");

		*ptr = nullptr;
	}


	void* JGLinearAllocator::Alloc(u64 size) 
	{
		void* p = nullptr;
		size = JGMath::AlignUp(size, sizeof(ptraddr));


		p = (void*)mCurrentAddress;

		mCurrentAddress += size;
		mUseMemSize     += size;

		if (mUseMemSize > mTotalMemSize)
			JASSERT("JGLinearAllocator::Alloc : mUseMemSize > mTotalMemSize");




		return p;
	}


	JGHeapAllocator::JGHeapAllocator(u64 size, ptraddr address) : JGAllocator(size, address)
	{
		mCurrStartAddr = mStartAddress;

		BlockHeader* startBH = (BlockHeader*)mStartAddress;
		startBH->prev = 0;
		startBH->next = 0;
		startBH->blockSize = mTotalMemSize;
		startBH->handle = 0;
	}


	void* JGHeapAllocator::Alloc(u64 size)
	{
		if (mUseMemSize > mTotalMemSize)
			JASSERT("JGHeapAllocator::Alloc : mUseMemSize > mTotalMemSize");
		size = JGMath::AlignUp(size, sizeof(ptraddr));
		
		// 예외처리
		void* result = nullptr;

		ptraddr lastAddr = mStartAddress + mTotalMemSize;
		u64     bhSize = sizeof(BlockHeader);


		u8 roundsCnt = 0;
		for (ptraddr pos = mCurrStartAddr; pos < lastAddr;)
		{
			BlockHeader startBH = *(BlockHeader*)pos;
			ptraddr	    startAddr = pos;
			if (startBH.handle == 0 && startBH.blockSize >= size + bhSize)
			{
				u64 leftSize  = size + bhSize;
				u64 rightSize = startBH.blockSize - leftSize;


				// 기본 메모리보다 크기가 작다면 통합 할당
				if (rightSize < bhSize + sizeof(ptraddr))
				{
					BlockHeader bh;
					bh.blockSize = leftSize + rightSize;
					bh.prev = startBH.prev;
					bh.next = startBH.next;

					*(BlockHeader*)startAddr = bh;

					result = (void*)(startAddr + bhSize);
					mCurrStartAddr = startAddr + bh.blockSize;
					mUseMemSize += bh.blockSize;
				}
				// 아니라면 기존 방식대로 할당
				else
				{
					BlockHeader leftBH;
					leftBH.blockSize = leftSize;
					leftBH.prev = startBH.prev;
					leftBH.next = startAddr + leftBH.blockSize;
					*(BlockHeader*)startAddr = leftBH;

					



					BlockHeader rightBH;
					rightBH.blockSize = rightSize;
					rightBH.handle = 0;
					rightBH.prev = startAddr;
					rightBH.next = startBH.next;
					*(BlockHeader*)(leftBH.next) = rightBH;

					if(rightBH.next != 0)
					{
						((BlockHeader*)(rightBH.next))->prev = leftBH.next;
					}



					result = (void*)(startAddr + bhSize);
					mCurrStartAddr = leftBH.next;
					mUseMemSize += leftBH.blockSize;
				}
				break;
			}
			// 사용 중이라면
			else
			{
			
				if (roundsCnt >= 1)
				{
					JASSERT("JGHeapAllocator::Alloc : Alloc Memory RoundsCount Exceed 1");
				}


				pos += startBH.blockSize;


				// 마지막 할당인지 확인
				if (pos >= lastAddr)
				{
					mCurrStartAddr = mStartAddress;
					pos = mCurrStartAddr;
					++roundsCnt;
				}
				continue;
			}
		}


		return result;
	}

	void JGHeapAllocator::DeAlloc(void** ptr)
	{
		
		ptraddr lastAddr  = mStartAddress + mTotalMemSize;
		ptraddr startAddr = (ptraddr)(*ptr);
		u64     bhSize    = sizeof(BlockHeader);
		if (startAddr < mStartAddress || startAddr > lastAddr)
		{
			JASSERT("JGHeapAllocator::DeAlloc : Not Allocated Memory.");
		}



		BlockHeader* startBH = (BlockHeader*)(startAddr - bhSize);
		

		u64 deAllocatedBlockSize = startBH->blockSize;
		// 중간
		if (startBH->prev != 0 && startBH->next != 0)
		{
			BlockHeader* prevBH = (BlockHeader*)startBH->prev;
			BlockHeader* nextBH = (BlockHeader*)startBH->next;


			bool isCurrStartAddr = (mCurrStartAddr == (ptraddr)startBH || mCurrStartAddr == (ptraddr)prevBH || mCurrStartAddr == (ptraddr)nextBH);
			bool isMemDefragCurrAddr = (mMemoryDefragCurrAddr == (ptraddr)startBH || mMemoryDefragCurrAddr == (ptraddr)prevBH || mMemoryDefragCurrAddr == (ptraddr)nextBH);
			// 양쪽 병합
			if (prevBH->handle == 0 && nextBH->handle == 0)
			{
				prevBH->blockSize = prevBH->blockSize + startBH->blockSize + nextBH->blockSize;
				prevBH->prev = prevBH->prev;
				prevBH->next = nextBH->next;
				prevBH->handle = 0;

				if (nextBH->next != 0)
				{
					((BlockHeader*)(nextBH->next))->prev = (ptraddr)prevBH;
				}
				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)prevBH;
				if(isMemDefragCurrAddr) mMemoryDefragCurrAddr = (ptraddr)prevBH;

			}
			// 오른쪽 병합
			else if (prevBH->handle != 0 && nextBH->handle == 0)
			{
				startBH->blockSize = startBH->blockSize + nextBH->blockSize;
				startBH->prev = startBH->prev;
				startBH->next = nextBH->next;
				startBH->handle = 0;

				
				if (nextBH->next != 0)
				{
					((BlockHeader*)(nextBH->next))->prev = (ptraddr)startBH;
				}
				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)startBH;
				if (isMemDefragCurrAddr) mMemoryDefragCurrAddr = (ptraddr)startBH;
			}
			// 왼쪽 병합
			else if (prevBH->handle == 0 && nextBH->handle != 0)
			{
				prevBH->blockSize = prevBH->blockSize + startBH->blockSize;
				prevBH->prev      = prevBH->prev;
				prevBH->next      = startBH->next;
				prevBH->handle = 0;

				nextBH->prev = (ptraddr)prevBH;

				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)prevBH;
				if (isMemDefragCurrAddr) mMemoryDefragCurrAddr = (ptraddr)prevBH;
			}
			// 병합 X
			else
			{
				startBH->handle = 0;
			}

		}
		// 처음 블럭
		else if (startBH->prev == 0 && startBH->next != 0)
		{
			// 다음 블럭이 할당된 블럭인지 확인
			BlockHeader* nextBH = (BlockHeader*)startBH->next;
			bool isCurrStartAddr = (mCurrStartAddr == (ptraddr)startBH ||  mCurrStartAddr == (ptraddr)nextBH);
			if (nextBH->handle == 0)
			{
				// 합병 시작
				BlockHeader bh;
				startBH->blockSize = startBH->blockSize + nextBH->blockSize;
				startBH->prev = startBH->prev;
				startBH->next = nextBH->next;
				startBH->handle = 0;

				((BlockHeader*)(nextBH->next))->prev = (ptraddr)startBH;
				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)startBH;
			}
			else
			{
				// 그냥 할당만 해제
				startBH->handle = 0;
			}
		}
		// 마지막 블럭
		else if (startBH->next == 0 && startBH->prev != 0)
		{
			BlockHeader* prevBH = (BlockHeader*)startBH->prev;
			bool isCurrStartAddr = (mCurrStartAddr == (ptraddr)startBH || mCurrStartAddr == (ptraddr)prevBH);
			if(prevBH->handle == 0)
			{
				prevBH->blockSize = prevBH->blockSize + startBH->blockSize;
				prevBH->prev = prevBH->prev;
				prevBH->next = startBH->next;
				prevBH->handle = 0;

				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)prevBH;
			}
			else
			{
				startBH->handle = 0;
			}

		}
		else
		{
			startBH->handle = 0;
		}

		mUseMemSize -= deAllocatedBlockSize;
		(*ptr) = nullptr;
	}

	void JGHeapAllocator::MemoryDefragmenter(u64 countPerFrame)
	{
		
		ptraddr curPos = mMemoryDefragCurrAddr;
		u64 count = 0;
		/*
		일단 현재 메모리 조각모음할때 시작점은 게속 증가하지만
		중간에 매모리 해제를 할 경우
		병합할려는 빈 블록이 메모리 조각모음주소나 해제할려는 블록이 메모리 조각모음 주소하는 모시기면은 메모리 조각모음 위치를 다음 블록으로 바꿈



		*/
		ptraddr emptyBlockAddr = 0;

		while (count < countPerFrame)
		{
			if (curPos == 0) {
				curPos = mStartAddress;
				emptyBlockAddr = 0;
			}
			++count;

			BlockHeader bh = (*(BlockHeader*)curPos);

			
			if (bh.handle != 0 && emptyBlockAddr != 0)
			{
				ptraddr* memPtr = (ptraddr*)bh.handle;
				

				



		
				BlockHeader* emptyBH = (BlockHeader*)emptyBlockAddr;
				BlockHeader* allocBH = (BlockHeader*)curPos;
				BlockHeader* prevBH  = (BlockHeader*)emptyBH->prev;
				BlockHeader* nextBH  = (BlockHeader*)allocBH->next;

				bool isCurrStartAddr = (mCurrStartAddr == (ptraddr)emptyBH || mCurrStartAddr == (ptraddr)allocBH);


				// 살짝 메모리를 다른 곳에 저장해둠
				u64 tempMemSize = allocBH->blockSize - sizeof(BlockHeader);
				void* tempMem = gAllocatorManager->mStackAllocator.Alloc(tempMemSize);
				memcpy_s(
					tempMem, tempMemSize,
					(void*)((ptraddr)allocBH + sizeof(BlockHeader)), tempMemSize);


				BlockHeader newAllocBH;
				newAllocBH.blockSize = allocBH->blockSize;
				newAllocBH.handle = allocBH->handle;
				newAllocBH.prev = (ptraddr)prevBH;
				newAllocBH.next = (ptraddr)emptyBH + newAllocBH.blockSize;


				BlockHeader newEmptyBH;
				newEmptyBH.blockSize = emptyBH->blockSize;
				newEmptyBH.handle = 0;
				newEmptyBH.prev = (ptraddr)emptyBH;
				newEmptyBH.next = (ptraddr)nextBH;


				if (prevBH != nullptr)
					prevBH->next = newEmptyBH.prev;
				if (nextBH != nullptr)
				{
					nextBH->prev = newAllocBH.next;
					// 다음 블록이 비어져있다면 병합 시도 
					if (nextBH->handle == 0)
					{
						newEmptyBH.blockSize = newEmptyBH.blockSize + nextBH->blockSize;
						newEmptyBH.next = nextBH->next;
						if (nextBH->next != 0)
						{
							((BlockHeader*)(nextBH->next))->prev = newAllocBH.next;
						}
						if (!isCurrStartAddr && mCurrStartAddr == (ptraddr)nextBH) isCurrStartAddr = true;
					}
				}



				// 현재 블록 빈블록 스왑
				(*emptyBH) = newAllocBH;
				*((BlockHeader*)newAllocBH.next) = newEmptyBH;

	
				// 메모리 복제
				u64 allocMemSize = newAllocBH.blockSize - sizeof(BlockHeader);
				memcpy_s(
					(void*)((ptraddr)emptyBH + sizeof(BlockHeader)), allocMemSize, 
					tempMem, allocMemSize);

				// 임시 저장소 메모리 해제
				gAllocatorManager->mStackAllocator.DeAlloc(&tempMem);


				if (isCurrStartAddr) mCurrStartAddr = newAllocBH.next;


				// 메모리 핸들 메모리 주소 변경
				*memPtr = (ptraddr)emptyBH + sizeof(BlockHeader);

	


				emptyBlockAddr = newAllocBH.next;
				mMemoryDefragCurrAddr = newAllocBH.next;
				curPos = newEmptyBH.next;
			}
			// 비어있는 메모리 블럭이면. 
			else if(bh.handle == 0)
			{
				// 마지막 블록이면
				if (bh.next == 0)
				{
					curPos = 0;
					mMemoryDefragCurrAddr = 0;
				}
				else
				{
					emptyBlockAddr = curPos;
					mMemoryDefragCurrAddr = curPos;
					curPos += bh.blockSize;
				}

			}
			else
			{
				mMemoryDefragCurrAddr = curPos;
				curPos += bh.blockSize;
			}
		}
	}

	void JGHeapAllocator::Swap(ptraddr* prev, ptraddr* next)
	{
		BlockHeader* prevBH = (BlockHeader*)(*prev);
		BlockHeader* nextBH = (BlockHeader*)(*next);
	}



	JGPoolAllocator::JGPoolAllocator(u64 size, ptraddr address, u32 memoryUnit) : JGAllocator(size, address), mMemoryUnit(memoryUnit)
	{

		mMemoryCount = mTotalMemSize / mMemoryUnit;
		mHeadMemoryIndex = 0;
		mTailMemoryIndex = 0;
		mMemoryPool  = (ptraddr*)malloc(mMemoryCount * sizeof(ptraddr));
		if(mMemoryPool == nullptr)
			JASSERT("JGPoolAllocator::JGPoolAllocator : fail malloc");

		ptraddr currAddr = mStartAddress;
		for (u64 i = 0; i < mMemoryCount; ++i)
		{
			mMemoryPool[i] = currAddr;
			currAddr += mMemoryUnit;
		}



	}

	void* JGPoolAllocator::Alloc()
	{
		if (mUseMemSize > mTotalMemSize)
			JASSERT("JGPoolAllocator::DeAlloc : mUseMemSize > mTotalMemSize");


		//
		void* result = (void*)mMemoryPool[mHeadMemoryIndex++];
		mHeadMemoryIndex = mHeadMemoryIndex % mMemoryCount;
		mUseMemSize += mMemoryUnit;


		return result;
	}

	void JGPoolAllocator::DeAlloc(void** ptr)
	{
		if (mTailMemoryIndex == mHeadMemoryIndex)
			JASSERT("JGPoolAllocator::DeAlloc : LastMemoryIndex == mHeadMemoryIndex");
		


		mMemoryPool[mTailMemoryIndex++] = (ptraddr)(*ptr);
		mTailMemoryIndex = mTailMemoryIndex % mMemoryCount;
		mUseMemSize -= mMemoryUnit;


		*ptr = nullptr;
	}

	void JGPoolAllocator::Clear()
	{
		mUseMemSize = 0;
		mHeadMemoryIndex = 0;
		mTailMemoryIndex = 0;
	}

	ptraddr JGPoolAllocator::GetMemory(u64 index)
	{
		if (index >= mMemoryCount) JASSERT("JGPoolAllocator::GetMemory : index >= mMemoryCount"); 
		return mMemoryPool[index];
	}



	void* JGSingleFrameAllocator::Alloc(u64 size)
	{
		size = JGMath::AlignUp(size, sizeof(ptraddr));



		mUseMemSize += size;
		if (mUseMemSize > mTotalMemSize)
			JASSERT("JGSingleFrameAllocator::Alloc : mUseMemSize > mTotalMemSize");

		void* p = (void*)mCurrStartAddr;
		mCurrStartAddr += size;


		return p;
	}


	void JGSingleFrameAllocator::Clear()
	{
		mCurrStartAddr = mStartAddress;
		mUseMemSize   = 0;
	}

	JGDoubleFrameAllocator::JGDoubleFrameAllocator(u64 size, ptraddr address) : JGAllocator(size, address)
	{
		mHalfTotalMemSize  = mTotalMemSize / 2;
		mFramePage[0] = mStartAddress;
		mFramePage[1] = mStartAddress + mHalfTotalMemSize;
		mFrameUseMemSize[0] = 0;
		mFrameUseMemSize[1] = 0;
	}

	void* JGDoubleFrameAllocator::Alloc(u64 size)
	{
		size = JGMath::AlignUp(size, sizeof(ptraddr));


		ptraddr& currentFramePage    = mFramePage[mCurrentFrameIndex];
		u64& currentFrameUseMemSize = mFrameUseMemSize[mCurrentFrameIndex];


		mUseMemSize += size;
		currentFrameUseMemSize += size;
		if (currentFrameUseMemSize > mHalfTotalMemSize)
		{
			JASSERT("JGDoubleFrameAllocator::Alloc : currentFrameUseMemSize > mHalfTotalMemSize");
		}



		void* p = (void*)currentFramePage;
		currentFramePage += size;
	


		return p;
	}
	void JGDoubleFrameAllocator::Swap()
	{
		mCurrentFrameIndex = (mCurrentFrameIndex + 1) % 2;
		mFramePage[mCurrentFrameIndex] -= mFrameUseMemSize[mCurrentFrameIndex];
		mUseMemSize                    -= mFrameUseMemSize[mCurrentFrameIndex];
		mFrameUseMemSize[mCurrentFrameIndex] = 0;
	}






	void JGAllocatorManager::Create(const JGAllocatorDesc& desc)
	{
		if (gAllocatorManager) return;

		gAllocatorManager = new JGAllocatorManager(desc);


	}
	void JGAllocatorManager::Update()
	{
		gAllocatorManager->mHeapAllocator.MemoryDefragmenter(gAllocatorManager->mDesc.MemoryDefragmenterCountPerFrame);
		gAllocatorManager->mSingleFrameAllocator.Clear();
		gAllocatorManager->mDoubleFrameAllocator.Swap();


		// 
		if (gAllocatorManager->mDesc.IsSafeNull)
		{
			u64 headIndex = gAllocatorManager->mMemoryHandleAllocator[(u8)EJGMemoryHandleLocation::SingleFrame].GetHeadIndex();
			u64 tailIndex = gAllocatorManager->mMemoryHandleAllocator[(u8)EJGMemoryHandleLocation::SingleFrame].GetTailIndex();
			u64 memoryCount = gAllocatorManager->mMemoryHandleAllocator[(u8)EJGMemoryHandleLocation::SingleFrame].GetMemoryCount();

			u64 currIndex = tailIndex;
			while (headIndex != currIndex)
			{
				ptraddr* memHandlePtr = (ptraddr*)gAllocatorManager->mMemoryHandleAllocator[(u8)EJGMemoryHandleLocation::SingleFrame].GetMemory(currIndex);
				*memHandlePtr = 0;
				currIndex = (currIndex + 1) % memoryCount;
			}


			u64 frameIndex = gAllocatorManager->mDoubleFrameAllocator.GetCurrentFrameIndex();
			EJGMemoryHandleLocation location = (frameIndex == 0) ? EJGMemoryHandleLocation::DoubleFrame1 : EJGMemoryHandleLocation::DoubleFrame2;
		


			headIndex = gAllocatorManager->mMemoryHandleAllocator[(u8)location].GetHeadIndex();
			tailIndex = gAllocatorManager->mMemoryHandleAllocator[(u8)location].GetTailIndex();
			memoryCount = gAllocatorManager->mMemoryHandleAllocator[(u8)location].GetMemoryCount();
			currIndex = tailIndex;
			while (headIndex != currIndex)
			{
				ptraddr* memHandlePtr = (ptraddr*)gAllocatorManager->mMemoryHandleAllocator[(u8)location].GetMemory(currIndex);
				*memHandlePtr = 0;
				currIndex = (currIndex + 1) % memoryCount;
			}
		}

		// SingleFrame, DoubleFrame 메모리 핸들 Pool 할당자 초기화
		gAllocatorManager->mMemoryHandleAllocator[(u8)EJGMemoryHandleLocation::SingleFrame].Clear();
		u64 frameIndex = gAllocatorManager->mDoubleFrameAllocator.GetCurrentFrameIndex();
		(frameIndex == 0) ?
			gAllocatorManager->mMemoryHandleAllocator[(u8)EJGMemoryHandleLocation::DoubleFrame1].Clear() :
			gAllocatorManager->mMemoryHandleAllocator[(u8)EJGMemoryHandleLocation::DoubleFrame2].Clear();
		
	}
	void JGAllocatorManager::Destroy()
	{
		if (gAllocatorManager == nullptr) return;


		for (EJGMemoryHandleLocation i = EJGMemoryHandleLocation::Stack; i < EJGMemoryHandleLocation::Count;)
		{
			gAllocatorManager->mMemoryHandleAllocator[(i32)i].Destroy();
			i = (EJGMemoryHandleLocation)((i32)i + 1);
		}


		delete gAllocatorManager;
		gAllocatorManager = nullptr;
	}

	JGAllocatorManager* JGAllocatorManager::GetInstance()
	{
		return gAllocatorManager;
	}

	JGStackAllocatorDebugInfo JGAllocatorManager::GetStackAllocatorDebugInfo()
	{
		JGStackAllocatorDebugInfo info;
		auto& stackAllocator = gAllocatorManager->mStackAllocator;


		info.TotalMemory = stackAllocator.GetTotalMemory();
		info.UseMemory = stackAllocator.GetUseMemory();
		info.StartAddress = stackAllocator.GetStartPtr();
		info.LastAddress = info.StartAddress + info.TotalMemory;
		info.TopAddress = stackAllocator.GetTopPtr();




		return info;
	}

	JGLinearAllocatorDebugInfo JGAllocatorManager::GetLinearAllocatorDebugInfo()
	{
		JGLinearAllocatorDebugInfo info;
		auto& linearAllocator = gAllocatorManager->mLinearAllocator;


		info.TotalMemory = linearAllocator.GetTotalMemory();
		info.UseMemory = linearAllocator.GetUseMemory();
		info.StartAddress = linearAllocator.GetStartPtr();
		info.LastAddress = info.StartAddress + info.TotalMemory;
		info.CurrAddress = linearAllocator.GetCurrentPtr();




		return info;
	}

	JGMemoryHandle JGAllocatorManager::StackAlloc(u64 size)
	{
		JGMemoryHandle handle = JGAllocatorManager::AllocMemoryHandle(EJGMemoryHandleLocation::Stack);
		*(handle.mPtr) = (ptraddr)gAllocatorManager->mStackAllocator.Alloc(size);


		return std::move(handle);
	}

	JGMemoryHandle JGAllocatorManager::LinearAlloc(u64 size)
	{
		JGMemoryHandle handle = JGAllocatorManager::AllocMemoryHandle(EJGMemoryHandleLocation::Linear);
		*(handle.mPtr) = (ptraddr)gAllocatorManager->mLinearAllocator.Alloc(size);


		return std::move(handle);
	}

	JGMemoryHandle JGAllocatorManager::HeapAlloc(u64 size)
	{
		JGMemoryHandle handle = JGAllocatorManager::AllocMemoryHandle(EJGMemoryHandleLocation::Heap);
		*(handle.mPtr) = (ptraddr)gAllocatorManager->mHeapAllocator.Alloc(size);
		auto bh = (JGHeapAllocator::BlockHeader*)((*(handle.mPtr)) - sizeof(JGHeapAllocator::BlockHeader));
		bh->handle = (ptraddr)handle.mPtr;
		return std::move(handle);
	}

	JGMemoryHandle JGAllocatorManager::SingleFrameAlloc(u64 size)
	{
		JGMemoryHandle handle = JGAllocatorManager::AllocMemoryHandle(EJGMemoryHandleLocation::SingleFrame);
		*(handle.mPtr) = (ptraddr)gAllocatorManager->mSingleFrameAllocator.Alloc(size);



		return handle;
	}

	JGMemoryHandle JGAllocatorManager::DoubleFrameAlloc(u64 size)
	{
		auto index = gAllocatorManager->mDoubleFrameAllocator.GetCurrentFrameIndex();
		EJGMemoryHandleLocation frameLocation = (index == 0) ? EJGMemoryHandleLocation::DoubleFrame1 : EJGMemoryHandleLocation::DoubleFrame2;
		JGMemoryHandle handle = JGAllocatorManager::AllocMemoryHandle(frameLocation);
		*(handle.mPtr) = (ptraddr)gAllocatorManager->mDoubleFrameAllocator.Alloc(size);

		
		return handle;
	}

	void JGAllocatorManager::DeAlloc(JGMemoryHandle& handle)
	{
		
		if (handle.mLocation == EJGMemoryHandleLocation::Count) return;
		if (handle.Get() == nullptr) return;
		//std::make_unique
		switch (handle.mLocation)
		{
		case EJGMemoryHandleLocation::Stack:
			gAllocatorManager->mStackAllocator.DeAlloc((void**)handle.mPtr);
			gAllocatorManager->mMemoryHandleAllocator[(u8)handle.mLocation].DeAlloc((void**)&handle.mPtr);
			break;
		case EJGMemoryHandleLocation::Heap:
			gAllocatorManager->mHeapAllocator.DeAlloc((void**)handle.mPtr);
			gAllocatorManager->mMemoryHandleAllocator[(u8)handle.mLocation].DeAlloc((void**)&handle.mPtr);
			break;
		case EJGMemoryHandleLocation::Linear:
		case EJGMemoryHandleLocation::SingleFrame:
		case EJGMemoryHandleLocation::DoubleFrame1:
		case EJGMemoryHandleLocation::DoubleFrame2:
			break;
		}


	}




	JGAllocatorManager::JGAllocatorManager(const JGAllocatorDesc& desc) : mDesc(desc)
	{
		u64 memoryHandleCount = 0;
		for (EJGMemoryHandleLocation i = EJGMemoryHandleLocation::Stack; i < EJGMemoryHandleLocation::Count;)
		{
			memoryHandleCount += mDesc.MemoryHandleCount[(i32)i];

			i = (EJGMemoryHandleLocation)((i32)i + 1);
		}

	


		u64 memoryHandleMemSize = (memoryHandleCount * sizeof(ptraddr));



		mTotalMemory = memoryHandleMemSize + mDesc.StackAllocMem + mDesc.LinearAllocMem + mDesc.HeapAllocMem + mDesc.SingleFrameAllocMem + mDesc.DoubleBufferedAllocMem;

		// 메모리 할당
		mStartAddress = malloc(mTotalMemory);

		ptraddr startAddr = (ptraddr)mStartAddress;

		// Proxy, Stack, Linear 제일 우선
		mStackAllocator  = JGStackAllocator(mDesc.StackAllocMem, startAddr);   startAddr += mDesc.StackAllocMem;
		mLinearAllocator = JGLinearAllocator(mDesc.LinearAllocMem, startAddr); startAddr += mDesc.LinearAllocMem;


		// Heap, Single, Double 
		mHeapAllocator        = JGHeapAllocator(mDesc.HeapAllocMem, startAddr);                 startAddr += mDesc.HeapAllocMem;
		mSingleFrameAllocator = JGSingleFrameAllocator(mDesc.SingleFrameAllocMem,startAddr);    startAddr += mDesc.SingleFrameAllocMem;
		mDoubleFrameAllocator = JGDoubleFrameAllocator(mDesc.DoubleBufferedAllocMem,startAddr); startAddr += mDesc.DoubleBufferedAllocMem;



		for (EJGMemoryHandleLocation i = EJGMemoryHandleLocation::Stack; i < EJGMemoryHandleLocation::Count;)
		{
			u64 memoryHandleMemSize = mDesc.MemoryHandleCount[(i32)i] * sizeof(ptraddr);
			mMemoryHandleAllocator[(i32)i] = JGPoolAllocator(memoryHandleMemSize, startAddr, sizeof(ptraddr));
			startAddr += memoryHandleMemSize;
			i = (EJGMemoryHandleLocation)((i32)i + 1);
		}
	}
	JGAllocatorManager::~JGAllocatorManager()
	{

		free(mStartAddress);
	}

	JGMemoryHandle JGAllocatorManager::AllocMemoryHandle(EJGMemoryHandleLocation location)
	{
		JGMemoryHandle handle;
		handle.mPtr = (ptraddr*)gAllocatorManager->mMemoryHandleAllocator[(i32)location].Alloc();
		handle.mLocation = location;

		return std::move(handle);
	}










}

