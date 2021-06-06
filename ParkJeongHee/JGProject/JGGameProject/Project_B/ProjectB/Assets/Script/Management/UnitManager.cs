using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class UnitManager : GlobalSingletone<UnitManager>
{
    Dictionary<EUnitCategory, HashSet<Unit>> mUnitPool = new Dictionary<EUnitCategory, HashSet<Unit>>();




    Queue<(Unit unit, IUnitCommand command)> mCommandQueue = new Queue<(Unit unit, IUnitCommand command)>();
    List<(Unit unit, IUnitCommand command)> mPendingCommandList = new List<(Unit unit, IUnitCommand command)>(); 
    bool mIsCommandRun = false;
    public void RegisterUnit(Unit unit)
    {
        if(mUnitPool.ContainsKey(unit.Category) == false)
        {
            mUnitPool.Add(unit.Category, new HashSet<Unit>());
        }
        Debug.Log(string.Format("¿Ø¥÷ µÓ∑œ : {0}", unit.UnitName));
        mUnitPool[unit.Category].Add(unit);
    }

    public void UnRegisterUnit(Unit unit)
    {
  
        if (mUnitPool.ContainsKey(unit.Category) == false)
        {
            return;
        }
        Debug.Log(string.Format("¿Ø¥÷ µÓ∑œ «ÿ¡¶ : {0}", unit.UnitName));
        mUnitPool[unit.Category].Remove(unit);
    }

    public void PushUnitCommand(Unit unit, IUnitCommand command)
    {
        if(mIsCommandRun == true)
        {
            mPendingCommandList.Add((unit, command));
        }
        else
        {
            mCommandQueue.Enqueue((unit, command));
        }
      
    }


    public List<Unit> GetUnitList(EUnitCategory category)
    {
        if (mUnitPool.ContainsKey(category) == false)
        {
            return null;
        }
        return mUnitPool[category].ToList();
    }
    public HashSet<Unit> GetUnitSet(EUnitCategory category)
    {
        if (mUnitPool.ContainsKey(category) == false)
        {
            return null; 
        }
        return mUnitPool[category];
    }

    private void Update()
    {
        mIsCommandRun = true;
        while (mCommandQueue.Count > 0)
        {
            var item = mCommandQueue.Dequeue();
            if(item.unit != null && item.command != null)
            {
                item.command.Execute(item.unit);
            }
        }
        mIsCommandRun = false;


        if (mPendingCommandList.Count > 0)
        {
            foreach(var item in mPendingCommandList)
            {
                mCommandQueue.Enqueue(item);
            }
            mPendingCommandList.Clear();
        }

      
    }

}
