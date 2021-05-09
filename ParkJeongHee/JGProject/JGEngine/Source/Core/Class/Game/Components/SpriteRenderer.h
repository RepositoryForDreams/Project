#pragma once
#include "BaseRenderer.h"


namespace JG
{
	class SpriteRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		SharedPtr<StandardSpriteRenderItem> mSpriteRI;
	public:
		virtual ~SpriteRenderer() = default;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
	public:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;
	};

}