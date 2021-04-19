#pragma once
#include "Common/Define.h"
namespace JG
{
	class AssetManager
	{
	private:
		// Texture
		// Mesh
		// Material
	public:
		bool AsyncLoadAsset(const String& path);
		void Wait();
		void Reset();
	public:
		
	};
}