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
}