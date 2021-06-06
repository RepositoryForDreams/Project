using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public enum EUnitAnimState
{
    None,
    Idle,
    Walk,
}
public enum EUnitDirection
{
    None,
    Back,
    Front,
    Left,
    Right,
}

public enum EUnitCategory
{
    None     = 0x000,
    Playable = 0x001,
    Enemy    = 0x002,
}
