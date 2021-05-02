#pragma once
#include "Class/Game/GameComponent.h"
#include "Graphics/Camera.h"



namespace JG
{
	class GameCamera : public GameComponent
	{
		JGCLASS
		SharedPtr<Camera> mCamera;
	protected:
		virtual void Start() override;
		virtual void Destory() override;
	public:
		virtual ~GameCamera() = default;
	public:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;
	};
}