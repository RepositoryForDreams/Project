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
			Scheduler::GetInstance().ScheduleOnceByFrame(1, SchedulePriority::StartGameClass, SCHEDULE_BIND_FN(&GameObjectFactory::UpdateStartObject));
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
			Scheduler::GetInstance().ScheduleOnceByFrame(1, SchedulePriority::DestroyGameClass, SCHEDULE_BIND_FN(&GameObjectFactory::UpdateDestroyObject));
		}
	}
	EScheduleResult GameObjectFactory::UpdateStartObject()
	{
		for (auto& obj : mReserveStartObjects)
		{
			JG_INFO("GameNode : {0}  Start", ws2s(obj->GetName()));
			obj->Start();
		}
		mReserveStartObjects.clear();
		return EScheduleResult::Continue;
	}
	EScheduleResult GameObjectFactory::UpdateDestroyObject()
	{
		for (auto& obj : mReservedDestroyObjects)
		{
			JG_INFO("GameNode : {0}  Destroy", ws2s(obj->GetName()));
			obj->Destory();
		}
		mReservedDestroyObjects.clear();
		return EScheduleResult::Continue;
	}

}