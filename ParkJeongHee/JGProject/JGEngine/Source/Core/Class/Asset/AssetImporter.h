#pragma once
#include "Common/Define.h"

#include "Math/JVector.h"
#include "Math/JMatrix.h"
#include "Asset.h"
#include "FileIO.h"

struct aiMesh;
namespace JG
{
	enum class EAssetImportResult
	{
		Success,
		
	};

	enum class EAssetImportFlags
	{
		None,
	};


	struct AssetImportSettings
	{
		String AssetPath;
		String OutputPath;
		EAssetImportFlags Flags = EAssetImportFlags::None;
	};


	class AssetImporter
	{
	public:
		static EAssetImportResult Import(const AssetImportSettings& setting);

	private:
		static void ReadMesh(aiMesh* mesh, StaticMeshAssetStock* output);
	private:
		static void WriteMesh(const String& outputPath, StaticMeshAssetStock& info);
	};
}