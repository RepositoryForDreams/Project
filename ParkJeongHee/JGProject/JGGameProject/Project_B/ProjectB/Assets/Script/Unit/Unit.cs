using System.Collections;
using System.Collections.Generic;
using UnityEngine;





// Folder/애니메이션 이름_Index
// 스프라이트 콜렉션에는 [Key1 :  애니메이션 스프라이트 이름, Key2 : 폴더 , Key3, Sprite
// -> Anim_Idle_0 -> Hero/Test -> Sprite 획득
// -> 애니메이션 스프라이트 변경
public class UnitStock
{
    public UnitStatus Status { get; set; }
    // Folder
    public string Anim_Idle { get; set; }
    // 
    public string Anim_Walk { get; set; }
    
}

public class UnitStatus
{
    public float HP { get; set; }
    public float MP { get; set; }
    public float Atk { get; set; }
    public float Speed { get; set; }
}

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
    UnitStatus   mUnitStatus = new UnitStatus();

    private static readonly string EMPTY_PREFAB_PATH = "";




    public string UnitName
    {
        get { return _unitName; }
    }

    public UnitStatus UnitStatus
    {
        get { return mUnitStatus; }
        set { mUnitStatus = value; }
    }

    public float Speed
    {
        get { return mUnitStatus.Speed; }
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

        mUnitStatus.HP = _hp;
        mUnitStatus.MP = _mp;
        mUnitStatus.Atk = _atk;
        mUnitStatus.Speed = _speed;
    }
    public void OnDestroy()
    {
        UnitManager.Instance.UnRegisterUnit(this);
    }



    static public T Create<T>(UnitStock stock) where T : Unit
    {
        var obj = Resources.Load<GameObject>(EMPTY_PREFAB_PATH);
        var go = Instantiate<GameObject>(obj, UnitManager.Instance.transform);
        var unit     = go.AddComponent<T>();
        var spriteRenderer = go.AddComponent<SpriteRenderer>();
        var animator = go.AddComponent<Animator>();
 
        

        // 스테이터스 등록
        unit.UnitStatus = stock.Status;

        // 애니메이션 등록
        // 애니메이션 클립 생성
        //RuntimeAnimatorController runtimeAnimatorController = new RuntimeAnimatorController();
   
        //stock.Anim_Idle;
        // 
        



        return unit;
    }


};