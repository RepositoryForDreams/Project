#pragma once
#include "Class/SystemLayer.h"
#include "Class/FileIO.h"
namespace JG
{
	enum class MenuType
	{
		MainMenu,
		ContextMenu,
	};
	class UILayer : public ISystemLayer
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
	public:
		static bool ShowContextMenu(const Type& type);

	private:
		void LoadUISettings(const String& fileName);
		void SaveUISettings(const String& fileName);
	private:
		EScheduleResult MenuUpdate();
		static void BeginMenu(const MenuItemNode* Node);
		static void EndMenu(const MenuItemNode* Node);
	};

}
