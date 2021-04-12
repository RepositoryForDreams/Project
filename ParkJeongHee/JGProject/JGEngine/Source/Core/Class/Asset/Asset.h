#pragma once
#include "Common/Abstract.h"

#include "Graphics/Resource.h"
#include "Class/Global/Scheduler.h"
#include "Class/FileIO.h"
namespace JG
{
#define ASSET_MESH_FORMAT TT(".mesh")

	enum class EAssetType
	{

	};

	namespace Asset
	{
		struct Vertex
		{
			JVector3 Position;
			JVector3 Normal;
			JVector3 Tangent;
			JVector3 Bitangent;
			JVector2 Texcoord;

			static SharedPtr<InputLayout> GetInputLayout() {
				auto inputLayout = InputLayout::Create();
				inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
				inputLayout->Add(EShaderDataType::_float3, "NORMAL", 0);
				inputLayout->Add(EShaderDataType::_float3, "TANGENT", 0);
				inputLayout->Add(EShaderDataType::_float3, "BITANGENT", 0);
				inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
				return inputLayout;
			}
		};

		struct Bone
		{

		};
	}

	class IAssetStock
	{
	public:
		virtual ~IAssetStock() = default;
	};


	class StaticMeshAssetStock : public IAssetStock, public ISerializable
	{
	public:
		String Name;
		bool   IsSkinned = false;
		List<String>                SubMeshNames;
		List<List<Asset::Vertex>>   Vertices;
		List<u32>		            Indices;
	public:
		virtual ~StaticMeshAssetStock() = default;
	protected:
		virtual void Serialize(FileStreamWriter* writer) const override;
		virtual void DeSerialize(FileStreamReader* reader) override;
	};



	class IAsset
	{
	public:
		virtual EAssetType GetAssetType() const        = 0;
		virtual const String& GetAssetFullPath() const = 0;
		virtual const String& GetAssetPath() const     = 0;
		virtual const String& GetAssetName() const     = 0;
	public:
		virtual ~IAsset() = default;
	};


	class AssetDataBase : public ObjectFactory<AssetDataBase, IAsset, 2>
	{
	protected:
		virtual void CreateObjectImpl(IAsset* asset) override;
		virtual void DestroyObjectImpl(IAsset* asset) override;
		virtual void ReserveDestroyObjectImpl(IAsset* asset) override;
	};
}