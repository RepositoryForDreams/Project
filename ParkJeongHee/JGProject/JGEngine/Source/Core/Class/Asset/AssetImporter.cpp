#include "pch.h"
#include "AssetImporter.h"
#include "Class/FileIO.h"
#include "Common/String.h"
#include "Application.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"




namespace JG
{
	EAssetImportResult AssetImporter::Import(const AssetImportSettings& setting)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(ws2s(setting.AssetPath),
			aiProcess_JoinIdenticalVertices |     // 동일한 꼭지점 결합, 인덱싱 최적화
			aiProcess_ValidateDataStructure |     // 로더의 출력을 검증
			aiProcess_ImproveCacheLocality |      // 출력 정점의 캐쉬위치를 개선
			aiProcess_RemoveRedundantMaterials |  // 중복된 매터리얼 제거
			aiProcess_GenUVCoords |               // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
			aiProcess_TransformUVCoords |         // UV 변환 처리기 (스케일링, 변환...)
			aiProcess_FindInstances |             // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
			aiProcess_LimitBoneWeights |          // 정점당 뼈의 가중치를 최대 4개로 제한
			aiProcess_OptimizeMeshes |            // 가능한 경우 작은 매쉬를 조인
			aiProcess_GenSmoothNormals |          // 부드러운 노말벡터(법선벡터) 생성
			aiProcess_SplitLargeMeshes |          // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
			aiProcess_Triangulate |               // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
			aiProcess_ConvertToLeftHanded |       // D3D의 왼손좌표계로 변환
			aiProcess_SortByPType |               // 단일타입의  프리미티브로 구성된 '깨끗한' 매쉬를 만듬
			aiProcess_CalcTangentSpace            // 탄젠트 공간 계산 )
		);


		if (scene != nullptr)
		{
			StaticMeshAssetStock meshInfo;
			meshInfo.Name = s2ws(scene->mName.C_Str());
			if (scene->HasMeshes() == true)
			{
				u32 meshCount = scene->mNumMeshes;
				for (u32 i = 0; i < meshCount; ++i)
				{
					auto mesh = scene->mMeshes[i];
					ReadMesh(mesh, &meshInfo);
				}
				WriteMesh(setting.OutputPath, meshInfo);
			}
			if (scene->HasAnimations() == true)
			{

			}
			if (scene->HasMaterials() == true)
			{

			}
			if (scene->HasTextures() == true)
			{

			}
			
		}





		return EAssetImportResult::Success;
	}
	void AssetImporter::ReadMesh(aiMesh* mesh, StaticMeshAssetStock* output)
	{
		if (output == nullptr || mesh == nullptr)
		{
			return;
		}
		List<JGVertex> vertices;
		List<u32>&   indices = output->Indices;
		output->SubMeshNames.push_back(s2ws(mesh->mName.C_Str()));

		vertices.resize(mesh->mNumVertices);
		
		for (u32 i = 0; i < mesh->mNumVertices; ++i)
		{
			JGVertex v;
			if (mesh->HasPositions() == true)
			{
				auto& ai_pos = mesh->mVertices[i];
				v.Position.x = ai_pos.x;
				v.Position.y = ai_pos.y;
				v.Position.z = ai_pos.z;
			}

			if (mesh->HasNormals() == true)
			{
				auto& ai_nor = mesh->mNormals[i];

				v.Normal.x = ai_nor.x;
				v.Normal.y = ai_nor.y;
				v.Normal.z = ai_nor.z;
			}


			if (mesh->HasTangentsAndBitangents() == true)
			{
				auto& ai_tan = mesh->mTangents[i];
				v.Tangent.x = ai_tan.x;
				v.Tangent.y = ai_tan.y;
				v.Tangent.z = ai_tan.z;

				auto& ai_bit = mesh->mBitangents[i];
				v.Bitangent.x = ai_bit.x;
				v.Bitangent.y = ai_bit.y;
				v.Bitangent.z = ai_bit.z;
			}


			vertices[i] = v;
		}

		output->Vertices.push_back(vertices);


		if (mesh->HasFaces() == true)
		{
			for (u32 i = 0; i < mesh->mNumFaces; ++i)
			{
				auto& face = mesh->mFaces[i];

				for (u32 j = 0; j < face.mNumIndices; ++j)
				{
					indices.push_back(face.mIndices[j]);
				}
			}
		}

		if (mesh->HasBones() == true)
		{

		}



	}
	void AssetImporter::WriteMesh(const String& outputPath, StaticMeshAssetStock& info)
	{
		if (info.Name.empty() == true)
		{
			info.Name = info.SubMeshNames[0];
		}
		auto filePath = CombinePath(outputPath, info.Name) + ASSET_MESH_FORMAT;
		FileStreamWriter writer;

		if (writer.Open(filePath) == true)
		{
			writer.Write(info);
			writer.Close();
		}
	}

}