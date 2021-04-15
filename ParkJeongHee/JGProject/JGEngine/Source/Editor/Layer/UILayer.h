#pragma once
#include "Class/Layer.h"
#include "Class/FileIO.h"
namespace JG
{
	enum class MenuType
	{
		MainMenu,
		ContextMenu,
	};
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
	public:
		static void ShowContextMenu(const Type& type);

	private:
		void LoadUISettings(const String& fileName);
		void SaveUISettings(const String& fileName);
	private:
		EScheduleResult MenuUpdate();
		static void BeginMenu(const MenuItemNode* Node);
		static void EndMenu(const MenuItemNode* Node);
	};

}
