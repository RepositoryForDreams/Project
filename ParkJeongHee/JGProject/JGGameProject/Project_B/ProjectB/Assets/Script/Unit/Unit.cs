using System.Collections;
using System.Collections.Generic;
using UnityEngine;










public class UnitAnimInfo
{
    public bool IsEdge { get; set; }
    public EUnitAnimState CurrentAnimState { get; set; }
    public EUnitDirection CurrentDirection { get; set; }
}

public class Unit : MonoBehaviour
{
    [SerializeField] private string _unitName = "Unit";
    [SerializeField] private float _hp  = 100;
    [SerializeField] private float _mp  = 10;
    [SerializeField] private float _atk = 5;
    [SerializeField] private float _speed = 10;

    Animator mAnimator = null;
    UnitAnimInfo mAnimInfo = new UnitAnimInfo();

  
    public string UnitName
    {
        get { return _unitName; }
    }

    public float Speed
    {
        get { return _speed; }
    }
    public UnitAnimInfo UnitAnimInfo { get { return mAnimInfo; } }
    public Animator UnitAnimator { get { return mAnimator; } }
    virtual public EUnitCategory Category => EUnitCategory.None;

    public void Awake()
    {
        UnitManager.Instance.RegisterUnit(this);
        mAnimInfo.CurrentAnimState = EUnitAnimState.Idle;
        mAnimInfo.CurrentDirection = EUnitDirection.Back;
        mAnimInfo.IsEdge = true;
        mAnimator = GetComponent<Animator>();
    }
    public void OnDestroy()
    {
        UnitManager.Instance.UnRegisterUnit(this);
    }




};