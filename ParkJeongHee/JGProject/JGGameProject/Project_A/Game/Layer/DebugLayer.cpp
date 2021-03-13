#include "DebugLayer.h"


using namespace cocos2d;
using namespace cocos2d::ui;

bool DebugLayer::init()
{
	if(!JGLayer::init())
	{
		return false;
	}
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin      = Director::getInstance()->getVisibleOrigin();

	
	this->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);


	auto sprite = Sprite::create("blank.png");
	this->addChild(sprite);
	sprite->setPosition(visibleSize.width * 0.5f, visibleSize.height * 0.5);
	this->scheduleUpdate();

	return true;
}

void DebugLayer::update(float delta)
{


	mLogs.remove_if([&](Log& log)
	{
		log.AccTime += delta;
		if (log.AccTime >= log.LifeTime)
		{
			this->removeChild(log.Label);
			log.Label = nullptr;
			return true;
		}
		return false;
	});


	float yOffset = Director::getInstance()->getVisibleSize().height - 20;
	for(auto& log : mLogs)
	{
		log.Label->setPosition(20, yOffset);
		yOffset -= 20.0f;
	}
	
}

void DebugLayer::Print(const std::string& text, const cocos2d::Color3B& color, float lifeTime)
{
	Log log;
	log.Label = Label::createWithSystemFont(text, "Arial", 16);
	log.Label->setColor(color);
	log.Label->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	log.LifeTime = lifeTime;
	mLogs.push_back(log);
	this->addChild(log.Label);
	
	mIsDirty = true;
}




