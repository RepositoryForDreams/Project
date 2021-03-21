#include "pch.h"
#include "Scheduler.h"

namespace JG
{
    ScheduleHandle::~ScheduleHandle()
    {
        Reset();
    }
    bool ScheduleHandle::IsValid() const
    {
        return mID != Scheduler::SCHEDULE_NULL_ID;
    }
    void ScheduleHandle::Reset()
    {
        if (Scheduler::IsValid() && mID != Scheduler::SCHEDULE_NULL_ID)
        {
            Scheduler::GetInstance().RemoveSchedule(*this);
            mID = Scheduler::SCHEDULE_NULL_ID;
        }
    }

    Scheduler::Scheduler()
    {
        mScheduleTimer = Timer::Create();
        mScheduleTimer->Start();
    }
    Scheduler::~Scheduler()
    {
        for (auto& _pair : mSyncTaskByTickPool)
        {
            _pair.second->ID          = SCHEDULE_NULL_ID;
            _pair.second->Handle->mID = SCHEDULE_NULL_ID;
        }
        for (auto& _pair : mSyncTaskByFramePool)
        {
            _pair.second->ID = SCHEDULE_NULL_ID;
            _pair.second->Handle->mID = SCHEDULE_NULL_ID;
        }
        mSortedSyncTaskByTicks.clear();
        mSortedSyncTaskByFrames.clear();
        mSyncTaskByTickPool.clear();
        mSyncTaskByFramePool.clear();

    }
    SharedPtr<ScheduleHandle> Scheduler::Schedule(f32 delay, f32 tickCycle, i32 repeat, i32 priority, const SyncTaskFunction& task)
    {
        u64 ID = ReceiveScheduleID();
        auto SyncTask = CreateSharedPtr<SyncTaskByTick>();
        SyncTask->ID    = ID;
        SyncTask->Delay = delay;
        SyncTask->TickCycle = tickCycle;
        SyncTask->Repeat = repeat;
        SyncTask->Priority = priority;
        SyncTask->Function = task;
        mSyncTaskByTickPool.emplace(ID, SyncTask);
        mSortedSyncTaskByTicks[SyncTask->Priority].push_back(SyncTask);
        
        auto handle = CreateSharedPtr<ScheduleHandle>();
        handle->mID = ID;
        handle->mState = EScheduleState::Wait;
        handle->mType  = EScheduleType::SyncByTick;
        SyncTask->Handle = handle;
        return handle;
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleOnce(f32 delay, i32 priority, const SyncTaskFunction& task)
    {
        return Schedule(delay, 0.0f, 1, priority, task);
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleByFrame(i32 delayFrame, i32 frameCycle, i32 repeat, i32 priority, const SyncTaskFunction& task)
    {
        u64 ID = ReceiveScheduleID();
        auto SyncTask = CreateSharedPtr<SyncTaskByFrame>();
        SyncTask->ID = ID;
        SyncTask->Delay = delayFrame;
        SyncTask->FrameCycle = frameCycle;
        SyncTask->Repeat = repeat;
        SyncTask->Priority = priority;
        SyncTask->Function = task;
        mSyncTaskByFramePool.emplace(ID, SyncTask);
        mSortedSyncTaskByFrames[ID].push_back(SyncTask);

        auto handle = CreateSharedPtr<ScheduleHandle>();
        handle->mID = ID;
        handle->mState = EScheduleState::Wait;
        handle->mType = EScheduleType::SyncByFrame;
        SyncTask->Handle = handle;
        return handle;
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleOnceByFrame(i32 delayFrame, i32 priority, const SyncTaskFunction& task)
    {
        return ScheduleByFrame(delayFrame, 0, 1, priority, task);
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleAsync(const AsyncTaskFunction& task)
    {
        u64 ID = ReceiveScheduleID();


        auto asyncTask = CreateSharedPtr<AsyncTask>();
        auto handle    = CreateSharedPtr<ScheduleHandle>();
        handle->mID = ID;
        handle->mState = EScheduleState::Wait;
        handle->mType  = EScheduleType::Async;
        asyncTask->Handle = handle;
        asyncTask->Function = task;
        mAsyncTaskPool.emplace(ID, asyncTask);

        asyncTask->Thread = std::thread([&](SharedPtr<AsyncTask> task)
        {
            task->SetState(EScheduleState::Run);
            if (task->Function)
            {
                task->Function();
            }
            task->SetState(EScheduleState::Compelete);
        }, asyncTask);

        return handle;
    }
    void Scheduler::Update()
    {
        for (auto& _pair : mSortedSyncTaskByTicks)
        {
            auto& taskList = _pair.second;
            Queue<u64> removeIndexQueue;
            u64 cnt = _pair.second.size();

            for (u64 i = 0; i < cnt; ++i)
            {
                auto task = taskList[i].lock();
                if (task)
                {
                    Update(task);
                }
                else
                {
                    removeIndexQueue.push(i);
                }
            }
        }

        for (auto _pair : mSortedSyncTaskByFrames)
        {
            auto& taskList = _pair.second;
            Queue<u64> removeIndexQueue;
            u64 cnt = _pair.second.size();

            for (u64 i = 0; i < cnt; ++i)
            {
                auto task = taskList[i].lock();
                if (task)
                {
                    Update(task);
                }
                else
                {
                    removeIndexQueue.push(i);
                }
            }
        }
    }
    void Scheduler::Update(SharedPtr<SyncTaskByTick> task)
    {
        if (task->Handle->GetState() == EScheduleState::Compelete)
        {
            task->Handle->Reset();
            return;
        }

        f32 tick = mScheduleTimer->GetTick();
        task->Tick += tick;

        if (task->Handle->GetState() == EScheduleState::Wait)
        {
            if (task->Delay <= task->Tick)
            {
                task->Handle->mState = EScheduleState::Run;
                task->Tick = 0.0f;
            }
        }


        if (task->Handle->GetState() == EScheduleState::Run)
        {
            if (task->TickCycle <= task->Tick)
            {
                task->Tick = 0.0f;
                task->CallCount += 1;

                if (task->Function)
                {
                    auto result = task->Function();
                    ResultProcess(task, result);
                }
       

                if (task->Repeat != -1 && task->CallCount >= task->Repeat)
                {
                    task->Handle->mState = EScheduleState::Compelete;
                }
            }
        }
    }
    void Scheduler::Update(SharedPtr<SyncTaskByFrame> task)
    {
        if (task->Handle->GetState() == EScheduleState::Compelete)
        {
            task->Handle->Reset();
            return;
        }
        task->Frame++;

        if (task->Handle->GetState() == EScheduleState::Wait)
        {
            if (task->Delay <= task->Frame)
            {
                task->Handle->mState = EScheduleState::Run;
                task->Frame = 0;
            }
        }

        if (task->Handle->GetState() == EScheduleState::Run)
        {
            if (task->FrameCycle <= task->Frame)
            {
                task->Frame = 0;
                task->CallCount += 1;

                if (task->Function)
                {
                    auto result = task->Function();
                    ResultProcess(task, result);
                }

                if (task->Repeat != -1 && task->CallCount >= task->Repeat)
                {
                    task->Handle->mState = EScheduleState::Compelete;
                }
            }
        }

    }
    void Scheduler::ResultProcess(SharedPtr<SyncTaskByTick> task, EScheduleResult result)
    {
        switch (result)
        {
        case EScheduleResult::Continue:
            break;
        case EScheduleResult::Break:
            task->Handle->mState = EScheduleState::Compelete;
            break;
        }
    }
    void Scheduler::ResultProcess(SharedPtr<SyncTaskByFrame> task, EScheduleResult result)
    {
        switch (result)
        {
        case EScheduleResult::Continue:
            break;
        case EScheduleResult::Break:
            task->Handle->mState = EScheduleState::Compelete;
            break;
        }
    }
    void Scheduler::RemoveSchedule(const ScheduleHandle& handle)
    {
        switch (handle.mType)
        {
        case EScheduleType::SyncByTick:
            mSyncTaskByTickPool.erase(handle.mID);
            break;
        case EScheduleType::SyncByFrame:
            mSyncTaskByFramePool.erase(handle.mID);
            break;
        case EScheduleType::Async:
            mAsyncTaskPool.erase(handle.mID);
            break;
        }
    }
    u64 Scheduler::ReceiveScheduleID()
    {
        if (mIDQueue.empty())
        {
            return mIDOffset++;
        }
        else
        {
            u64 ID = mIDQueue.front(); mIDQueue.pop();
            return ID;
        }
    }

}