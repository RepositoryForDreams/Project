#pragma once
#include "Class/Layer.h"

namespace JG
{

	class UILayer : public ILayer
	{
	public:
		virtual ~UILayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;


		virtual void Update() override;
		virtual void LateUpdate() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;
	};

}
