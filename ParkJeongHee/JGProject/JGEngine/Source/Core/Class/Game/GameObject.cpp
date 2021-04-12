#include "pch.h"
#include "GameObject.h"
#include "Class/Global/Scheduler.h"

namespace JG
{
	GameObject::~GameObject()
	{
		for (auto& handle : mScheduleHandleList)
		{
			handle->Reset();
		}
		mScheduleHandleList.clear();
	}
	SharedPtr<ScheduleHandle> GameObject::Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task)
	{
		ArrangeScheduleHandle();
		auto handle = Scheduler::GetInstance().Schedule(delay, tickCycle, repeat, SchedulePriority::UpdateGameClass + priority, task);
		mScheduleHandleList.push_back(handle);
		return handle;
	}
	SharedPtr<ScheduleHandle> GameObject::ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task)
	{
		ArrangeScheduleHandle();
		auto handle = Scheduler::GetInstance().ScheduleOnce(delay, SchedulePriority::UpdateGameClass + priority, task);
		mScheduleHandleList.push_back(handle);
		return handle;
	}
	SharedPtr<ScheduleHandle> GameObject::ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task)
	{
		ArrangeScheduleHandle();
		auto handle = Scheduler::GetInstance().ScheduleByFrame(delayFrame, frameCycle, repeat, SchedulePriority::UpdateGameClass + priority, task);
		mScheduleHandleList.push_back(handle);
		return handle;
	}
	SharedPtr<ScheduleHandle> GameObject::ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task)
	{
		ArrangeScheduleHandle();
		auto handle = Scheduler::GetInstance().ScheduleOnceByFrame(delayFrame, SchedulePriority::UpdateGameClass + priority, task);
		mScheduleHandleList.push_back(handle);
		return handle;
	}
	SharedPtr<ScheduleHandle> GameObject::ScheduleAsync(const AsyncTaskFunction& task)
	{
		return Scheduler::GetInstance().ScheduleAsync(task);
	}
	u64 GameObject::GetID() const
	{
		return mID;
	}
	const String& GameObject::GetName() const
	{
		return mName;
	}
	void GameObject::SetName(const String& name)
	{
		mName = name;
	}
	void GameObject::ArrangeScheduleHandle()
	{
		u64 cnt = mScheduleHandleList.size();
		for (u64 i = 0; i < cnt;)
		{
			if (mScheduleHandleList[i]->IsValid() == false)
			{
				std::swap(mScheduleHandleList.end() - 1, mScheduleHandleList.begin() + i);
				mScheduleHandleList.pop_back();
				--cnt;
			}
			else
			{
				++i;
			}
		}
	}
}

