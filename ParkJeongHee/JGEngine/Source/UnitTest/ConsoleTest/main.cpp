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
	ptraddr prev;         // 전 블록의 주솟값
	ptraddr next;         // 다음 블록의 주솟값
	ptraddr handle;
	u64     blockSize;    // 블록 사이즈(전체)

};
void blockHeapAllocatePrint(void* ptr)
{
	BlockHeader* bh = (BlockHeader*)ptr;

	printf("시작 주소 : %llu \n", (ptraddr)bh);
	printf("메모리 핸들 : %llu \n", bh->handle);
	printf("메모리 크기 : %llu \n", bh->blockSize);
	printf("이전 주소 : %llu \n", bh->prev);
	printf("다음 주소 : %llu \n", bh->next);
	printf("\n\n");
}
//void blockStackAllocatePrint(string name, void* ptr, u64 size)
//{
//	u64* blockSize = (u64*)((u64)ptr - sizeof(u64));
//
//	printf("메모리 블럭 할당 정보(%s) \n", name.c_str());
//	printf("시작 주소 : %llu \n", blockSize);
//	printf("메모리 크기 : %llu \n", *blockSize);
//	printf("원래 크기 : %llu \n", size);
//	printf("\n\n");
//}
//void blockLinearAllocatePrint(string name, void* ptr, u64 size)
//{
//	u64 blockSize = JGMath::AlignUp(size, sizeof(ptraddr));
//
//	printf("메모리 블럭 할당 정보(%s) \n", name.c_str());
//	printf("시작 주소 : %llu \n", ptr);
//	printf("메모리 크기 : %llu \n", blockSize);
//	printf("원래 크기 : %llu \n", size);
//	printf("\n\n");
//}
//
//
//void StackAllocatorInfoPrint(JG::JGAllocatorManager& manager)
//{
//	printf("StackAllocator 정보 \n");
//	printf("시작 주소 : %llu \n", manager.GetStackAllocator().GetStartPtr());
//	printf("현재 Top 주소 : %llu \n", manager.GetStackAllocator().GetTopPtr());
//	printf("총 메모리 크기 : %llu \n", manager.GetStackAllocator().GetTotalMemory());
//	printf("사용 메모리 크기 : %llu \n", manager.GetStackAllocator().GetUseMemory());
//	printf("\n\n");
//}
//void LinearAllocatorInfoPrint(JG::JGAllocatorManager& manager)
//{
//	printf("LinearAllocator 정보 \n");
//	printf("시작 주소 : %llu \n", manager.GetLinearAllocator().GetStartPtr());
//	printf("현재 주소 : %llu \n", manager.GetLinearAllocator().GetCurrentPtr());
//	printf("총 메모리 크기 : %llu \n", manager.GetLinearAllocator().GetTotalMemory());
//	printf("사용 메모리 크기 : %llu \n", manager.GetLinearAllocator().GetUseMemory());
//	printf("\n\n");
//}
//
void HeapAllocatorInfoPrint()
{

	printf("HeapAllocator 정보 \n");
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
	// 문제점 발견
	JGAllocatorDesc desc;
	desc.StackAllocMem = 256;
	desc.LinearAllocMem = 256;
	desc.HeapAllocMem = 1024;
	desc.SingleFrameAllocMem = 128;
	desc.DoubleBufferedAllocMem = 256;
	desc.MemoryDefragmenterCountPerFrame = 3;
	JG::JGAllocatorManager::Create(desc);
	// Heap
	{
		auto m1 = JGAllocatorManager::HeapAlloc(sizeof(TestData));
		auto m2 = JGAllocatorManager::HeapAlloc(sizeof(TestData2));
		auto m3 = JGAllocatorManager::HeapAlloc(sizeof(TestData));

		auto t1 = (TestData*)m1.Get();
		auto t2 = (TestData2*)m2.Get();
		auto t3 = (TestData*)m3.Get();

		t1->fnum = 50.0f;
		t1->unsignNum = 2;

		t2->numArray[1] = 4;

		t3->num = 15;
		

		JGAllocatorManager::DeAlloc(std::move(m2));
		HeapAllocatorInfoPrint();

		cout << endl; 		cout << endl;
		cout << "Memory Defrag" << endl;
		cout << endl;
		JGAllocatorManager::Update();
		auto newt3 = (TestData*)m3.Get();
		HeapAllocatorInfoPrint();



		auto mm1 = JGAllocatorManager::HeapAlloc(sizeof(TestData2));
		auto mm2 = JGAllocatorManager::HeapAlloc(sizeof(TestData2));
		auto mm3 = JGAllocatorManager::HeapAlloc(sizeof(TestData2));
		cout << endl; 		cout << endl;
		HeapAllocatorInfoPrint();

		JGAllocatorManager::DeAlloc(std::move(mm1));
		JGAllocatorManager::DeAlloc(std::move(mm2));
		JGAllocatorManager::DeAlloc(std::move(m1));
		cout << endl; 		cout << endl;
		HeapAllocatorInfoPrint();


		cout << endl; 		cout << endl;
		cout << "Memory Defrag" << endl;
		cout << endl;
		JGAllocatorManager::Update();
		HeapAllocatorInfoPrint();
	}


	JG::JGAllocatorManager::Destroy();
	_CrtDumpMemoryLeaks();
	return 0;
}


