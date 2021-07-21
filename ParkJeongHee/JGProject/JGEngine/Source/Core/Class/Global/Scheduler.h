#pragma once
#include "JGCore.h"
#include <thread>


namespace JG
{
#define SCHEDULE_BIND_FN(func) std::bind(func, this)


	class SchedulePriority
	{
	public:
		enum 
		{
			// System 
			BeginSystem      = -1000000,
			// Layer
			ImGuiSystemLayer		 = -900000,
			GameLayer        = -800000,
			UISystemLayer          = -700000,
			MainMenuLayer    = -600000,
			//
			Default = 0,
			// GameLogic
			DestroyGameClass = 100000,
			UpdateGameClass  = 200000,

			// System
			OnGUI         = 1000000,
			DestroyObject = 2000000,
			EndSystem     = 10000000,
		};
	};
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
	public:
		void* UserData = nullptr;
	};
	using SyncTaskFunction  = std::function<EScheduleResult()>;
	using AsyncTaskFunction = std::function<void(void*)>;
	// Worker 를 생성하여 스케쥴 작성
	class Scheduler : public GlobalSingleton<Scheduler>
	{
		friend ScheduleHandle;
		friend Application;
		static const u64 SCHEDULE_NULL_ID = -1;
	private:
		class SyncTask
		{
		public:
			SharedPtr<ScheduleHandle> Handle;
			u64 ID = SCHEDULE_NULL_ID;
			i32 Repeat = 0;
			i32 Priority = 0;
			SyncTaskFunction Function;
			i32 CallCount = 0;
		public:
			virtual ~SyncTask() = default;
		public:
			virtual EScheduleType GetScheduleType() const = 0;
		};
		class SyncTaskByTick : public SyncTask
		{
		public:
			f32 Delay     = 0.0f;
			f32 TickCycle = 0.0f;
			f32 Tick = 0.0f;
		public:
			virtual ~SyncTaskByTick() = default;
			virtual EScheduleType GetScheduleType() const {
				return EScheduleType::SyncByTick;
			}
		public:

		};
		class SyncTaskByFrame : public SyncTask
		{
		public:
			i32 Delay = 0;
			i32 FrameCycle = 0;
			i32 Frame = 0;
		public:
			virtual ~SyncTaskByFrame() = default;
			virtual EScheduleType GetScheduleType() const {
				return EScheduleType::SyncByFrame;
			}
		};
		struct AsyncTask
		{
			SharedPtr<ScheduleHandle> Handle;
			AsyncTaskFunction Function;
			void SetState(EScheduleState State) {
				Handle->mState = State;
			}
		};
	private:
		u64 mIDOffset = 0;
		Queue<u64> mIDQueue;
		
		SharedPtr<Timer> mScheduleTimer;
		//
		Dictionary<u64, SharedPtr<SyncTask>>  mSyncTaskPool;
		SortedDictionary<i32, List<WeakPtr<SyncTask>>>  mSortedSyncTasks;
		Queue<WeakPtr<SyncTask>> mReservedSyncTasks;
		//Dictionary<u64, SharedPtr<SyncTaskByTick>>  mSyncTaskByTickPool;
		//Dictionary<u64, SharedPtr<SyncTaskByFrame>> mSyncTaskByFramePool;
		//
		//SortedDictionary<i32, List<WeakPtr<SyncTaskByTick>>>  mSortedSyncTaskByTicks;
		//SortedDictionary<i32, List<WeakPtr<SyncTaskByFrame>>> mSortedSyncTaskByFrames;
		//
		//Queue<WeakPtr<SyncTaskByTick>> mReservedSyncTaskByTick;
		//Queue<WeakPtr<SyncTaskByFrame>> mReservedSyncTaskByFrame;

		// Thread 관련
		Queue<SharedPtr<AsyncTask>> mAsyncTaskQueue;
		List<std::thread>			mThreads;
		i32		   mMaxThreadCount = 0;
		std::mutex mMutex;
		std::condition_variable mRunAsyncTaskConVar;
		bool mIsRunAsyncTaskAll = true;
		bool mIsRunSyncTaskAll  = false;
	public:
		Scheduler();
		virtual ~Scheduler();
	public:
		SharedPtr<ScheduleHandle> Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task);
		SharedPtr<ScheduleHandle> ScheduleAsync(const AsyncTaskFunction& task, void* userData = nullptr, u64 dataSize = 0);


		void FlushAsyncTask(bool isRestart = true);
		const Timer* GetScheduleTimer() const;
	private:
		void Update();
		void Update(SyncTaskByTick* task);
		void Update(SyncTaskByFrame* task);
		void ResultProcess(SyncTaskByTick* task, EScheduleResult result);
		void ResultProcess(SyncTaskByFrame* task, EScheduleResult result);
		void RemoveSchedule(const ScheduleHandle& handle);
		u64 ReceiveScheduleID();
	};


}