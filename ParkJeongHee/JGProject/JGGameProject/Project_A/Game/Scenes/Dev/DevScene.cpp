#include "DevScene.h"

#include "../Game/UIExtension/UIExtension.h"


using namespace cocos2d;

cocos2d::Scene* DevScene::createScene()
{
	return DevScene::create();
}

bool DevScene::init()
{
	if (!JGScene::init())
	{
		return false;
	}
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();




	auto redSprite   = Sprite::create("blank.png");
	auto blueSprite  = Sprite::create("blank.png");
	auto greenSprite = Sprite::create("blank.png");
	
	auto bt = ui::Button::create("Blank.png");
	


	redSprite->setColor(Color3B::RED);
	blueSprite->setColor(Color3B::BLUE);
	greenSprite->setColor(Color3B::GREEN);


	redSprite->setPosition(0, 0);
	blueSprite->setPosition(1300, 200);
	greenSprite->setPosition(900, 700);
	
	blueSprite->addChild(redSprite);
	//addChild(redSprite, 0, "Red");
	addChild(blueSprite, 0, "Blue");
	addChild(greenSprite, 0, "Green");

	// Debug
	{
		char buf[1024] = {0, };

	sprintf(buf, R"(
1. Red
Position      x : {%f}  y : {%f}  
Scale         x : {%f}  y : {%f}
ContentsSize  w : {%f}  y : {%f} 
AncherPoint   x : {%f}  y : {%f}

2. Blue
Position      x : {%f}  y : {%f} 
Scale         x : {%f}  y : {%f}
ContentsSize  w : {%f}  y : {%f} 
AncherPoint   x : {%f}  y : {%f}

3. Green
Position      x : {%f}  y : {%f}  
Scale         x : {%f}  y : {%f}
ContentsSize  w : {%f}  y : {%f}
AncherPoint   x : {%f}  y : {%f}
)", 
redSprite->getPositionX(),		   redSprite->getPositionY(),
redSprite->getScaleX(),			   redSprite->getScaleY(),
redSprite->getContentSize().width, redSprite->getContentSize().height,
redSprite->getAnchorPoint().x,     redSprite->getAnchorPoint().y,

blueSprite->getPositionX(), blueSprite->getPositionY(),
blueSprite->getScaleX(), blueSprite->getScaleY(),
blueSprite->getContentSize().width, blueSprite->getContentSize().height,
blueSprite->getAnchorPoint().x, blueSprite->getAnchorPoint().y,

greenSprite->getPositionX(), greenSprite->getPositionY(),
greenSprite->getScaleX(), greenSprite->getScaleY(),
greenSprite->getContentSize().width, greenSprite->getContentSize().height,
greenSprite->getAnchorPoint().x, greenSprite->getAnchorPoint().y
);

		
		auto debugLabel = Label::createWithSystemFont(
			buf, "Arial", 16);
		debugLabel->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		debugLabel->setPosition(visibleSize.width, visibleSize.height);

		addChild(debugLabel);


		auto label = Label::createWithSystemFont("DevScene", "Arial", 32);
		label->setPosition(visibleSize.width * 0.5f, visibleSize.height * 0.5f);
		addChild(label);

		//
		{
			auto panel = Sprite::create("blank.png");
			panel->setContentSize(Size(1000, 1000));
			panel->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
			auto btn = UIExtension::CreateButton(
				panel,
				Vec2(panel->getContentSize().width * 0.5f, panel->getContentSize().height * 0.5f),
				Size(400,200),
				"blank.png", "", "", ui::Widget::TextureResType::LOCAL);

			btn->setColor(Color3B::GRAY);
			btn->setTitleLabel(Label::createWithSystemFont("NextSscene..", "Arial", 32));
			//btn->setPosition(Vec2(panel->getContentSize().width * 0.5f, panel->getContentSize().height * 0.5f));
			//btn->setScale9Enabled(true);
			//btn->setContentSize(Size(200, 400));

			addChild(panel);
			//panel->addChild(btn);
		}
	}
	return true;
}
