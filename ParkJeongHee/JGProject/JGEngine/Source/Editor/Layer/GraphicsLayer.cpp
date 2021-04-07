#include "pch.h"
#include "GraphicsLayer.h"
#include "Application.h"
#include "Class/Asset/AssetImporter.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
namespace JG
{
	void GraphicsLayer::OnAttach()
	{

	}

	void GraphicsLayer::OnDetach()
	{

	}

	void GraphicsLayer::Begin()
	{
		auto fbxAssetPath = CombinePath(Application::GetAssetPath(), TT("Samples/Model/FBX/Sphere.fbx"));
		auto outputPath = CombinePath(Application::GetAssetPath(), TT("Samples/Model"));
		auto meshPath = CombinePath(Application::GetAssetPath(), TT("Samples/Model/Sphere.mesh"));
		AssetImportSettings settings;
		settings.AssetPath = fbxAssetPath;
		settings.OutputPath = outputPath;
		//AssetImporter::Import(settings);
		//C:\Project\ParkJeongHee\JGProject\JGGameProject\Project_C\Asset\Samples\Model
		auto mesh = IMesh::CreateFromFile(meshPath);
	}

	void GraphicsLayer::Destroy()
	{
	}

	void GraphicsLayer::OnEvent(IEvent& e)
	{
	}

	String GraphicsLayer::GetLayerName()
	{
		return String();
	}

	EScheduleResult GraphicsLayer::Update()
	{
		return EScheduleResult();
	}
}

