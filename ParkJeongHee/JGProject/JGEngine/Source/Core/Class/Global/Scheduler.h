#pragma once
#include "JGCore.h"
#include <thread>


namespace JG
{
	enum class EScheduleState
	{
		Wait,
		Run,
		Compelete,
	};
	enum class EScheduleType
	{
		SyncByTick,
		SyncByFrame,
		Async
	};
	enum class EScheduleResult
	{
		Continue,
		Break,
	};
	using SyncTaskFunction  = std::function<EScheduleResult()>;
	using AsyncTaskFunction = std::function<void()>;
	class Timer;
	class Application;
	class Scheduler;
	class ScheduleHandle
	{
		friend Scheduler;

		u64 mID = 0;
		EScheduleState mState;
		EScheduleType  mType;
	public:
		~ScheduleHandle();
	public:
		u64 GetID() const {
			return mID;
		}
		EScheduleState GetState() const {
			return mState;
		}
		EScheduleType GetType() const {
			return mType;
		}
		bool IsValid() const;
		void Reset();
	};


	class Scheduler : public GlobalSingleton<Scheduler>
	{
		friend ScheduleHandle;
		friend Application;
		static const u64 SCHEDULE_NULL_ID = -1;
	private:
		struct SyncTaskByTick
		{
			SharedPtr<ScheduleHandle> Handle;
			u64 ID = SCHEDULE_NULL_ID;
			f32 Delay     = 0.0f;
			f32 TickCycle = 0.0f;
			i32 Repeat   = 0;
			i32 Priority = 0;
			SyncTaskFunction Function;
			f32 Tick = 0.0f;
			i32 CallCount = 0;
		};
		struct SyncTaskByFrame
		{
			SharedPtr<ScheduleHandle> Handle;
			u64 ID = SCHEDULE_NULL_ID;
			f32 Delay = 0.0f;
			i32 FrameCycle = 0;
			i32 Repeat = 0;
			i32 Priority = 0;
			SyncTaskFunction Function;
			i32 Frame = 0;
			i32 CallCount = 0;
		};
		struct AsyncTask
		{
			SharedPtr<ScheduleHandle> Handle;
			AsyncTaskFunction Function;
			std::thread Thread;

			void SetState(EScheduleState State) {
				Handle->mState = State;
			}
		};

	private:
		u64 mIDOffset = 0;
		std::mutex mMutex;
		Queue<u64> mIDQueue;
		
		SharedPtr<Timer> mScheduleTimer;
		//
		Dictionary<u64, SharedPtr<SyncTaskByTick>>  mSyncTaskByTickPool;
		Dictionary<u64, SharedPtr<SyncTaskByFrame>> mSyncTaskByFramePool;
		Dictionary<u64, SharedPtr<AsyncTask>> mAsyncTaskPool;

		//
		Dictionary<i32, List<WeakPtr<SyncTaskByTick>>>  mSortedSyncTaskByTicks;
		Dictionary<i32, List<WeakPtr<SyncTaskByFrame>>> mSortedSyncTaskByFrames;
	public:
		Scheduler();
		~Scheduler();
	public:
		SharedPtr<ScheduleHandle> Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleAsync(const AsyncTaskFunction& task);
	private:
		void Update();
		void Update(SharedPtr<SyncTaskByTick> task);
		void Update(SharedPtr<SyncTaskByFrame> task);
		void ResultProcess(SharedPtr<SyncTaskByTick> task, EScheduleResult result);
		void ResultProcess(SharedPtr<SyncTaskByFrame> task, EScheduleResult result);
		void RemoveSchedule(const ScheduleHandle& handle);
		u64 ReceiveScheduleID();
	};


}