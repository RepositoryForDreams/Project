#include "UIExtension.h"


using namespace cocos2d;
using namespace cocos2d::ui;

cocos2d::ui::Button* UIExtension::CreateButton(
	cocos2d::Node* pNode,
	const cocos2d::Vec2& startPos,
	const cocos2d::Size& contentSize,
	const std::string& normalImage, 
	const std::string& selectedImage,
	const std::string& disableImage,
	cocos2d::ui::Widget::TextureResType texType)
{

	auto btn = Button::create(normalImage, selectedImage, disableImage,
		texType);

	btn->setPosition(startPos);
	btn->setContentSize(contentSize);
	btn->setScale9Enabled(true);
	
	pNode->addChild(btn);
	return btn;
}
