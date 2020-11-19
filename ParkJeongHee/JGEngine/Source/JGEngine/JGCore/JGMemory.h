#pragma once
#include "Define.h"
#include "TypeDefine.h"

/*
1. 메모리 할당
2. 타이머
3. 컨테이너
4. 문자열
5. 


*/



namespace JG
{
	template<class T>
	class JGBasePtr
	{
	public:
		virtual ~JGBasePtr() = default;
	};


	template<class T>
	class JGUniquePtr : public JGBasePtr<T>
	{
	public:
		virtual ~JGUniquePtr() = default;


	};

	template<class T>
	class JGWeakPtr : public JGBasePtr<T>
	{
	public:
		virtual ~JGWeakPtr() = default;
	};

	template<class T>
	class JGSharedPtr : public JGBasePtr<T>
	{
	public:
		virtual ~JGSharedPtr() = default;
	};



	class JGAllocator
	{
	public:
		JGAllocator() = default;
		JGAllocator(u64 size, ptraddr address) : mStartAddress(address), mTotalMemSize(size) {}
		virtual ~JGAllocator() = default;

	public:
		void* GetStartPtr() const {
			return (void*)mStartAddress;
		}
		u64 GetUseMemory() const {
			return mUseMemSize;
		}
		u64 GetTotalMemory() const {
			return mTotalMemSize;
		}
	protected:
		ptraddr mStartAddress  = 0;
		u64   mTotalMemSize    = 0;
		u64   mUseMemSize      = 0;
	};





	/*
	* Stack 형식의 할당자 / DeAlloc 시 Alloc을 한 반대 순으로 해야함.
	*/
	class JGStackAllocator : public JGAllocator
	{
	public:
		JGStackAllocator() = default;
		JGStackAllocator(u64 size, ptraddr address) : JGAllocator(size, address), mTopAddress(address) {}
		virtual ~JGStackAllocator() = default;
	public:
		void* Alloc(u64 size);
		void  DeAlloc(void** ptr);
		void* GetTopPtr() const {
			return (void*)mTopAddress;
		}
	private:
		ptraddr mTopAddress = 0;
	};


	/* 
	* 할당만 해주는 할당자, 할당 해제 불가
	*/
	class JGLinearAllocator : public JGAllocator
	{
	public:
		JGLinearAllocator() = default;
		JGLinearAllocator(u64 size, ptraddr address) : JGAllocator(size, address), mCurrentAddress(address) {}
		virtual ~JGLinearAllocator() = default;
	public:
		void* Alloc(u64 size);
		void* GetCurrentPtr() const {
			return (void*)mCurrentAddress;
		}
	private:
		ptraddr mCurrentAddress = 0;
	};

	/*
	* 자유 형식의 할당자 
	*/
	class JGHeapAllocator : public JGAllocator
	{
	public:
		JGHeapAllocator() = default;
		JGHeapAllocator(u64 size, ptraddr address);
		virtual ~JGHeapAllocator() = default;
	public:
		void* Alloc(u64 size);
		void  DeAlloc(void** ptr);
		void  MemoryDefragmenter(u64 count_per_frame);
	private:
		void Swap(ptraddr* prev, ptraddr* next);
	private:
		struct BlockHeader
		{
			ptraddr prev;         // 전 블록의 주솟값
			ptraddr next;         // 다음 블록의 주솟값
			u64     blockSize;    // 블록 사이즈(전체)
			bool    used = false; // 사용 했나 안했나
			byte    padding[7];   //
		};
		ptraddr mCurrStartAddr = 0;
	};

	/*
	메모리 풀 할당자
	*/

	class JGPoolAllocator : public JGAllocator
	{
	public:
		JGPoolAllocator() = default;
		JGPoolAllocator(u64 size, ptraddr address, u32 memoryUnit);
		virtual ~JGPoolAllocator() = default;
	public:
		void* Alloc();
		void DeAlloc(void** ptr);
	private:
		ptraddr* mMemoryPool = nullptr;
		u64 mMemoryCount = 0;
		u32 mMemoryUnit  = 0;
	};

	/*
	* 1프레임만 살아있는 메모리를 할당하는 할당자
	*/
	class JGSingleFrameAllocator : public JGAllocator
	{
	public:
		JGSingleFrameAllocator() = default;
		JGSingleFrameAllocator(u64 size, ptraddr address) : JGAllocator(size, address), mCurrStartAddr(address) {}
		virtual ~JGSingleFrameAllocator() = default;
	public:
		void* Alloc(u64 size);
		void Clear();

	public:
		void* GetCurrStartPtr() const {
			return (void*)mCurrStartAddr;
		}
	private:
		ptraddr mCurrStartAddr = 0;
	};

	class JGDoubleFrameAllocator : public JGAllocator
	{
	public:
		JGDoubleFrameAllocator() = default;
		JGDoubleFrameAllocator(u64 size, ptraddr address);
		virtual ~JGDoubleFrameAllocator() = default;
	public:
		void* Alloc(u64 size);
		void Swap();


	private:
		ptraddr mFramePage[2];
		u64		mFrameUseMemSize[2];
		u64     mHalfTotalMemSize = 0;
		u8 mCurrentFrameIndex = 0;
	};

	struct JGAllocatorDesc
	{
		u64 stackAllocMem   = 0;
		u64 linearAllocMem  = 0;
		u64 heapAllocMem    = 0;
		u64 singleFrameAllocMem    = 0;
		u64 doubleBufferedAllocMem = 0;
		u64 memoryHandleAllocMem = _KB;
	};

	class JGAllocatorManager
	{
		friend JGPoolAllocator;

	public:
		static void Create(const JGAllocatorDesc& desc);
		static void Update();
		static void Destroy();
	public:
		JGAllocatorManager(const JGAllocatorDesc& desc);
		~JGAllocatorManager();
	private:
		void*   mStartAddress = nullptr;
		ptraddr mTotalMemory;
		JGAllocatorDesc mDesc;



		JGStackAllocator       mStackAllocator;
		JGLinearAllocator      mLinearAllocator;
		JGHeapAllocator        mHeapAllocator;
		JGSingleFrameAllocator mSingleFrameAllocator;
		JGDoubleFrameAllocator mDoubleFrameAllocator;
	};








}