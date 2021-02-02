#pragma once

#include "JGCore.h"
#include "DirectX12Helper.h"

namespace JG
{
	class CommandList
	{
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE d3dType) {}
	public:
		void Reset() {}
		bool Close() {}
		bool Close(CommandList* commandList) { return false; }


	};
	
}

