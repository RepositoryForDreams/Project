#include "pch.h"
#include "TimerManager.h"

using namespace std::chrono;
using jg_clock = std::chrono::steady_clock;
namespace JG
{

    void TimerManager::Timer::Start()
    {
        mIsRun = true;
    }
    void TimerManager::Timer::Tick()
    {
        if (mIsFirst)
        {
            mTimePoint = jg_clock::now();
            mIsFirst = false;
            return;
        }
        if (!mIsRun)
        {
            mDeltaTime  = 0.0f;
            mFrameCount = 0;
            mTimePoint  = jg_clock::now();
            return;
        }
 


        duration<float> sec = jg_clock::now() - mTimePoint;
        mDeltaTime = sec.count();
        if (mDeltaTime >= 0.0f)
        {
            mTotalTime += mDeltaTime;

            ++mFrameCount;
            mTimeElapsed += mDeltaTime;
            if (mTimeElapsed >= 1.0f)
            {
                mFPS = mFrameCount;
                mFrameCount  = 0;
                mTimeElapsed = 0.0f;
            }
        }
        mTimePoint = jg_clock::now();
    }
    void TimerManager::Timer::Stop()
    {
        mIsRun = false;
    }
    f32 TimerManager::Timer::GetTotalTime(ETimeStepType stepType) const
    {
        return mTotalTime;
    }
    f32 TimerManager::Timer::GetTick(ETimeStepType stepType) const
    {
        return mDeltaTime;
    }
    u64 TimerManager::Timer::GetFPS() const
    {
        return mFPS;
    }


    void TimerManager::Update()
    {
        static i32 delayFameCount = 0;
        if (delayFameCount < 5) delayFameCount++;

        for (auto& _pair : mTimers)
        {
            _pair.second->Tick();
        }
    }

    TimerHandle TimerManager::CreateTimer()
    {
        std::lock_guard<std::mutex> lock(mTimerMutex);
        u64 ID = 0;
        if (mTimerIDQueue.empty())
        {
            ID = mTimerIDOffset++;
        }
        else
        {
            ID = mTimerIDQueue.front(); mTimerIDQueue.pop();
        }
        mTimers[ID] = CreateUniquePtr<Timer>();
        TimerHandle handle;
        handle.mOwnerTimer = mTimers[ID].get();
        handle.mTimerID = ID;

        return std::move(handle);
    }
    void TimerManager::DestroyTimer(u64 timerID)
    {
        std::lock_guard<std::mutex> lock(mTimerMutex);
        auto iter = mTimers.find(timerID);
        if (iter != mTimers.end())
        {
            mTimers.erase(timerID);
            mTimerIDQueue.push(timerID);
        }
    }
    TimerHandle::~TimerHandle()
    {
        if (TimerManager::IsValid() && mTimerID != TimerManager::TIMER_NULL_ID)
        {
            TimerManager::GetInstance().DestroyTimer(mTimerID);
        }
    }
    TimerHandle::TimerHandle(TimerHandle&& rhs) noexcept
    {
        mOwnerTimer = rhs.mOwnerTimer;
        mTimerID = rhs.mTimerID;

        rhs.mOwnerTimer = nullptr;
        rhs.mTimerID = TimerManager::TIMER_NULL_ID;
    }
    TimerHandle& TimerHandle::operator=(TimerHandle&& rhs) noexcept
    {
        mOwnerTimer = rhs.mOwnerTimer;
        mTimerID = rhs.mTimerID;

        rhs.mOwnerTimer = nullptr;
        rhs.mTimerID = TimerManager::TIMER_NULL_ID;
        return *this;
    }
    TimerHandle Timer::Create()
    {
        if (TimerManager::IsValid())
        {
            return TimerManager::GetInstance().CreateTimer();
        }
        return TimerHandle();
    }
}