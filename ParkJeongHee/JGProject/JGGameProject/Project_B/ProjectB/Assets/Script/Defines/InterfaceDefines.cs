using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public interface IControllable
{

    void Controll();
    Transform GetTransform();
}

public interface IUnitCommand
{
    public void Execute(Unit unit);
}
