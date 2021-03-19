#pragma once
#include "JGCore.h"



namespace JG
{
	using SyncTask  = std::function<void()>;
	using AsyncTask = std::function<void()>;
	class ScheduleHandle;
	enum class EScheduleState
	{
		Wait,
		Run,
		Compelete,
	};
	enum class EScheduleWay
	{

	};
	class Scheduler : public GlobalSingleton<Scheduler>
	{
		//- Sequence �켱���� -> �⺻ 0
		// Start, Update, LateUpdate
		// tick ���� ��� ȣ��  ( ���������� ȣ��), �񵿱� ����, Delay���
		// Frame ���� ��� ȣ�� ( ���������� ȣ�� ), �񵿱� ����
	
		// TimerManager���� Timer ����
		// StopWatch, Timer, 
	public:
		/*
		@Param
		delay : ������ ���� ������ ����

		*/
		SharedPtr<ScheduleHandle> Schedule(float delay, float tick, i32 repeat, i32 priority, const SyncTask& task);
		SharedPtr<ScheduleHandle> ScheduleOnce(float delay, i32 priority, const SyncTask& task);
		SharedPtr<ScheduleHandle> ScheduleByFrame(i32 delayFrame, i32 perFrame, i32 repeat, i32 priority, const SyncTask& task);
		SharedPtr<ScheduleHandle> ScheduleAsync(float delay, const AsyncTask& task);
	};

	class ScheduleHandle
	{
		u64 mID = 0;
		EScheduleState mState;
	public:
		u64 GetID() const {
			return mID;
		}
		EScheduleState GetState() const {
			return mState;
		}
	};
}