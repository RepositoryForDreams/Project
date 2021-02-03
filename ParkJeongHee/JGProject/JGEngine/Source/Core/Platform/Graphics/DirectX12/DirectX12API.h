#pragma once
#include "Graphics/GraphicsAPI.h"




struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12Resource;
namespace JG
{
	// Rendering Flow
	//
	// �� ó��
	// CommandList ���� �� Reset
	// CommandList SetViewport, ���� Rect ����
	// ClearRenderTarget
	// SetRenderTarget
	// SetDescriptionHeaps (�ؽ��� �Ҵ��� DescriptionHeap)
	//
	// 
	// �߰�
	// ������ ���ε�
	// Vertex, Index Buffer ���ε�
	// DrawCall
	//
	//
	// ��
	// Close
	// CommandQueue �� ������

	
	class DirectX12API : public IGraphicsAPI
	{
	private:

	public:
		virtual bool Create() override;
		virtual void Destroy() override;
		virtual EGraphicsAPI GetAPI()const override;

	public:
		static IDXGIFactory4* GetDXGIFactory();
		static ID3D12Device*  GetD3DDevice();
		static u64			  GetFrameBufferIndex();
	protected:
		virtual void Begin() override;
		virtual void End()   override;
		virtual void Flush() override;
	};
}
