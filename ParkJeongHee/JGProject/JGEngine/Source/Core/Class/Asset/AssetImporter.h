#pragma once
#include "Common/Define.h"

#include "Math/JVector.h"
#include "Math/JMatrix.h"
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
	private:
		struct Vertex
		{
			JVector3 Position;
			JVector3 Normal;
			JVector3 Tangent;
			JVector3 Bitangent;
			JVector2 Texcoord;
		};
		struct Bone
		{
			
		};


		class MeshInfo : public ISerializable
		{
		public:
			String Name;
			List<String>       SubMeshNames;
			List<List<Vertex>> Vertices;
			List<u32>		   Indices;

			bool IsSkinned = false;

		protected:
			virtual void Serialize(FileStreamWriter* writer) const override {
				writer->Write(Name);
				writer->Write(SubMeshNames);
				writer->Write(Vertices);
				writer->Write(Indices);
				writer->Write(IsSkinned);
			}
			virtual void DeSerialize(FileStreamReader* reader) const override {
				reader->Read(&Name);
				reader->Read(&SubMeshNames);
				reader->Read(&Vertices);
				reader->Read(&Indices);
				reader->Read(&IsSkinned);
			}
		};

		//struct 


	public:
		static EAssetImportResult Import(const AssetImportSettings& setting);

	private:
		static void ReadMesh(aiMesh* mesh, MeshInfo* output);
	private:
		static void WriteMesh(const String& outputPath, const MeshInfo& info);
	};
}