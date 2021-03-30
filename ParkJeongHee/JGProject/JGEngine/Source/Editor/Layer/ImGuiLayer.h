#pragma once
#include "Class/Layer.h"





namespace JG
{
	class ImGuiLayer : public ILayer
	{
	public:
		virtual ~ImGuiLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;




		virtual void Begin() override;
		virtual void Destroy() override {}

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;


		EScheduleResult Update();
	private:
		bool Resize(AppResizeEvent& e);
	};
}