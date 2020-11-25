#include <iostream>
#include <string>
#include <crtdbg.h>
#include "JGMemory.h"
#include "JGMath.h"
#pragma comment(lib, "../Bin/lib/Debug/JGCore.lib")

using namespace std;
using namespace JG;
struct test
{
	short a;
	int   b;
	bool  c;
};
struct BlockHeader
{
	ptraddr prev;         // �� ����� �ּڰ�
	ptraddr next;         // ���� ����� �ּڰ�
	ptraddr handle;
	u64     blockSize;    // ��� ������(��ü)

};
void blockHeapAllocatePrint(void* ptr)
{
	BlockHeader* bh = (BlockHeader*)ptr;

	printf("���� �ּ� : %llu \n", (ptraddr)bh);
	printf("�޸� �ڵ� : %llu \n", bh->handle);
	printf("�޸� ũ�� : %llu \n", bh->blockSize);
	printf("���� �ּ� : %llu \n", bh->prev);
	printf("���� �ּ� : %llu \n", bh->next);
	printf("\n\n");
}
//void blockStackAllocatePrint(string name, void* ptr, u64 size)
//{
//	u64* blockSize = (u64*)((u64)ptr - sizeof(u64));
//
//	printf("�޸� �� �Ҵ� ����(%s) \n", name.c_str());
//	printf("���� �ּ� : %llu \n", blockSize);
//	printf("�޸� ũ�� : %llu \n", *blockSize);
//	printf("���� ũ�� : %llu \n", size);
//	printf("\n\n");
//}
//void blockLinearAllocatePrint(string name, void* ptr, u64 size)
//{
//	u64 blockSize = JGMath::AlignUp(size, sizeof(ptraddr));
//
//	printf("�޸� �� �Ҵ� ����(%s) \n", name.c_str());
//	printf("���� �ּ� : %llu \n", ptr);
//	printf("�޸� ũ�� : %llu \n", blockSize);
//	printf("���� ũ�� : %llu \n", size);
//	printf("\n\n");
//}
//
//
//void StackAllocatorInfoPrint(JG::JGAllocatorManager& manager)
//{
//	printf("StackAllocator ���� \n");
//	printf("���� �ּ� : %llu \n", manager.GetStackAllocator().GetStartPtr());
//	printf("���� Top �ּ� : %llu \n", manager.GetStackAllocator().GetTopPtr());
//	printf("�� �޸� ũ�� : %llu \n", manager.GetStackAllocator().GetTotalMemory());
//	printf("��� �޸� ũ�� : %llu \n", manager.GetStackAllocator().GetUseMemory());
//	printf("\n\n");
//}
//void LinearAllocatorInfoPrint(JG::JGAllocatorManager& manager)
//{
//	printf("LinearAllocator ���� \n");
//	printf("���� �ּ� : %llu \n", manager.GetLinearAllocator().GetStartPtr());
//	printf("���� �ּ� : %llu \n", manager.GetLinearAllocator().GetCurrentPtr());
//	printf("�� �޸� ũ�� : %llu \n", manager.GetLinearAllocator().GetTotalMemory());
//	printf("��� �޸� ũ�� : %llu \n", manager.GetLinearAllocator().GetUseMemory());
//	printf("\n\n");
//}
//
//void HeapAllocatorInfoPrint()
//{
//
//	printf("HeapAllocator ���� \n");
//	void* start = (void*)JG::JGAllocatorManager::GetInstance()->mHeapAllocator.GetStartPtr();
//	BlockHeader* bh = (BlockHeader*)start;
//	while (true)
//	{
//
//		blockHeapAllocatePrint(bh);
//		if (bh->next == 0) break;
//		bh = (BlockHeader*)bh->next;
//		
//	}
//
//
//
//}

#include <chrono>



class TestObject : public JGObject
{
public:
	TestObject() {
	
	}
	virtual ~TestObject() {
	
	}
};

class TT
{
public:
	TT() {}
	~TT() {}
};


void blockDeAllocatePrint(string name, void* ptr)
{
	BlockHeader* bh = (BlockHeader*)((u64)ptr - sizeof(BlockHeader));
}

int main()
{
	// ������ �߰�
	JGAllocatorDesc desc;
	desc.StackAllocMem = _MB * 100;
	desc.LinearAllocMem = _MB * 100;
	desc.HeapAllocMem = _MB * 100;
	desc.SingleFrameAllocMem = 128;
	desc.DoubleBufferedAllocMem = 256;
	desc.MemoryDefragmenterCountPerFrame = 20;
	JG::JGAllocatorManager::Create(desc);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	std::unique_ptr<TestObject> testNew[10000];
	for (int i = 0; i < 10000; ++i)
	{
		testNew[i] = std::make_unique<TestObject>();
	}



	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;

	cout << "new 10000�� �޸� �Ҵ� : " << sec.count() << endl;

	start = std::chrono::system_clock::now();

	JGUniquePtr<TestObject> testNew2[10000];
	for (int i = 0; i < 10000; ++i)
	{
		testNew2[i] = CreateUniquePtr<TestObject>();
	}

	sec = std::chrono::system_clock::now() - start;

	cout << "custom 10000�� �޸� �Ҵ� : " << sec.count() << endl;
	
	JG::JGAllocatorManager::Destroy();
	_CrtDumpMemoryLeaks();
	return 0;
}


