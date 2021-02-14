#pragma once


#include "cocos2d.h"

class JGScene : public cocos2d::Scene
{
public:
	virtual bool init() override;
protected:
	void LogOnScreen(const cocos2d::Color3B& color, float lifeTime, const char* text, ...);
};
