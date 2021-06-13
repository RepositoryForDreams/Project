using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// ������ �ִϸ��̼�


public class UnitAttackCommand : IUnitCommand
{
    // � �����ΰ� ?
    public void Execute(Unit unit)
    {
        throw new NotImplementedException();
    }
}



public class UnitMoveCommand : IUnitCommand
{
    public Vector2 Force = new Vector2();
    public void Execute(Unit unit)
    {
        Vector3 force = new Vector3(Force.x, Force.y, 0.0f);
        force = Vector3.Normalize(force);
        unit.transform.position += (force * Time.deltaTime * unit.Speed);

        UnitAnimCommand command = new UnitAnimCommand();
        if (force.sqrMagnitude <= 0.00001f)
        {
            command.RequestAnimState = EUnitAnimState.Idle;
        }
        else
        {
            command.RequestAnimState = EUnitAnimState.Walk;
        }
        float xForce = Math.Abs(force.x);
        float yForce = Math.Abs(force.y);
        if (force.x > 0.0f && force.y > 0.0f)
        {
            command.RequestDirection = (xForce >= yForce) ? EUnitDirection.Right : EUnitDirection.Front;
        }
        else if (force.x > 0.0f && force.y < 0.0f)
        {
            command.RequestDirection = (xForce >= yForce) ? EUnitDirection.Right : EUnitDirection.Back;
        }
        else if (force.x < 0.0f && force.y > 0.0f)
        {
            command.RequestDirection = (xForce >= yForce) ? EUnitDirection.Left : EUnitDirection.Front;
        }
        else if (force.x < 0.0f && force.y > 0.0f)
        {
            command.RequestDirection = (xForce >= yForce) ? EUnitDirection.Left : EUnitDirection.Back;
        }
        else
        {
            if (xForce > 0.0f && yForce <= 0.0f)
            {
                if (force.x > 0.0f) command.RequestDirection = EUnitDirection.Right;
                else if (force.x < 0.0f) command.RequestDirection = EUnitDirection.Left;
            }
            else if (xForce <= 0.0f && yForce > 0.0f)
            {
                if (force.y > 0.0f) command.RequestDirection = EUnitDirection.Front;
                else if (force.y < 0.0f) command.RequestDirection = EUnitDirection.Back;
            }
        }
        UnitManager.Instance.PushUnitCommand(unit, command);
    }
}





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