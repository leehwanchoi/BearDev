using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Advertisements;

public class UnityAd : MonoBehaviour
{
    private float WorldTime;

    private void Awake()
    {
        WorldTime = 0.0f;
    }

    private void Update()
    {
        WorldTime += Time.deltaTime;

        if (WorldTime > 300.0f)
        {
            ShowAd();
            WorldTime = 0.0f;
        }
    }

    public void ShowAd()
    {
        if (Advertisement.IsReady())
            Advertisement.Show();
    }
}