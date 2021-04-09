#pragma once
#include "Class/Layer.h"
#include "Class/FileIO.h"
namespace JG
{

	class UILayer : public ILayer
	{

	public:
		virtual ~UILayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;
	private:
		void LoadUISettings(const String& fileName);
		void SaveUISettings(const String& fileName);
	};

}
