#include "pch.h"
#include "AssetImporter.h"
#include "Common/String.h"
#include "Application.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace JG
{
	EAssetImportResult AssetImporter::Import(const String& path)
	{
		auto assetPath = CombinePath(Application::GetAssetPath(), path);

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(ws2s(assetPath),
			0);


		if (scene == nullptr)
		{
			// TODO
			// Error Log
			// Result return
		}



		return EAssetImportResult::Success;
	}
}