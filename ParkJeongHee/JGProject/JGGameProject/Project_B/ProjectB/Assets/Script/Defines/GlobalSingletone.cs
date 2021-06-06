using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;



public class GlobalSingletone<T>: MonoBehaviour where T : MonoBehaviour
{
    private static object mSyncObj = new object();
    private static T mInstance;

    
    public static T Instance
    {
        get
        {
            lock (mSyncObj)
            {
                if (mInstance == null)
                {
                    T[] objs = FindObjectsOfType<T>();

                    if (objs.Length > 0)
                        mInstance = objs[0];

                    if (objs.Length > 1)
                        Debug.LogError("There is more than one " + typeof(T).Name + " in the scene.");

                    if (mInstance == null)
                    {
                        string goName = typeof(T).ToString();
                        GameObject go = GameObject.Find(goName);
                        if (go == null)
                            go = new GameObject(goName);
                        mInstance = go.AddComponent<T>();
                    }
                }
            }
           
            return mInstance;
        }
    }

    public void Awake()
    {
        if (Instance == this)
        {
            DontDestroyOnLoad(gameObject);
        }
        else
        {
            Destroy(gameObject);
        }
    }
}
