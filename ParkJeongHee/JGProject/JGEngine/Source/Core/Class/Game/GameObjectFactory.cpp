#include "pch.h"
#include "GameObjectFactory.h"


namespace JG
{
	void GameObjectFactory::CreateObjectImpl(IGameObject* gameObject)
	{
		bool is_run_start = mReserveStartObjects.empty() == false;

		mReserveStartObjects.push_back(gameObject);

		if (is_run_start == false)
		{
			Scheduler::GetInstance().ScheduleOnceByFrame(1, SchedulePriority::StartGameClass, SCHEDULE_BIND_FN(&GameObjectFactory::StartObject));
		}

	}
	void GameObjectFactory::DestroyObjectImpl(IGameObject* gameObject)
	{

	}
	void GameObjectFactory::ReserveDestroyObjectImpl(IGameObject* gameObject)
	{
		bool is_run_destroy = mReservedDestroyObjects.empty() == false;

		mReservedDestroyObjects.push_back(gameObject);

		if (is_run_destroy == false)
		{
			Scheduler::GetInstance().ScheduleOnceByFrame(1, SchedulePriority::DestroyGameClass, SCHEDULE_BIND_FN(&GameObjectFactory::DestroyObject));
		}
	}
	EScheduleResult GameObjectFactory::StartObject()
	{
		for (auto& obj : mReserveStartObjects)
		{
			obj->Start();
		}
		mReserveStartObjects.clear();
		return EScheduleResult::Continue;
	}
	EScheduleResult GameObjectFactory::DestroyObject()
	{
		for (auto& obj : mReservedDestroyObjects)
		{
			obj->Destory();
		}
		mReservedDestroyObjects.clear();
		return EScheduleResult::Continue;
	}

}