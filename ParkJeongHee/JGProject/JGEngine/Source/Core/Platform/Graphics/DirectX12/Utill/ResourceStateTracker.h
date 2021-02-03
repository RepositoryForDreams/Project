#pragma once
#include "JGCore.h"
#include "DirectX12Helper.h"

namespace JG
{
	class ResourceState
	{
	public:
		std::map<uint32_t, D3D12_RESOURCE_STATES> StateMap;
		D3D12_RESOURCE_STATES State;
	public:
		D3D12_RESOURCE_STATES Get(uint32_t subresource) const {
			if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) return State;
			else
			{
				auto iter = StateMap.find(subresource);
				if (iter == StateMap.end())
				{
					return StateMap.at(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
				}
				return iter->second;
			}

		}
		void Set(uint32_t subresource, D3D12_RESOURCE_STATES state)
		{
			if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
			{
				State = state;
				StateMap.clear();
			}
			else
			{
				StateMap[subresource] = state;

			}

		}
	};

	class ResourceInfo
	{
	public:
		ResourceState State;
		String Name;
		u64 RefCount = 0;
	public:
		ResourceInfo() = default;
		ResourceInfo(const String name, u64 refCount) : Name(name), RefCount(refCount) {}
	};

	class ResourceStateTracker
	{
	private:
		std::vector<D3D12_RESOURCE_BARRIER> mResourceBarriers;
		std::vector<D3D12_RESOURCE_BARRIER> mPendingResourceBarriers;
		std::unordered_map<ID3D12Resource*, ResourceState> mResourceStates;
	public:
		void TransitionBarrier(ComPtr<ID3D12Resource> d3dResource, D3D12_RESOURCE_STATES state, uint32_t subResource);

		void UAVBarrier(ComPtr<ID3D12Resource> d3dResource);
		void AliasBarrier(ComPtr<ID3D12Resource> beforeD3DResource, ComPtr<ID3D12Resource> afterD3DResource);

		void FlushResourceBarrier(ID3D12GraphicsCommandList* cmdList);
		bool FlushPendingResourceBarrier(ID3D12GraphicsCommandList* cmdList);
		void CommitResourceState();
		void Reset();

	public:
		static void Lock();
		static void UnLock();
		static void ForEach(const std::function<void(const ID3D12Resource*, const ResourceInfo&)>& action);
		static void RegisterResource(const String& name, ID3D12Resource* d3dResource);
		static void SetResourceName(ID3D12Resource* d3dResource, const String& name);
		static bool GetResourceName(ID3D12Resource* d3dResource, String* out_name);
		static void UnRegisterResource(ID3D12Resource* d3dResource);
	};
}