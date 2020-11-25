#pragma once
#include "Define.h"
#include "TypeDefine.h"
#include "Abstract.h"

/*
1. �޸� �Ҵ�
2. Ÿ�̸�
3. �����̳�
4. ���ڿ�
5. 


*/



namespace JG
{
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

	public:
		JGMemoryHandle(const JGMemoryHandle& copy) = delete;
		JGMemoryHandle& operator=(const JGMemoryHandle& copy) = delete;
	public:
		JGMemoryHandle() = default;
		JGMemoryHandle(JGMemoryHandle&& rhs) noexcept;
		JGMemoryHandle& operator=(JGMemoryHandle&& rhs) noexcept;
		~JGMemoryHandle();
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
	* Stack ������ �Ҵ��� / DeAlloc �� Alloc�� �� �ݴ� ������ �ؾ���.
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
	* �Ҵ縸 ���ִ� �Ҵ���, �Ҵ� ���� �Ұ�
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
	* ���� ������ �Ҵ��� 
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
			ptraddr prev = 0;         // �� ����� �ּڰ�
			ptraddr next = 0;         // ���� ����� �ּڰ�
			u64     blockSize = 0;    // ��� ������(��ü)
			ptraddr handle = 0;
		};
	private:
		ptraddr mCurrStartAddr = 0;
		ptraddr mMemoryDefragAddr = 0;
	};

	/*
	�޸� Ǯ �Ҵ���
	*/

	class JGPoolAllocator : public JGAllocator
	{
	public:
		JGPoolAllocator() = default;
		JGPoolAllocator(u64 size, ptraddr address, u32 memoryUnit);
	public:
		void* Alloc();
		void DeAlloc(void** ptr);

		u64 GetHeadIndex() const {
			return mHeadMemoryIndex;
		}
		u64 GetTailIndex() const {
			return mTailMemoryIndex;
		}
		u64 GetMemoryUnit() const {
			return mMemoryUnit;
		}
		void Destroy() {
			free(mMemoryPool);
			mMemoryPool = nullptr;
		}
	private:
		ptraddr* mMemoryPool = nullptr;
		u64 mMemoryCount = 0;
		u32 mMemoryUnit  = 0;
		u64 mHeadMemoryIndex = 0;
		u64 mTailMemoryIndex = 0;
	};

	/*
	* 1�����Ӹ� ����ִ� �޸𸮸� �Ҵ��ϴ� �Ҵ���
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


		static JGMemoryHandle StackAlloc(u64 size);
		static JGMemoryHandle LinearAlloc(u64 size);
		static JGMemoryHandle HeapAlloc(u64 size);
		static JGMemoryHandle SingleFrameAlloc(u64 size);
		static JGMemoryHandle DoubleFrameAlloc(u64 size);
		static void DeAlloc(JGMemoryHandle handle);


		static JGStackAllocatorDebugInfo  GetStackAllocatorDebugInfo();
		static JGLinearAllocatorDebugInfo GetLinearAllocatorDebugInfo();
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
}