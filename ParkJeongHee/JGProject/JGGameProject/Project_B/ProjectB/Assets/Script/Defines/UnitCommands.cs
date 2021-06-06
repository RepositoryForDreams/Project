using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// ������ �ִϸ��̼�
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
                Debug.Log("���� �ʱ�ȭ");
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
                    Debug.Log(string.Format("{0}���� �ִϸ��̼� ���� {1} -> {2} �� ����", unit.UnitName,
                         unit.UnitAnimInfo.CurrentAnimState.ToString(), RequestAnimState.ToString()));
                    unit.UnitAnimInfo.CurrentAnimState = RequestAnimState;
                    unit.UnitAnimator.SetInteger(UnitAnimationDefines.AnimState, (int)RequestAnimState);



                }
                if (isDirectionState)
                {
                    Debug.Log(string.Format("{0}���� ���� ���� {1} -> {2} �� ����", unit.UnitName,
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