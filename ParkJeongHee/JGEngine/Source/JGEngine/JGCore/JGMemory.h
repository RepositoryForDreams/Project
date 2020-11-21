#pragma once
#include "Define.h"
#include "TypeDefine.h"

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
		_4BytePool,
		_8BytePool,
		_12BytePool,
		_16BytePool,
		_32BytePool,
		_64BytePool,
		_128BytePool,

		Count
	};



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
	private:
		EJGMemoryHandleLocation mLocation = EJGMemoryHandleLocation::Count;
		u64 mRefCount = 0;
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
		void* GetCurrentPtr() const {
			return (void*)mCurrentAddress;
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
	private:
		struct BlockHeader
		{
			ptraddr prev = 0;         // �� ����� �ּڰ�
			ptraddr next = 0;         // ���� ����� �ּڰ�
			u64     blockSize = 0;    // ��� ������(��ü)
			bool    used = false; // ��� �߳� ���߳�
			byte    padding[7] = { 0,0,0,0,0,0,0 };   //
		};
		ptraddr mCurrStartAddr = 0;
	};

	/*
	�޸� Ǯ �Ҵ���
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
		u64 mCurrMemoryIndex = 0;
		u64 mLastMemoryIndex = 0;
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
			_MB,_MB,_MB,_MB,_MB,_MB,_MB,_MB,_MB,_MB,_MB,_MB, _MB
		};


		u64 _4BitMemoryCount = 0;
		u64 _8BitMemoryCount = 0;
		u64 _12BitMemoryCount = 0;
		u64 _16BitMemoryCount = 0;
		u64 _32BitMemoryCount = 0;
		u64 _64BitMemoryCount = 0;
		u64 _128BitMemoryCount = 0;

		//
		bool IsSafeNull = true;
	};

	struct JGLinearAllocatorDebugInfo
	{
		u64 TotalMemory    = 0;
		u64 TotalUseMemory = 0;
		ptraddr StartAddress = 0;
		ptraddr LastAddress  = 0;
		ptraddr CurrentAddress = 0;
	};
	struct JGStackAllocatorDebugInfo
	{
		u64 TotalMemory = 0;
		u64 TotalUseMemory = 0;
		ptraddr StartAddress = 0;
		ptraddr LastAddress = 0;
		ptraddr TopAddress = 0;
	};
	
	struct JGHeapAllocatorDebugInfo
	{
		struct JGMemoryBlock
		{
			ptraddr prev = 0;
			ptraddr next = 0;
			u64     size = 0;
			bool    used = false;
		};
		u64 TotalMemory = 0;
		u64 TotalUseMemory = 0;
		ptraddr StartAddress = 0;
		ptraddr LastAddress = 0;
		ptraddr CurrentAddress = 0;


		u64 MemoryBlockCount = 0;
		JGMemoryBlock* MemoryBlocks;
	};

	struct JGPoolAllocatorDebugInfo
	{
		u64 MemoryUnit     = 0;
		u64 TotalMemory    = 0;
		u64 TotalUseMemory = 0;
	};

	struct JGMemoryDebugInfo
	{
		u64 TotalMemory    = 0;
		u64 TotalUseMemory = 0;

		JGLinearAllocatorDebugInfo LinearDebugInfo;
		JGStackAllocatorDebugInfo  StackDebugInfo;
		JGHeapAllocatorDebugInfo   HeapDebugInfo;
		u64 PoolCount = 0;
		JGPoolAllocatorDebugInfo* PoolDebugInfos;

	};

	/*

	������ҿ� �´� �޸� �ڵ� �Ҵ��ڸ� �������ִ�.
	
	����� ����϶��� �ʱ�ȭ�Ҷ����� �޸��ڵ��� �̿��Ͽ� null�� �ʱ�ȭ����
	
	*/
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
		static JGMemoryHandle MemoryPoolAlloc(u64 size);
		static void DeAlloc(JGMemoryHandle handle);
	public:
		JGAllocatorManager(const JGAllocatorDesc& desc);
		~JGAllocatorManager();
	private:
		static JGMemoryHandle AllocMemoryHandle(EJGMemoryHandleLocation location);
	private:
		void*   mStartAddress = nullptr;
		ptraddr mTotalMemory;
		JGAllocatorDesc mDesc;

		JGLinearAllocator      mProxyAllocator;
		JGStackAllocator       mStackAllocator;
		JGLinearAllocator      mLinearAllocator;
		JGHeapAllocator        mHeapAllocator;
		JGSingleFrameAllocator mSingleFrameAllocator;
		JGDoubleFrameAllocator mDoubleFrameAllocator;


	
		JGPoolAllocator m4bytePoolAllocator;
		JGPoolAllocator m8bytePoolAllocator;
		JGPoolAllocator m12bytePoolAllocator;
		JGPoolAllocator m16bytePoolAllocator;
		JGPoolAllocator m32bytePoolAllocator;
		JGPoolAllocator m64bytePoolAllocator;
		JGPoolAllocator m128bytePoolAllocator;

		JGPoolAllocator mMemoryHandleAllocator[(i32)EJGMemoryHandleLocation::Count];
	};
}