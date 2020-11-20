#include "JGMemory.h"
#include "JGMath.h"
namespace JG
{
	static JGAllocatorManager* gAllocatorManager = nullptr;


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
		mMemoryPool  = (ptraddr*)gAllocatorManager->mLinearAllocator.Alloc(mMemoryCount * sizeof(ptraddr));



		ptraddr curAddr = mStartAddress;
		for (u64 i = 0; i < mMemoryCount; ++i)
		{
			mMemoryPool[i] = curAddr;
			curAddr += mMemoryUnit;
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



	JGAllocatorManager::JGAllocatorManager(const JGAllocatorDesc& desc) : mDesc(desc)
	{

		mTotalMemory = desc.stackAllocMem + desc.linearAllocMem + desc.heapAllocMem + desc.singleFrameAllocMem + desc.doubleBufferedAllocMem;

		mStartAddress = malloc(mTotalMemory);

		ptraddr startAddr = (ptraddr)mStartAddress;


		// Allocator 생성
		mStackAllocator  = JGStackAllocator(desc.stackAllocMem, startAddr);   startAddr += desc.stackAllocMem;
		mLinearAllocator = JGLinearAllocator(desc.linearAllocMem, startAddr); startAddr += desc.linearAllocMem;
		mHeapAllocator   = JGHeapAllocator(desc.heapAllocMem, startAddr);     startAddr += desc.heapAllocMem;
		mSingleFrameAllocator = JGSingleFrameAllocator(desc.singleFrameAllocMem, startAddr); startAddr    += desc.singleFrameAllocMem;
		mDoubleFrameAllocator = JGDoubleFrameAllocator(desc.doubleBufferedAllocMem, startAddr); startAddr += desc.doubleBufferedAllocMem;


		// mMemoryHandleAllocator = JGPoolAllocator();

	}
	JGAllocatorManager::~JGAllocatorManager()
	{

		free(mStartAddress);

	}








}

