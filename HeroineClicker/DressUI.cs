using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DressUI : MonoBehaviour
{
    private float HideTime;

    private void Awake()
    {
        HideTime = 0.0f;
    }

    private void Update()
    {
        HideTime += Time.deltaTime;

        if (HideTime > 2.0f)
        {
            HideTime = 0.0f;

            this.gameObject.SetActive(false);
        }
    }
}