﻿#pragma once

#include "JGCore.h"
#include "Class/Layer.h"


namespace JG
{
	class EditorLayer : public ILayer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach()  override;


		virtual void Update() override;
		virtual void LateUpdate() override;


		virtual void Begin() override;
		virtual void Destroy()  override;

		virtual void OnEvent(IEvent& e)  override;

		virtual String GetLayerName()  override;

	private:
		bool EditorLayer::Resize(AppResizeEvent& e);
	};
}

