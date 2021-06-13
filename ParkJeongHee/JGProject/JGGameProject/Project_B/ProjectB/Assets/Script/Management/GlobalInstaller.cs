using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GlobalInstaller : MonoBehaviour
{
    int frame = 0;
    // Start is called before the first frame update
    private void Awake()
    {
        gameObject.AddComponent<TaskScheduler>();
        gameObject.AddComponent<GameManager>();
        gameObject.AddComponent<UnitManager>();
        gameObject.AddComponent<CameraManager>();
    }
    void Start()
    {
        TaskScheduler.Instance.SceduleCroutine(Test);
    }

    IEnumerator Test()
    {
        Debug.Log("Frame : " + frame.ToString());
        frame++;
        yield return new WaitForSeconds(1.0f);
        Debug.Log("Frame : " + frame.ToString());
        frame++;
        yield return new WaitForSeconds(1.0f);
        Debug.Log("Frame : " + frame.ToString());
        frame++;
        yield return new WaitForSeconds(1.0f);
        Debug.Log("Frame : " + frame.ToString());
        frame++;
        yield return new WaitForSeconds(1.0f);
        Debug.Log("Frame : " + frame.ToString());
        frame++;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
