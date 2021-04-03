#pragma once
#include "Common/Define.h"


namespace JG
{
	enum class EAssetImportResult
	{
		Success,
		
	};
	class AssetImporter
	{
		EAssetImportResult Import(const String& path);
	};
}