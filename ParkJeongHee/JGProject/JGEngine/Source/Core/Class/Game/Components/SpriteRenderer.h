#pragma once
#include "BaseRenderer.h"


namespace JG
{
	class SpriteRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		bool mIsRunning = false;
		SharedPtr<StandardSpriteRenderItem> mSpriteRI;
	public:
		virtual ~SpriteRenderer() = default;
	public:
		


	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;
	};

}