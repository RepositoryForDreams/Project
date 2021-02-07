#pragma once
#include "../JGScene.h"


class DevScene : public JGScene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init() override;


	CREATE_FUNC(DevScene);
private:
	

	
};
