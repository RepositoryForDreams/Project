


#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__



#include "cocos2d.h"

class TestScene : public cocos2d::Scene
{
private:
    cocos2d::Sprite* mSprite;
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    virtual void update(float delta) override;
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(TestScene);
};


#endif 