using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : GlobalSingletone<GameManager>
{
    [SerializeField]
    private IControllable _targetControllUnit = null;


    public IControllable TargetControllUnit { 
        get { return _targetControllUnit; }
        set { _targetControllUnit = value; }
    }

    private void Update()
    {
        if(_targetControllUnit == null)
        {
            var set = UnitManager.Instance.GetUnitSet(EUnitCategory.Playable);
            foreach(var unit in set)
            {
                if(unit is IControllable)
                {
                    _targetControllUnit = unit as IControllable;
                    break;
                }
            }
        }
        if (TargetControllUnit != null)
        {
            TargetControllUnit.Controll();
        }
    }
}
