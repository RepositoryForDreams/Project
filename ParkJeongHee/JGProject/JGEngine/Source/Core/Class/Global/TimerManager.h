#pragma once
#include "JGCore.h"
#include <chrono>


namespace JG
{
	using jg_clock = std::chrono::steady_clock;

	enum class ETimeStepType
	{
		Hour,
		Minute,
		Second,
		MiliSecond
	};

	class Timer;
	class TimerHandle;
	class TimerManager : public GlobalSingleton<TimerManager>
	{
		static const u64 TIMER_NULL_ID = -1;
		friend TimerHandle;
		friend Timer;
	private:
		class Timer
		{
			friend TimerManager;
		private:
			jg_clock::time_point mTimePoint;
			f32 mTotalTime = 0.0f;
			f32 mDeltaTime = 0.0f;
			f32 mTimeElapsed = 0.0f;
			u32 mFPS = 0;
			u32 mFrameCount = 0;
			bool mIsRun = false;
			bool mIsFirst = true;
		private:
			void Start();
			void Tick();
			void Stop();
		public:
			f32 GetTotalTime(ETimeStepType stepType = ETimeStepType::Second) const;
			f32 GetTick(ETimeStepType stepType = ETimeStepType::Second) const;
			u64 GetFPS() const;
		};

	private:
		u64 mTimerIDOffset = 0;
		std::mutex mTimerMutex;
		Queue<u64> mTimerIDQueue;
		Dictionary<u64, UniquePtr<Timer>> mTimers;
	public:
		void Update();
	private:
		TimerHandle CreateTimer();
		void DestroyTimer(u64 timerID);
	};

	class TimerHandle
	{
		friend TimerManager;
		u64 mTimerID = TimerManager::TIMER_NULL_ID;
		TimerManager::Timer* mOwnerTimer = nullptr;
	private:
		TimerHandle(const TimerHandle& copy) = delete;
		TimerHandle& operator=(const TimerHandle& copy) const = delete;
	public:
		TimerHandle() = default;
		~TimerHandle();
		TimerHandle(TimerHandle&& rhs) noexcept;
		TimerHandle& operator=(TimerHandle&& rhs) noexcept;
	public:
		bool IsValid() const {
			return mOwnerTimer != nullptr;
		}
		TimerManager::Timer* operator->()
		{
			return mOwnerTimer;
		}
	};

	class Timer
	{
	public:
		static TimerHandle Create();
	};
	
}