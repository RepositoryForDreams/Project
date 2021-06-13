using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;



public enum ETaskPriority
{
    UnitManager = 0,
}

public enum ETaskResult
{
    Break,
    Continue,
}


public class TaskScheduler : GlobalSingletone<TaskScheduler>
{
    static readonly int INFINITE_REPEAT = -1;


    SortedDictionary<int, HashSet<JGTask>> mSyncTaskPool = new SortedDictionary<int, HashSet<JGTask>>();
    Queue<JGTask> mRemoveTaskQueue = new Queue<JGTask>();
    Queue<JGTask> mReservedSyncTaskQueue = new Queue<JGTask>();
    bool mIsRun = false;



    enum ETaskType
    {
        ByFrame,
        ByTick,
        Corutine,
    }
    private abstract class JGTask
    {
        public bool IsStart = false;
        public int Priority = 0;
        public int RepeatCount = 0;
        public int Repeat = 0;

        public JGTask(int priority, int repeat)
        {
            Priority = priority;
            Repeat = repeat;
        }
        abstract public ETaskType GetTaskType();
    }

    private class SyncTaskByFrame : JGTask
    {
        public Func<ETaskResult> Action;
        public int DelayFrame = 0;
        public int FrameCount = 0;
        public int FrameCycle = 0;

        public SyncTaskByFrame(Func<ETaskResult> func, int delayFrame, int frameCycle, int priority, int repeat) : base(priority, repeat)
        {
            Action = func;
            DelayFrame = delayFrame;
            FrameCycle = frameCycle; 
        }
        public override ETaskType GetTaskType()
        {
            return ETaskType.ByFrame;
        }
    }

    private class SyncTaskByTick : JGTask
    {
        public Func<ETaskResult> Action;
        public float DelayTick = 0.0f;
        public float Tick = 0.0f;
        public float TickCycle = 0.0f;

        public SyncTaskByTick(Func<ETaskResult> func, float delayTick, float tickCycle, int priority, int repeat) : base(priority, repeat)
        {
            Action = func;
            DelayTick = delayTick;
            TickCycle = tickCycle;
        }
        public override ETaskType GetTaskType()
        {
            return ETaskType.ByTick;
        }
    }

    private class CroutineTask :  JGTask
    {
        public bool IsCompelete = false;

        public Func<IEnumerator> Croutine;

        public CroutineTask(Func<IEnumerator> co, int priority) : base(priority, 0)
        {
            Croutine = co;
        }
        public override ETaskType GetTaskType()
        {
            return ETaskType.Corutine;
        }
    }

    public void ScheduleByTick(Func<ETaskResult> action, float tickCycle = 0.0f, float delay = 0.0f, int repeat = -1, int priority = 0)
    {
        AddTask(new SyncTaskByTick(action, delay, tickCycle, priority, repeat));
    }
    public void ScheduleOnceByTick(Func<ETaskResult> action, float delay = 0.0f, int priority = 0)
    {
        AddTask(new SyncTaskByTick(action, delay, 0.0f, priority, 1));
    }
    public void ScheduleByFrame(Func<ETaskResult> action, int frameCycle = 0, int delay = 0, int repeat = -1, int priority = 0)
    {
        AddTask(new SyncTaskByFrame(action, delay, frameCycle, priority, repeat));
    }
    public void ScheduleOnceByFrame(Func<ETaskResult> action, int delay = 0, int priority = 0)
    {
        AddTask(new SyncTaskByFrame(action, delay, 0, priority, 1));
    }
    public void SceduleCroutine(Func<IEnumerator> co, int priority = 0)
    {
        AddTask(new CroutineTask(co, priority));
    }


    private void AddTask(JGTask task)
    {
        if (mIsRun)
        {
            mReservedSyncTaskQueue.Enqueue(task);
        }
        else
        {
            if (mSyncTaskPool.ContainsKey(task.Priority) == false)
            {
                mSyncTaskPool[task.Priority] = new HashSet<JGTask>();
            }
            mSyncTaskPool[task.Priority].Add(task);
        }
    }

    private void Update()
    {
        mIsRun = true;
    
        foreach(var _pair in mSyncTaskPool)
        {
            foreach(var task in _pair.Value)
            {
                ETaskResult result = ETaskResult.Continue;
                switch (task.GetTaskType())
                {
                    case ETaskType.ByFrame:
                        result = Update(task as SyncTaskByFrame);
                        break;
                    case ETaskType.ByTick:
                        result = Update(task as SyncTaskByTick);
                        break;
                    case ETaskType.Corutine:
                        result = Update(task as CroutineTask);
                        break;
                }
                if(result == ETaskResult.Break)
                {
                    mRemoveTaskQueue.Enqueue(task);
                }
            }
        }
        mIsRun = false;


        // 삭제할거 삭제
        while (mRemoveTaskQueue.Count > 0)
        {
            var task = mRemoveTaskQueue.Dequeue();
            mSyncTaskPool[task.Priority].Remove(task);
        }
        // 예약된 테스크 추가
        while(mReservedSyncTaskQueue.Count > 0)
        {
            var task = mReservedSyncTaskQueue.Dequeue();
            if(mSyncTaskPool.ContainsKey(task.Priority) == false)
            {
                mSyncTaskPool[task.Priority] = new HashSet<JGTask>();
            }
            mSyncTaskPool[task.Priority].Add(task);
        }


    }
    private ETaskResult Update(SyncTaskByFrame task)
    {
        ETaskResult result = ETaskResult.Continue;
        if(task.IsStart == false)
        {
            if (task.FrameCount >= task.DelayFrame)
            {
                task.DelayFrame = 0;
                task.FrameCount = task.FrameCycle;
                task.IsStart = true;
            }
        }

        if(task.IsStart == true)
        {
            if (task.FrameCount >= task.FrameCycle)
            {
                task.FrameCount = 0;
                task.RepeatCount++;
                result = task.Action();
            }

            if (task.Repeat != INFINITE_REPEAT && task.RepeatCount >= task.Repeat)
            {
                result = ETaskResult.Break;
            }
        }
        task.FrameCount++;
        return result;
    }
    private ETaskResult Update(SyncTaskByTick task)
    {
        ETaskResult result = ETaskResult.Continue;

        if(task.IsStart == false)
        {
            task.DelayTick = 0.0f;
            task.Tick = task.TickCycle;
            task.IsStart = true;
        }

        if(task.IsStart == true)
        {
            if (task.Tick >= task.TickCycle)
            {
                task.Tick = 0.0f;
                task.RepeatCount++;
                result = task.Action();
            }

            if (task.Repeat != INFINITE_REPEAT && task.RepeatCount >= task.Repeat)
            {
                result = ETaskResult.Break;
            }
        }

        task.Tick += Time.deltaTime;
        return result;
    }
    private ETaskResult Update(CroutineTask task)
    {
        ETaskResult result = ETaskResult.Continue;
        if (task.IsCompelete)
        {
            result = ETaskResult.Break;
        }
        if (task.IsStart == false)
        {
            task.IsStart = true;
            StartCoroutine(StartCo(task));
        }


        return result;
    }
    private IEnumerator StartCo(CroutineTask task)
    {
        
        yield return task.Croutine();
        task.IsCompelete = true;
    }
}
