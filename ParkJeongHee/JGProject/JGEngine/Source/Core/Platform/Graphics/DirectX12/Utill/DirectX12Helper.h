#pragma once
#include "JGCore.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include "d3dx12.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"Dxgi.lib")
#pragma comment(lib,"dxguid.lib")

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif
#if defined(far)
#undef far
#endif

#if defined(near)
#undef near
#endif


using namespace Microsoft::WRL;



namespace JG
{
	ComPtr<IDXGIFactory4>   CreateDXGIFactory();
	
	ComPtr<ID3D12Device>    CreateD3DDevice(ComPtr<IDXGIFactory4> factory, bool is_UseWrapDevice, DXGI_ADAPTER_DESC1* OutadapterDesc);
	
	ComPtr<IDXGISwapChain4> CreateDXGISwapChain(
		HWND hWnd, ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> cmdQue,
		DXGI_FORMAT format, u32 width, u32 height, u32 bufferCount);

	
	// D3D Command
	ComPtr<ID3D12CommandQueue>        CreateD3DCommandQueue(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12GraphicsCommandList> CreateD3DCommandList(ComPtr<ID3D12Device> device, ComPtr<ID3D12CommandAllocator> cmdAllocator, D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12CommandAllocator>	  CreateD3DCommandAllocator(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);


	// D3D Coms
	ComPtr<ID3D12DescriptorHeap> CreateD3DDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag, u32 numDescriptor);
	ComPtr<ID3D12RootSignature>  CreateD3DRootSignature(ComPtr<ID3D12Device> device,  CD3DX12_ROOT_SIGNATURE_DESC* DESC);


#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif
	inline u64 HashRange(const u32* const Begin, const u32* const End, u64 Hash)
	{
#if ENABLE_SSE_CRC32
		const u64* Iter64 = (const u64*)(((u64)Begin + 7) & ~7);
		const u64* const End64 = (const u64* const)((u64)End & ~7);

		// If not 64-bit aligned, start with a single u32
		if ((u32*)Iter64 > Begin)
			Hash = _mm_crc32_u32((u32)Hash, *Begin);

		// Iterate over consecutive u64 values
		while (Iter64 < End64)
			Hash = _mm_crc32_u64((u64)Hash, *Iter64++);

		// If there is a 32-bit remainder, accumulate that
		if ((u32*)Iter64 < End)
			Hash = _mm_crc32_u32((u32)Hash, *(u32*)Iter64);
#else
		// An inexpensive hash for CPUs lacking SSE4.2
		for (const u32* Iter = Begin; Iter < End; ++Iter)
			Hash = 16777619U * Hash ^ *Iter;
#endif

		return Hash;
	}

	template <typename T>
	inline u64 HashState(const T* StateDesc, u64 Count = 1, u64 Hash = 2166136261U)
	{
		static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
		return HashRange((u32*)StateDesc, (u32*)(StateDesc + Count), Hash);
	}
}