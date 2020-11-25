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
void HeapAllocatorInfoPrint()
{

	printf("HeapAllocator ���� \n");
	void* start = (void*)JG::JGAllocatorManager::GetInstance()->mHeapAllocator.GetStartPtr();
	BlockHeader* bh = (BlockHeader*)start;
	while (true)
	{

		blockHeapAllocatePrint(bh);
		if (bh->next == 0) break;
		bh = (BlockHeader*)bh->next;
		
	}



}

struct TestData
{
	int num = 0;
	float fnum = 0.0f;
	u64 unsignNum = 0;
};

struct TestData2
{
	int numArray[20] = { 0, };
};
void blockDeAllocatePrint(string name, void* ptr)
{
	BlockHeader* bh = (BlockHeader*)((u64)ptr - sizeof(BlockHeader));
}

int main()
{
	// ������ �߰�
	JGAllocatorDesc desc;
	desc.StackAllocMem = 256;
	desc.LinearAllocMem = 256;
	desc.HeapAllocMem = 1024;
	desc.SingleFrameAllocMem = 128;
	desc.DoubleBufferedAllocMem = 256;
	desc.MemoryDefragmenterCountPerFrame = 20;
	JG::JGAllocatorManager::Create(desc);
	// Heap
	{
		auto m1 = JGAllocatorManager::SingleFrameAlloc(32);
		auto m2 = JGAllocatorManager::SingleFrameAlloc(32);
		auto m3 = JGAllocatorManager::SingleFrameAlloc(32);
		auto m4 = JGAllocatorManager::SingleFrameAlloc(32);

		auto m13 = JGAllocatorManager::DoubleFrameAlloc(32);
		auto m23 = JGAllocatorManager::DoubleFrameAlloc(32);
		auto m33 = JGAllocatorManager::DoubleFrameAlloc(32);
		auto m43 = JGAllocatorManager::DoubleFrameAlloc(32);

		JGAllocatorManager::Update();

		auto m12 = JGAllocatorManager::DoubleFrameAlloc(32);
		auto m22 = JGAllocatorManager::DoubleFrameAlloc(32);
		auto m32 = JGAllocatorManager::DoubleFrameAlloc(32);
		auto m42 = JGAllocatorManager::DoubleFrameAlloc(32);

		JGAllocatorManager::Update();
	}


	JG::JGAllocatorManager::Destroy();
	_CrtDumpMemoryLeaks();
	return 0;
}


