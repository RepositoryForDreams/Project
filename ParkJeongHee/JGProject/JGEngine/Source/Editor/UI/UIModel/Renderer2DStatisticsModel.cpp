#include "pch.h"
#include "Renderer2DStatisticsModel.h"

#include "Graphics/Renderer.h"


namespace JG
{



	bool Renderer2DStatisticsModel::Initialize()
	{
		Start();
		return true;
	}

	void Renderer2DStatisticsModel::Destroy()
	{
		Reset();
	}

	void Renderer2DStatisticsModel::Start()
	{
		Reset();

		mHandle = Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, 0,
			[&]()->EScheduleResult
		{
			auto stats = Renderer2D::GetStats();
			mDrawCalls = stats.DrawCalls;
			mQuadCount = stats.QuadCount;

			return EScheduleResult::Continue;
		});
	}

	void Renderer2DStatisticsModel::Reset()
	{
		if (mHandle && mHandle->IsValid())
		{
			mHandle->Reset();
			mHandle = nullptr;
		}

		mDrawCalls = 0;
		mQuadCount = 0;
	}

	u64 Renderer2DStatisticsModel::GetDrawCalls() const
	{
		return mDrawCalls;
	}
	u64 Renderer2DStatisticsModel::GetQuadCount() const
	{
		return mQuadCount;
	}

}