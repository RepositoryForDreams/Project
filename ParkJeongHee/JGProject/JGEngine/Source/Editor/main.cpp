#include <crtdbg.h>

#include "JGCore.h"
#include "JGEditor.h"


using namespace JG;

void Leak()
{
	_CrtDumpMemoryLeaks();
}
int main(int argc, char** argv)
{
	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	ApplicaionProps props;
	props.WindowPlatform = EWindowPlatform::Windows;
	props.WindowProps.TitleName = L"JGEditor";
	props.WindowProps.Width = 1920;
	props.WindowProps.Height = 1080;
	Application* app = new JGEditor();

	if (app->Create(props))
	{
		app->Run();
	}

	app->Destroy();

	delete app;
	app = nullptr;
	std::atexit(Leak);
	return 0;
}
