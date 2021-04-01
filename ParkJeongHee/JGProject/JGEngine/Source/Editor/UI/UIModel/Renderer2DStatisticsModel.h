#pragma once

#include "Common/Define.h"
#include "Class/UI/UIModel.h"
#include "Class/Global/Scheduler.h"



namespace JG
{
	class Renderer2DStatisticsModel : public UIModel
	{
		//
		u64 mDrawCalls  = 0;
		u64 mQuadCount = 0;


		SharedPtr<ScheduleHandle> mHandle;
	protected:
		virtual bool Initialize();
		virtual void Destroy();

		void Start();
		void Reset();
	public:
		u64 GetDrawCalls() const;
		u64 GetQuadCount() const;
	public:
		virtual ~Renderer2DStatisticsModel() = default;
	};
}