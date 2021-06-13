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
        UnitMoveCommand moveCommand = new UnitMoveCommand();
        moveCommand.Force = force;
        UnitManager.Instance.PushUnitCommand(this, moveCommand);
    }

    public Transform GetTransform()
    {
        return transform;
    }
}
