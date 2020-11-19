#include <iostream>
#include <string>

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
	u64     blockSize;    // ��� ������(��ü)
	bool    used = false; // ��� �߳� ���߳�
	byte    padding[7];   //
};
//void blockHeapAllocatePrint(void* ptr)
//{
//	BlockHeader* bh = (BlockHeader*)ptr;
//
//	printf("���� �ּ� : %llu \n", bh);
//	printf("�Ҵ� ���� : %d \n", bh->used);
//	printf("�޸� ũ�� : %llu \n", bh->blockSize);
//	printf("���� �ּ� : %llu \n", bh->prev);
//	printf("���� �ּ� : %llu \n", bh->next);
//	printf("\n\n");
//}
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
//void HeapAllocatorInfoPrint(JG::JGAllocatorManager& manager)
//{
//
//	printf("HeapAllocator ���� \n");
//	void* start = manager.GetHeapAllocator().GetStartPtr();
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


void blockDeAllocatePrint(string name, void* ptr)
{
	BlockHeader* bh = (BlockHeader*)((u64)ptr - sizeof(BlockHeader));
}

int main()
{
	JGAllocatorDesc desc;
	desc.stackAllocMem = 256;
	desc.linearAllocMem = 256;
	desc.heapAllocMem = 1024;
	desc.singleFrameAllocMem = 128;
	desc.doubleBufferedAllocMem = 256;

	JG::JGAllocatorManager AllocManager(desc);


	//// stack �Ҵ�
	//{
	//	StackAllocatorInfoPrint(AllocManager);

	//	void* p1 = (test*)AllocManager.GetStackAllocator().Alloc(sizeof(test));
	//
	//	blockStackAllocatePrint("p1", p1, sizeof(test));

	//	void* p2 = (test*)AllocManager.GetStackAllocator().Alloc(sizeof(test) * 10);

	//	blockStackAllocatePrint("p2", p2, sizeof(test) * 10);

	//	void* p3 = (test*)AllocManager.GetStackAllocator().Alloc(sizeof(test) * 4);

	//	blockStackAllocatePrint("p3", p3, sizeof(test) * 4);



	//	StackAllocatorInfoPrint(AllocManager);


	//	AllocManager.GetStackAllocator().DeAlloc(&p3);

	//	StackAllocatorInfoPrint(AllocManager);

	//	AllocManager.GetStackAllocator().DeAlloc(&p2);

	//	StackAllocatorInfoPrint(AllocManager);

	//	AllocManager.GetStackAllocator().DeAlloc(&p1);

	//	StackAllocatorInfoPrint(AllocManager);

	//}

	//// Linear �Ҵ�
	//{

	//	LinearAllocatorInfoPrint(AllocManager);

	//	void* p1 = (test*)AllocManager.GetLinearAllocator().Alloc(sizeof(test));

	//	blockLinearAllocatePrint("p1", p1, sizeof(test));
	//	LinearAllocatorInfoPrint(AllocManager);



	//	void* p2 = (test*)AllocManager.GetLinearAllocator().Alloc(sizeof(test) * 10);
	//	blockLinearAllocatePrint("p2", p2, sizeof(test) * 10);
	//	LinearAllocatorInfoPrint(AllocManager);



	//	void* p3 = (test*)AllocManager.GetLinearAllocator().Alloc(sizeof(test) * 4);

	//	blockLinearAllocatePrint("p3", p3, sizeof(test) * 4);
	//	LinearAllocatorInfoPrint(AllocManager);
	//}

	//// Heap �Ҵ�
	//{
	//	void* p1 = (test*)AllocManager.GetHeapAllocator().Alloc(sizeof(test));
	//	void* p2 = (test*)AllocManager.GetHeapAllocator().Alloc(sizeof(test) * 4);
	//	void* p3 = (test*)AllocManager.GetHeapAllocator().Alloc(sizeof(test) * 10);
	//	void* p4 = (test*)AllocManager.GetHeapAllocator().Alloc(sizeof(test) * 3);

	//	HeapAllocatorInfoPrint(AllocManager);

	//	cout << "�Ҵ� ����" << endl << endl;
	//	AllocManager.GetHeapAllocator().DeAlloc(&p2);
	//	AllocManager.GetHeapAllocator().DeAlloc(&p4);
	//	AllocManager.GetHeapAllocator().DeAlloc(&p1);
	//	HeapAllocatorInfoPrint(AllocManager);



	//	cout << "�ٽ� �Ҵ�" << endl << endl;
	//	p2 = (test*)AllocManager.GetHeapAllocator().Alloc(sizeof(test) * 30);
	//	p4 = (test*)AllocManager.GetHeapAllocator().Alloc(sizeof(test) * 3);
	//	void* p5 = (test*)AllocManager.GetHeapAllocator().Alloc(sizeof(test) * 17);
	//	void* p6 = (test*)AllocManager.GetHeapAllocator().Alloc(sizeof(test) * 3);
	//	HeapAllocatorInfoPrint(AllocManager);


	//	cout << "�Ҵ� ����" << endl << endl;
	//	AllocManager.GetHeapAllocator().DeAlloc(&p2);
	//	AllocManager.GetHeapAllocator().DeAlloc(&p5);
	//	
	//	HeapAllocatorInfoPrint(AllocManager);

	//}

	//// SingleAllocator �Ҵ�
	//{
	//	test* p1 = (test*)AllocManager.GetSingleFrameAllocator().Alloc(sizeof(test));
	//	test* p2 = (test*)AllocManager.GetSingleFrameAllocator().Alloc(sizeof(test) * 3);
	//	p1->a = 3;
	//	p1->b = 6;
	//	AllocManager.GetSingleFrameAllocator().Clear();

	//	test* p3 = (test*)AllocManager.GetSingleFrameAllocator().Alloc(sizeof(test));
	//	p3->a = 1;
	//	p3->b = 2;
	//	p1 = nullptr;
	//	p2 = nullptr;

	//}
	//{
	//	test* p1 = (test*)AllocManager.GetDoubleFrameAllocator().Alloc(sizeof(test));
	//	test* p2 = (test*)AllocManager.GetDoubleFrameAllocator().Alloc(sizeof(test) * 3);
	//	p1->a = 2;
	//	
	//	AllocManager.GetDoubleFrameAllocator().Swap();

	//	test* p3 = (test*)AllocManager.GetDoubleFrameAllocator().Alloc(sizeof(test));
	//	p3->a = 2;
	//	test* p4 = (test*)AllocManager.GetDoubleFrameAllocator().Alloc(sizeof(test) * 3);

	//	AllocManager.GetDoubleFrameAllocator().Swap();

	//	test* p5 = (test*)AllocManager.GetDoubleFrameAllocator().Alloc(sizeof(test));
	//	p5->a = 0;
	//	test* p6 = (test*)AllocManager.GetDoubleFrameAllocator().Alloc(sizeof(test) * 3);
	//}




	return 0;
}


