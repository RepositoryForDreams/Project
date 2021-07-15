#pragma once
#include "Class/Game/Components/BaseRenderer.h"
#include "Class/Asset/Asset.h"

namespace JG
{
	class StaticMeshRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		SharedPtr<StandardStaticMeshRenderItem> mStaticRI;
		String  mMeshPath;
		AssetID mMeshID;
		Asset<IMesh>* mMeshAsset = nullptr;
	public:
		virtual ~StaticMeshRenderer() = default;
	public:
		StaticMeshRenderer();
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
	public:
		void SetMesh(const String& path);
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual SharedPtr<IRenderItem> PushRenderItem() override;
	public:
		virtual void OnChange(const ChangeData & data) override;
		virtual void OnInspectorGUI() override;
	};
}