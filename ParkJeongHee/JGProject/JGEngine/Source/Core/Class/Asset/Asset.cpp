#include "pch.h"
#include "Asset.h"


namespace JG
{
	void StaticMeshAssetStock::Serialize(FileStreamWriter* writer) const {
		writer->Write(Name);
		writer->Write(IsSkinned);
		writer->Write(SubMeshNames);
		writer->Write(Vertices);
		writer->Write(Indices);

	}
	void StaticMeshAssetStock::DeSerialize(FileStreamReader* reader) {
		reader->Read(&Name);
		reader->Read(&IsSkinned);
		reader->Read(&SubMeshNames);
		reader->Read(&Vertices);
		reader->Read(&Indices);

	}

	u64 AssetBase::GetAssetID() const
	{
		return mAssetID;
	}

	const String& AssetBase::GetAssetFullPath() const
	{
		return mAssetFullPath;
	}
	const String& AssetBase::GetAssetPath() const
	{
		return mAssetPath;
	}
	const String& AssetBase::GetAssetName() const
	{
		return mName;
	}

	const String& AssetBase::GetExtension() const
	{
		return mExtension;
	}

	void AssetDataBase::CreateObjectImpl(IAsset* asset)
	{

	}
	void AssetDataBase::DestroyObjectImpl(IAsset* asset)
	{

	}
	void AssetDataBase::ReserveDestroyObjectImpl(IAsset* asset)
	{

	}

}