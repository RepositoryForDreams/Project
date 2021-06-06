using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Hero : Unit, IControllable
{



    public override EUnitCategory Category => EUnitCategory.Playable;

    public void Controll()
    {
        Vector3 force = new Vector3();

        if (Input.GetKey(KeyCode.RightArrow))
        {
            force.x += 1.0f;
        }
        if(Input.GetKey(KeyCode.LeftArrow))
        {
            force.x -= 1.0f;
        }

        if (Input.GetKey(KeyCode.UpArrow))
        {
            force.y += 1.0f;
        }
        if (Input.GetKey(KeyCode.DownArrow))
        {
            force.y -= 1.0f;
        }
        force = Vector3.Normalize(force);
        transform.position += (force * Time.deltaTime * Speed);

        

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
                else if(force.x < 0.0f) command.RequestDirection = EUnitDirection.Left;
            }
            else if(xForce <= 0.0f && yForce > 0.0f)
            {
                if (force.y > 0.0f) command.RequestDirection = EUnitDirection.Front;
                else if (force.y < 0.0f) command.RequestDirection = EUnitDirection.Back;
            }
        }
        UnitManager.Instance.PushUnitCommand(this, command);
    }

    public Transform GetTransform()
    {
        return transform;
    }
}
