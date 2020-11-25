#pragma once
#include "Define.h"
#include "TypeDefine.h"
#include "Abstract.h"

/*
1. 메모리 할당
2. 타이머
3. 컨테이너
4. 문자열
5. 


*/



namespace JG
{
	/*
	UniquePtr 구현
	SharedPtr 구현
	WeakPtr 구현
	메모리 누수 감지 구현
	할당 시 생성자 소멸자 하도록 구현
	
	
	
	*/
	enum class EJGMemoryHandleLocation : u8
	{
		Stack,
		Linear,
		Heap,
		SingleFrame,
		DoubleFrame1,
		DoubleFrame2,
		Count
	};





	class JGMemoryHandle 
	{
		friend class JGAllocatorManager;
		template<class T>
		friend class JGUniquePtr;
		template<class T>
		friend class JGWeakPtr;
		template<class T>
		friend class JGSharedPtr;
	public:
		JGMemoryHandle()  = default;
		~JGMemoryHandle() = default;
	public:
		void* Get() const;
		bool  IsValid() const {
			return Get() != nullptr;
		}
	private:
		EJGMemoryHandleLocation mLocation = EJGMemoryHandleLocation::Count;
		ptraddr* mPtr = nullptr;
	};


	class JGAllocator
	{
	public:
		JGAllocator() = default;
		JGAllocator(u64 size, ptraddr address) : mStartAddress(address), mTotalMemSize(size) {}
		virtual ~JGAllocator() = default;

	public:
		ptraddr GetStartPtr() const {
			return mStartAddress;
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
		ptraddr GetTopPtr() const {
			return mTopAddress;
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
		ptraddr GetCurrentPtr() const {
			return mCurrentAddress;
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
	public:
		struct BlockHeader
		{
			ptraddr prev = 0;         // 전 블록의 주솟값
			ptraddr next = 0;         // 다음 블록의 주솟값
			ptraddr handle = 0;
			u64     blockSize = 0;    // 블록 사이즈(전체)

		};
	private:
		ptraddr mCurrStartAddr = 0;
		ptraddr mMemoryDefragCurrAddr = 0;
	};

	/*
	메모리 풀 할당자
	*/

	class JGPoolAllocator : public JGAllocator
	{
	public:
		JGPoolAllocator() = default;
		JGPoolAllocator(u64 size, ptraddr address, u32 memoryUnit);
	public:
		void* Alloc();
		void  DeAlloc(void** ptr);
		void  Clear();
		u64 GetHeadIndex() const {
			return mHeadMemoryIndex;
		}
		u64 GetTailIndex() const {
			return mTailMemoryIndex;
		}
		u64 GetMemoryUnit() const {
			return mMemoryUnit;
		}
		u64 GetMemoryCount() const {
			return mMemoryCount;
		}
		void Destroy() {
			free(mMemoryPool);
			mMemoryPool = nullptr;
		}
		ptraddr GetMemory(u64 index);
	private:
		ptraddr* mMemoryPool = nullptr;
		u64 mMemoryCount = 0;
		u32 mMemoryUnit  = 0;
		u64 mHeadMemoryIndex = 0;
		u64 mTailMemoryIndex = 0;
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
		u8 GetCurrentFrameIndex() const {
			return mCurrentFrameIndex;
		}

	private:
		ptraddr mFramePage[2] = { 0,0 };
		u64		mFrameUseMemSize[2] = { 0,0 };
		u64     mHalfTotalMemSize = 0;
		u8 mCurrentFrameIndex = 0;
	};











	template<class T>
	class JGUniquePtr
	{
	private:
		JGUniquePtr(const JGUniquePtr& copy) = delete;
		JGUniquePtr& operator=(const JGUniquePtr& copy) = delete;
	public:
		JGUniquePtr() = default;
		JGUniquePtr(JGUniquePtr&& rhs) {
			Reset();
			mHandle.mPtr      = rhs.mHandle.mPtr;
			mHandle.mLocation = rhs.mHandle.mLocation;

			rhs.mHandle.mPtr = nullptr;
			rhs.mHandle.mLocation = EJGMemoryHandleLocation::Count;
		}

		JGUniquePtr& operator=(JGUniquePtr&& rhs) {
			Reset();
			mHandle.mPtr = rhs.mHandle.mPtr;
			mHandle.mLocation = rhs.mHandle.mLocation;

			rhs.mHandle.mPtr = nullptr;
			rhs.mHandle.mLocation = EJGMemoryHandleLocation::Count;
			return *this;
		}

		T* operator->() {
			return (T*)mHandle.Get();
		}

		~JGUniquePtr()
		{
			Reset();
		}
	public:
		T* Get() {
			return (T*)mHandle.Get();
		}
		void Reset() {
			if (mHandle.Get()) {
				// 소멸자 호출되는지 확인해보기
				JGAllocatorManager::DeAlloc(mHandle);
			}
			mHandle.mPtr = nullptr;
			mHandle.mLocation = EJGMemoryHandleLocation::Count;
		}
	private:
		JGMemoryHandle mHandle;
	};


	template<class T>
	class JGSharedPtr
	{
	private:
		JGMemoryHandle mHandle;
	};


	template<class T>
	class JGWeakPtr
	{
	private:
		ptraddr* mPtr = 0;
	};

































	struct JGAllocatorDesc
	{
		u64 StackAllocMem   = 0;
		u64 LinearAllocMem  = 0;
		u64 HeapAllocMem    = 0;
		u64 SingleFrameAllocMem    = 0; 
		u64 DoubleBufferedAllocMem = 0;

		u64 MemoryHandleCount[(i32)EJGMemoryHandleLocation::Count] = {
			_MB,_MB,_MB,_MB,_MB,_MB,
		};

		u64 MemoryDefragmenterCountPerFrame = 1;
		//
		bool IsSafeNull = true;
	};
	class JGStackAllocatorDebugInfo
	{
	public:
		struct JGStackMemoryBlock
		{
			u64 Size = 0;
			ptraddr Address = 0;
		};
		u64 TotalMemory = 0;
		u64 UseMemory = 0;
		ptraddr StartAddress = 0;
		ptraddr LastAddress  = 0;
		ptraddr TopAddress = 0;
	};

	class JGLinearAllocatorDebugInfo
	{
	public:
		struct JGLinearMemoryBlock
		{
			u64 Size = 0;
			ptraddr Address = 0;
		};
		u64 TotalMemory = 0;
		u64 UseMemory = 0;
		ptraddr StartAddress = 0;
		ptraddr LastAddress = 0;
		ptraddr CurrAddress = 0;
	};

	class JGAllocatorManager
	{
		friend JGMemoryHandle::~JGMemoryHandle();
		friend JGStackAllocator;
		friend JGLinearAllocator;
		friend JGHeapAllocator;
		friend JGSingleFrameAllocator;
		friend JGDoubleFrameAllocator;
		friend JGPoolAllocator;
	public:
		static void Create(const JGAllocatorDesc& desc);
		static void Update();
		static void Destroy();



		static JGAllocatorManager* GetInstance();
	public:
		//template<class T>
		//JGUniquePtr CreateUniquePtr(JGObject)



	public:
		static JGStackAllocatorDebugInfo  GetStackAllocatorDebugInfo();
		static JGLinearAllocatorDebugInfo GetLinearAllocatorDebugInfo();
	private:
		static JGMemoryHandle StackAlloc(u64 size);
		static JGMemoryHandle LinearAlloc(u64 size);
		static JGMemoryHandle HeapAlloc(u64 size);
		static JGMemoryHandle SingleFrameAlloc(u64 size);
		static JGMemoryHandle DoubleFrameAlloc(u64 size);
		static void DeAlloc(JGMemoryHandle& handle);


	public:
		JGAllocatorManager(const JGAllocatorDesc& desc);
		~JGAllocatorManager();
	private:
		static JGMemoryHandle AllocMemoryHandle(EJGMemoryHandleLocation location);
	public:
		void*   mStartAddress = nullptr;
		ptraddr mTotalMemory;
		JGAllocatorDesc mDesc;

		JGStackAllocator       mStackAllocator;
		JGLinearAllocator      mLinearAllocator;
		JGHeapAllocator        mHeapAllocator;
		JGSingleFrameAllocator mSingleFrameAllocator;
		JGDoubleFrameAllocator mDoubleFrameAllocator;


		JGPoolAllocator mMemoryHandleAllocator[(i32)EJGMemoryHandleLocation::Count];
	};






	// CreateUniquePtr
	// CreateWeakPtr
	// CreateSharedPtr
}