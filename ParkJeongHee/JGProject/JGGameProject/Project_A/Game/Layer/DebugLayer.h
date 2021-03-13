#pragma once
#include "JGLayer.h"


class DebugLayer : public JGLayer
{

public:
	struct Log
	{
		cocos2d::Label*  Label = nullptr;
		float LifeTime = 2.0f;
		float AccTime = 0.0f;

		Log() = default;
	};

private:
	std::list<Log> mLogs;
	bool mIsDirty = false;



public:
	virtual bool init() override;
	virtual void update(float delta) override;


public:
	void Print(const std::string& text, const cocos2d::Color3B& color, float lifeTime);

	
public:
	CREATE_FUNC(DebugLayer);
};
