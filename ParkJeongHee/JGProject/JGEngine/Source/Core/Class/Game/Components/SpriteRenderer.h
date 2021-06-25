#pragma once
#include "BaseRenderer.h"


namespace JG
{
	class SpriteRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		SharedPtr<Standard2DRenderItem> mSpriteRI;
	public:
		virtual ~SpriteRenderer() = default;
	public:
		SpriteRenderer();
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual SharedPtr<IRenderItem> PushRenderItem() override;
	public:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;
	};

}