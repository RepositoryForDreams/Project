using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraManager : GlobalSingletone<CameraManager>
{
    [SerializeField]
    private Camera _camera = null;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    void Update()
    {
        if(_camera == null)
        {
            _camera = Camera.main;
        }
        else 
        {
            if(UnitManager.Instance.TargetControllUnit != null)
            {
                var newPos = UnitManager.Instance.TargetControllUnit.GetTransform().transform.position;
                newPos.z = _camera.transform.position.z; ;
                _camera.transform.position = newPos;
            }
        }
    }
}
