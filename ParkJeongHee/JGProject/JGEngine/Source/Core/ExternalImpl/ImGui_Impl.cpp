#include "pch.h"
#include "ImGui_Impl.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx12.h"
#include "Imgui/imgui_impl_win32.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
// NOTE
// DIRECTX 12 INCLUDE
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DXGI.lib")


JG::EWindowPlatform g_Platform;
JG::EGraphicsAPI    g_Api;
JG::ptraddr         g_Handle;

namespace DirectX12
{
    struct FrameContext
    {
        ID3D12CommandAllocator* CommandAllocator;
        UINT64                  FenceValue;
    };

    // Data
    static int const                    NUM_FRAMES_IN_FLIGHT = 3;
    static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
    static UINT                         g_frameIndex = 0;
    static int const                    NUM_BACK_BUFFERS = 3;
    static ID3D12Device* g_pd3dDevice = NULL;
    static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
    static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
    static ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
    static ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
    static ID3D12Fence* g_fence = NULL;
    static HANDLE                       g_fenceEvent = NULL;
    static UINT64                       g_fenceLastSignaledValue = 0;
    static IDXGISwapChain3* g_pSwapChain = NULL;
    static HANDLE                       g_hSwapChainWaitableObject = NULL;
    static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
    static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    void WaitForLastSubmittedFrame();
    FrameContext* WaitForNextFrameResources();
    void ResizeSwapChain(HWND hWnd, int width, int height);
    // Forward declarations of helper functions

    bool CreateDeviceD3D(HWND hWnd)
    {
        // Setup swap chain
        DXGI_SWAP_CHAIN_DESC1 sd;
        {
            ZeroMemory(&sd, sizeof(sd));
            sd.BufferCount = NUM_BACK_BUFFERS;
            sd.Width = 0;
            sd.Height = 0;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            sd.Scaling = DXGI_SCALING_STRETCH;
            sd.Stereo = FALSE;
        }


        g_pd3dDevice = JG::DirectX12API::GetD3DDevice();
    	



        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = NUM_BACK_BUFFERS;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 1;
            if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
                return false;

            SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
            for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
            {
                g_mainRenderTargetDescriptor[i] = rtvHandle;
                rtvHandle.ptr += rtvDescriptorSize;
            }
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
                return false;
        }

        {
            D3D12_COMMAND_QUEUE_DESC desc = {};
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            desc.NodeMask = 1;
            if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
                return false;
        }

        for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
            if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
                return false;

        if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
            g_pd3dCommandList->Close() != S_OK)
            return false;

        if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
            return false;

        g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (g_fenceEvent == NULL)
            return false;

        {
            IDXGIFactory4* dxgiFactory = NULL;
            IDXGISwapChain1* swapChain1 = NULL;
            if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK ||
                dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK ||
                swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
                return false;
            swapChain1->Release();
            dxgiFactory->Release();
            g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
            g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
        }

        CreateRenderTarget();
        return true;
    }

    void CleanupDeviceD3D()
    {
        CleanupRenderTarget();
        if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
        if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
        for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
            if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = NULL; }
        if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
        if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
        if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
        if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
        if (g_fence) { g_fence->Release(); g_fence = NULL; }
        if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = NULL; }
       // if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    }

    void CreateRenderTarget()
    {
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            ID3D12Resource* pBackBuffer = NULL;
            g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
            g_mainRenderTargetResource[i] = pBackBuffer;
        }
    }

    void CleanupRenderTarget()
    {
        WaitForLastSubmittedFrame();

        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
            if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
    }

    void WaitForLastSubmittedFrame()
    {
        FrameContext* frameCtxt = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

        UINT64 fenceValue = frameCtxt->FenceValue;
        if (fenceValue == 0)
            return; // No fence was signaled

        frameCtxt->FenceValue = 0;
        if (g_fence->GetCompletedValue() >= fenceValue)
            return;

        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        WaitForSingleObject(g_fenceEvent, INFINITE);
    }

    FrameContext* WaitForNextFrameResources()
    {
        UINT nextFrameIndex = g_frameIndex + 1;
        g_frameIndex = nextFrameIndex;

        HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, NULL };
        DWORD numWaitableObjects = 1;

        FrameContext* frameCtxt = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
        UINT64 fenceValue = frameCtxt->FenceValue;
        if (fenceValue != 0) // means no fence was signaled
        {
            frameCtxt->FenceValue = 0;
            g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
            waitableObjects[1] = g_fenceEvent;
            numWaitableObjects = 2;
        }

        WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

        return frameCtxt;
    }

    void ResizeSwapChain(HWND hWnd, int width, int height)
    {
        DXGI_SWAP_CHAIN_DESC1 sd;
        g_pSwapChain->GetDesc1(&sd);
        sd.Width = width;
        sd.Height = height;

        IDXGIFactory4* dxgiFactory = NULL;
        g_pSwapChain->GetParent(IID_PPV_ARGS(&dxgiFactory));

        g_pSwapChain->Release();
        CloseHandle(g_hSwapChainWaitableObject);

        IDXGISwapChain1* swapChain1 = NULL;
        dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1);
        swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain));
        swapChain1->Release();
        dxgiFactory->Release();

        g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);

        g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
        assert(g_hSwapChainWaitableObject != NULL);
    }
}

namespace ImGui
{
	void ExternalImpl_Init(JG::EWindowPlatform platform, JG::EGraphicsAPI API, JG::ptraddr handle)
	{
        g_Platform = platform;
		g_Api = API;
        g_Handle = handle;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

		
        switch(platform)
        {
        case JG::EWindowPlatform::Windows:
            ImGui_ImplWin32_Init((void*)g_Handle);
            break;

        default:
            JGASSERT("not supported Window Platform");
        }
		
		
		switch(API)
		{
        case JG::EGraphicsAPI::DirectX12:
	        {
				DirectX12::CreateDeviceD3D((HWND)g_Handle);
                ImGui_ImplDX12_Init(DirectX12::g_pd3dDevice, DirectX12::NUM_FRAMES_IN_FLIGHT,
                                    DXGI_FORMAT_R8G8B8A8_UNORM, DirectX12::g_pd3dSrvDescHeap,
                                    DirectX12::g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                                    DirectX12::g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
	        }

            break;
        default:
            JGASSERT("not supported Graphics API");
		}
	}
    void ExternalImpl_Resize(JG::u32 width, JG::u32 height)
	{
        switch (g_Api)
        {
        case JG::EGraphicsAPI::DirectX12:
            if (DirectX12::g_pd3dDevice != NULL)
            {
                DirectX12::WaitForLastSubmittedFrame();
                ImGui_ImplDX12_InvalidateDeviceObjects();
                DirectX12::CleanupRenderTarget();
                DirectX12::ResizeSwapChain((HWND)g_Handle, (UINT)width, (UINT)height);
                DirectX12::CreateRenderTarget();
                ImGui_ImplDX12_CreateDeviceObjects();
            }
            break;
        default:
            JGASSERT("not supported Graphics API");
        }
	}
	void ExternalImpl_BeginFrame()
	{
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
	}

	void ExternalImpl_EndFrame(ImVec4 clearColor)
	{

		switch(g_Api)
		{
        case JG::EGraphicsAPI::DirectX12:
	        {
                DirectX12::FrameContext* frameCtxt = DirectX12::WaitForNextFrameResources();
                UINT backBufferIdx = DirectX12::g_pSwapChain->GetCurrentBackBufferIndex();
                frameCtxt->CommandAllocator->Reset();

                D3D12_RESOURCE_BARRIER barrier = {};
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource = DirectX12::g_mainRenderTargetResource[backBufferIdx];
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

                DirectX12::g_pd3dCommandList->Reset(frameCtxt->CommandAllocator, NULL);
                DirectX12::g_pd3dCommandList->ResourceBarrier(1, &barrier);
                DirectX12::g_pd3dCommandList->ClearRenderTargetView(DirectX12::g_mainRenderTargetDescriptor[backBufferIdx], (float*)&clearColor, 0, NULL);
                DirectX12::g_pd3dCommandList->OMSetRenderTargets(1, &DirectX12::g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
                DirectX12::g_pd3dCommandList->SetDescriptorHeaps(1, &DirectX12::g_pd3dSrvDescHeap);
                ImGui::Render();
                ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectX12::g_pd3dCommandList);
                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
                DirectX12::g_pd3dCommandList->ResourceBarrier(1, &barrier);
                DirectX12::g_pd3dCommandList->Close();

                DirectX12::g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&DirectX12::g_pd3dCommandList);

                DirectX12::g_pSwapChain->Present(1, 0); // Present with vsync
                //g_pSwapChain->Present(0, 0); // Present without vsync

                UINT64 fenceValue = DirectX12::g_fenceLastSignaledValue + 1;
                DirectX12::g_pd3dCommandQueue->Signal(DirectX12::g_fence, fenceValue);
                DirectX12::g_fenceLastSignaledValue = fenceValue;
                frameCtxt->FenceValue = fenceValue;
	        }
            break;

        default:
            JGASSERT("not supported Graphics API");
		}
        
	}
	
	void ExternalImpl_Destroy()
	{
        switch (g_Api)
        {
        case JG::EGraphicsAPI::DirectX12:
        {
            DirectX12::WaitForLastSubmittedFrame();
            ImGui_ImplDX12_Shutdown();
            DirectX12::CleanupDeviceD3D();
        }

        break;
        default:
            JGASSERT("not supported Graphics API");
        }
		
        switch (g_Platform)
        {
        case JG::EWindowPlatform::Windows:
            ImGui_ImplWin32_Shutdown();
            break;

        default:
            JGASSERT("not supported Window Platform");
        }
        ImGui::DestroyContext();
	}
}