#include "JGScene.h"
#include "../Game/Layer/DebugLayer.h"



bool JGScene::init()

{
	if(!cocos2d::Scene::init())
	{
		return false;
	}
	addChild(DebugLayer::create(), 0, "DebugLayer");

	return true;
}

void JGScene::LogOnScreen(const cocos2d::Color3B& color, float lifeTime, const char* text, ...)
{
	auto debugLayer = static_cast<DebugLayer*>(getChildByName("DebugLayer"));
	if(debugLayer)
	{
		int len = strlen(text);


		std::string buffer; buffer.resize(len + 1);

		va_list t_va;
		va_start(t_va, text);
		vsnprintf(&buffer[0], len + 1, text, t_va);
		va_end(t_va);

		
		debugLayer->Print(buffer, color, lifeTime);
	}
}
