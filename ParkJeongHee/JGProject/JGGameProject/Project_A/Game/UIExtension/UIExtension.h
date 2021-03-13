#pragma once

#include "cocos2d.h"
#include "ui/UIButton.h"


class UIExtension
{
public:
	static cocos2d::ui::Button* CreateButton(
		cocos2d::Node* pNode,
		const cocos2d::Vec2& startPos,
		const cocos2d::Size& contentSize,
		const std::string&   normalImage,
		const std::string&   selectedImage,
		const std::string&   disableImage,
		cocos2d::ui::Widget::TextureResType texType);
};
