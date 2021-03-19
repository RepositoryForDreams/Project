#include "pch.h"
#include "Scheduler.h"

namespace JG
{
    SharedPtr<ScheduleHandle> Scheduler::Schedule(float delay, float tick, i32 repeat, i32 priority, const SyncTask& task)
    {
        return SharedPtr<ScheduleHandle>();
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleOnce(float delay, i32 priority, const SyncTask& task)
    {
        return SharedPtr<ScheduleHandle>();
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleByFrame(i32 delayFrame, i32 perFrame, i32 repeat, i32 priority, const SyncTask& task)
    {
        return SharedPtr<ScheduleHandle>();
    }
    SharedPtr<ScheduleHandle> Scheduler::ScheduleAsync(float delay, const AsyncTask& task)
    {
        return SharedPtr<ScheduleHandle>();
    }
}