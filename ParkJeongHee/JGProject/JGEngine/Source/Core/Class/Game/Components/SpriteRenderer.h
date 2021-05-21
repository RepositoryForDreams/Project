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
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Serialize(FileStreamWriter* writer)   const override;
		virtual void DeSerialize(FileStreamReader* reader) override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
	public:
		virtual SharedPtr<IRenderItem> PushRenderItem() override;
	public:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;
	};

}