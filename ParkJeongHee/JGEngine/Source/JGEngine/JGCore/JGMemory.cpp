#include "JGMemory.h"
#include "JGMath.h"
namespace JG
{
	static JGAllocatorManager* gAllocatorManager = nullptr;

	JGMemoryHandle::JGMemoryHandle(JGMemoryHandle&& rhs) noexcept
	{
		mPtr = rhs.mPtr;
		mLocation = rhs.mLocation;
		mRefCount = rhs.mRefCount;


		rhs.mPtr = nullptr;
		rhs.mLocation = EJGMemoryHandleLocation::Count;
		rhs.mRefCount = 0;
	}

	JGMemoryHandle& JGMemoryHandle::operator=(JGMemoryHandle&& rhs) noexcept
	{
		mPtr = rhs.mPtr;
		mLocation = rhs.mLocation;
		mRefCount = rhs.mRefCount;
		rhs.mPtr      = nullptr;
		rhs.mLocation = EJGMemoryHandleLocation::Count;
		rhs.mRefCount = 0;
		return *this;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	JGMemoryHandle::~JGMemoryHandle()
	{
		if (mPtr == nullptr) return;

		--mRefCount;
		if (mRefCount == 0) {
			gAllocatorManager->mMemoryHandleAllocator[(i32)mLocation].DeAlloc((void**)mPtr);
		}
	}

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
		startBH->used = false;
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
			if (startBH.used == false && startBH.blockSize >= size + bhSize)
			{
				u64 leftSize  = size + bhSize;
				u64 rightSize = startBH.blockSize - leftSize;


				// 기본 메모리보다 크기가 작다면 통합 할당
				if (rightSize < bhSize + sizeof(ptraddr))
				{
					BlockHeader bh;
					bh.blockSize = leftSize + rightSize;
					bh.used = true;
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
					leftBH.used = true;
					leftBH.prev = startBH.prev;
					leftBH.next = startAddr + leftBH.blockSize;
					*(BlockHeader*)startAddr = leftBH;

					



					BlockHeader rightBH;
					rightBH.blockSize = rightSize;
					rightBH.used = false;
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
			// 양쪽 병합
			if (prevBH->used == false && nextBH->used == false)
			{
				prevBH->blockSize = prevBH->blockSize + startBH->blockSize + nextBH->blockSize;
				prevBH->prev = prevBH->prev;
				prevBH->next = nextBH->next;
				prevBH->used = false;

				if (nextBH->next != 0)
				{
					((BlockHeader*)(nextBH->next))->prev = (ptraddr)prevBH;
				}
				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)prevBH;
			}
			// 오른쪽 병합
			else if (prevBH->used == true && nextBH->used == false)
			{
				startBH->blockSize = startBH->blockSize + nextBH->blockSize;
				startBH->prev = startBH->prev;
				startBH->next = nextBH->next;
				startBH->used = false;

				
				if (nextBH->next != 0)
				{
					((BlockHeader*)(nextBH->next))->prev = (ptraddr)startBH;
				}
				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)startBH;
			}
			// 왼쪽 병합
			else if (prevBH->used == false && nextBH->used == true)
			{
				prevBH->blockSize = prevBH->blockSize + startBH->blockSize;
				prevBH->prev      = prevBH->prev;
				prevBH->next      = startBH->next;
				prevBH->used      = false;

				nextBH->prev = (ptraddr)prevBH;

				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)prevBH;
			}
			// 병합 X
			else
			{
				startBH->used = false;
			}

		}
		// 처음 블럭
		else if (startBH->prev == 0 && startBH->next != 0)
		{
			// 다음 블럭이 할당된 블럭인지 확인
			BlockHeader* nextBH = (BlockHeader*)startBH->next;
			bool isCurrStartAddr = (mCurrStartAddr == (ptraddr)startBH ||  mCurrStartAddr == (ptraddr)nextBH);
			if (nextBH->used == false)
			{
				// 합병 시작
				BlockHeader bh;
				startBH->blockSize = startBH->blockSize + nextBH->blockSize;
				startBH->prev = startBH->prev;
				startBH->next = nextBH->next;
				startBH->used = false;

				((BlockHeader*)(nextBH->next))->prev = (ptraddr)startBH;
				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)startBH;
			}
			else
			{
				// 그냥 할당만 해제
				startBH->used = false;
			}
		}
		// 마지막 블럭
		else if (startBH->next == 0 && startBH->prev != 0)
		{
			BlockHeader* prevBH = (BlockHeader*)startBH->prev;
			bool isCurrStartAddr = (mCurrStartAddr == (ptraddr)startBH || mCurrStartAddr == (ptraddr)prevBH);
			if(prevBH->used == false)
			{
				prevBH->blockSize = prevBH->blockSize + startBH->blockSize;
				prevBH->prev = prevBH->prev;
				prevBH->next = startBH->next;
				prevBH->used = false;

				if (isCurrStartAddr) mCurrStartAddr = (ptraddr)prevBH;
			}
			else
			{
				startBH->used = false;
			}

		}
		else
		{
			startBH->used = false;
		}

		mUseMemSize -= deAllocatedBlockSize;
		(*ptr) = nullptr;
	}

	void JGHeapAllocator::MemoryDefragmenter(u64 countPerFrame)
	{
		BlockHeader* bh = (BlockHeader*)mStartAddress;
		u64 blockCnt = 0;
		ptraddr transPoint = 0;
		while (blockCnt < countPerFrame)
		{
			if (bh->used == false)
			{
				transPoint = (ptraddr)bh;
			}
			else
			{
				if (transPoint != 0)
				{

					Swap(&transPoint, (ptraddr*)(&bh));
					

				}
			}
			bh = (BlockHeader*)bh->next;
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
		mCurrMemoryIndex = 0;
		mLastMemoryIndex = 0;
		mMemoryPool  = (ptraddr*)gAllocatorManager->mProxyAllocator.Alloc(mMemoryCount * sizeof(ptraddr));



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
		void* result = (void*)mMemoryPool[mCurrMemoryIndex++];
		mCurrMemoryIndex = mCurrMemoryIndex % mMemoryCount;
		mUseMemSize += mMemoryUnit;


		return result;
	}

	void JGPoolAllocator::DeAlloc(void** ptr)
	{
		if (mLastMemoryIndex == mCurrMemoryIndex)
			JASSERT("JGPoolAllocator::DeAlloc : LastMemoryIndex == CurrMemoryIndex");
		


		mMemoryPool[mLastMemoryIndex++] = (ptraddr)(*ptr);
		mLastMemoryIndex = mLastMemoryIndex % mMemoryCount;
		mUseMemSize -= mMemoryUnit;


		*ptr = nullptr;
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
		gAllocatorManager->mSingleFrameAllocator.Clear();
		gAllocatorManager->mDoubleFrameAllocator.Swap();

	}
	void JGAllocatorManager::Destroy()
	{
		if (gAllocatorManager == nullptr) return;


		delete gAllocatorManager;
		gAllocatorManager = nullptr;
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

	JGMemoryHandle JGAllocatorManager::MemoryPoolAlloc(u64 size)
	{



		return JGMemoryHandle();
	}

	void JGAllocatorManager::DeAlloc(JGMemoryHandle handle)
	{
		//std::make_unique
	}



	JGAllocatorManager::JGAllocatorManager(const JGAllocatorDesc& desc) : mDesc(desc)
	{
		// Proxy 할당자 메모리 계산
		/*
		1. 메모리 풀 할당
		2. 메모리 핸들 할당
		*/
		u64 memoryHandleCount = 0;
		for (EJGMemoryHandleLocation i = EJGMemoryHandleLocation::Stack; i < EJGMemoryHandleLocation::Count;)
		{
			memoryHandleCount += mDesc.MemoryHandleCount[(i32)i];

			i = (EJGMemoryHandleLocation)((i32)i + 1);
		}

		u64 _4bitMemSize   = mDesc._4BitMemoryCount * 4;
		u64 _8bitMemSize   = mDesc._8BitMemoryCount * 8;
		u64 _12bitMemSize  = mDesc._12BitMemoryCount * 12;
		u64 _16bitMemSize  = mDesc._16BitMemoryCount * 16;
		u64 _32bitMemSize  = mDesc._32BitMemoryCount * 32;
		u64 _64bitMemSize  = mDesc._64BitMemoryCount * 64;
		u64 _128bitMemSize = mDesc._128BitMemoryCount * 128;
	


		u64 allPoolMemSize  = 
			_4bitMemSize  + _8bitMemSize + _12bitMemSize + _16bitMemSize +
			_32bitMemSize +_64bitMemSize + _128bitMemSize;

		u64 allPoolMemCount =
			mDesc._4BitMemoryCount  + mDesc._8BitMemoryCount  + mDesc._12BitMemoryCount  + mDesc._16BitMemoryCount + 
			mDesc._32BitMemoryCount + mDesc._64BitMemoryCount + mDesc._128BitMemoryCount;




		u64 proxyAllocMem = ((allPoolMemCount + memoryHandleCount) * sizeof(ptraddr));



		mTotalMemory = proxyAllocMem + mDesc.StackAllocMem + mDesc.LinearAllocMem + mDesc.HeapAllocMem + mDesc.SingleFrameAllocMem + mDesc.DoubleBufferedAllocMem + allPoolMemSize;

		// 메모리 할당
		mStartAddress = malloc(mTotalMemory);

		ptraddr startAddr = (ptraddr)mStartAddress;


		// Allocator 생성


		// Proxy, Stack, Linear 제일 우선
		mProxyAllocator  = JGLinearAllocator(proxyAllocMem, startAddr); startAddr += proxyAllocMem;
		mStackAllocator  = JGStackAllocator(mDesc.StackAllocMem, startAddr);   startAddr += mDesc.StackAllocMem;
		mLinearAllocator = JGLinearAllocator(mDesc.LinearAllocMem, startAddr); startAddr += mDesc.LinearAllocMem;


		// Heap, Single, Double 
		mHeapAllocator        = JGHeapAllocator(mDesc.HeapAllocMem, startAddr);                 startAddr += mDesc.HeapAllocMem;
		mSingleFrameAllocator = JGSingleFrameAllocator(mDesc.SingleFrameAllocMem,startAddr);    startAddr += mDesc.SingleFrameAllocMem;
		mDoubleFrameAllocator = JGDoubleFrameAllocator(mDesc.DoubleBufferedAllocMem,startAddr); startAddr += mDesc.DoubleBufferedAllocMem;


		// 메모리 풀 할당자 생성
		m4bytePoolAllocator   = JGPoolAllocator(_4bitMemSize, startAddr, 4); startAddr += _4bitMemSize;
		m8bytePoolAllocator   = JGPoolAllocator(_8bitMemSize, startAddr, 8); startAddr += _8bitMemSize;
		m12bytePoolAllocator  = JGPoolAllocator(_12bitMemSize, startAddr, 12); startAddr += _12bitMemSize;
		m16bytePoolAllocator  = JGPoolAllocator(_16bitMemSize, startAddr, 16); startAddr += _16bitMemSize;
		m32bytePoolAllocator  = JGPoolAllocator(_32bitMemSize, startAddr, 32); startAddr += _32bitMemSize;
		m64bytePoolAllocator  = JGPoolAllocator(_64bitMemSize, startAddr, 64); startAddr += _64bitMemSize;
		m128bytePoolAllocator = JGPoolAllocator(_128bitMemSize, startAddr, 128); startAddr += _128bitMemSize;



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
		handle.mRefCount = 1;

		return std::move(handle);
	}










}

