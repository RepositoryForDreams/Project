#pragma once
#include "Class/Layer.h"

namespace JG
{
	// UIManager
	// bool IsOpen
	// bool Is
	class MainMenuLayer : public ILayer
	{
	public:
		virtual ~MainMenuLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;


		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;
		
		EScheduleResult Update();
	};

}
