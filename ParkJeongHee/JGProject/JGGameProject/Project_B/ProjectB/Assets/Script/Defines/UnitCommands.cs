using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 설정한 애니메이션
public class UnitAnimCommand : IUnitCommand
{
    public EUnitAnimState RequestAnimState = EUnitAnimState.None;
    public EUnitDirection RequestDirection = EUnitDirection.None;
    public void Execute(Unit unit)
    {
        if (unit == null || unit.UnitAnimator == null)
        {
            return;
        }
        var animInfo = unit.UnitAnimInfo;

        bool isDiffAnimState = (RequestAnimState != EUnitAnimState.None) ?
            unit.UnitAnimInfo.CurrentAnimState != RequestAnimState : false;
        bool isDirectionState = (RequestDirection != EUnitDirection.None) ?
            unit.UnitAnimInfo.CurrentDirection != RequestDirection : false;
        if (isDiffAnimState || isDirectionState)
        {

            if(unit.UnitAnimInfo.IsEdge == false)
            {
                Debug.Log("상태 초기화");
                unit.UnitAnimator.SetTrigger(UnitAnimationDefines.StateRevertTrigger);
                unit.UnitAnimInfo.IsEdge = true;
            }
            UnitManager.Instance.PushUnitCommand(unit, new UnitAnimApplayCommand(RequestAnimState, RequestDirection));
        }
    }
    private class UnitAnimApplayCommand : IUnitCommand
    {
        public EUnitAnimState RequestAnimState = EUnitAnimState.None;
        public EUnitDirection RequestDirection = EUnitDirection.None;

        public UnitAnimApplayCommand(EUnitAnimState animState, EUnitDirection dir)
        {
            RequestAnimState = animState;
            RequestDirection = dir;
        }
        public void Execute(Unit unit)
        {
            bool isDiffAnimState = (RequestAnimState != EUnitAnimState.None) ?
                unit.UnitAnimInfo.CurrentAnimState != RequestAnimState : false;
            bool isDirectionState = (RequestDirection != EUnitDirection.None) ?
                unit.UnitAnimInfo.CurrentDirection != RequestDirection : false;
            if (isDiffAnimState || isDirectionState)
            {
                if (isDiffAnimState)
                {
                    Debug.Log(string.Format("{0}유닛 애니메이션 상태 {1} -> {2} 로 변경", unit.UnitName,
                         unit.UnitAnimInfo.CurrentAnimState.ToString(), RequestAnimState.ToString()));
                    unit.UnitAnimInfo.CurrentAnimState = RequestAnimState;
                    unit.UnitAnimator.SetInteger(UnitAnimationDefines.AnimState, (int)RequestAnimState);



                }
                if (isDirectionState)
                {
                    Debug.Log(string.Format("{0}유닛 방향 상태 {1} -> {2} 로 변경", unit.UnitName,
                    unit.UnitAnimInfo.CurrentDirection.ToString(), RequestDirection.ToString()));
                    unit.UnitAnimInfo.CurrentDirection = RequestDirection;
                    unit.UnitAnimator.SetInteger(UnitAnimationDefines.Direction, (int)RequestDirection);

                    

                    var localScale = unit.transform.localScale;
                    if (unit.UnitAnimInfo.CurrentDirection == EUnitDirection.Left)
                    {
                        if (localScale.x <= 0.0f)
                        {
                            localScale.x *= -1;
                            unit.transform.localScale = localScale;
                        }
                    }
                    if (unit.UnitAnimInfo.CurrentDirection == EUnitDirection.Right)
                    {
                        if (localScale.x >= 0.0f)
                        {
                            localScale.x *= -1;
                            unit.transform.localScale = localScale;
                        }
                    }


                }
                unit.UnitAnimator.SetTrigger(UnitAnimationDefines.StateApplyTrigger);
                unit.UnitAnimInfo.IsEdge = false;
            }
        }
    }
}