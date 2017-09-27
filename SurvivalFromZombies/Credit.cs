using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Credit : MonoBehaviour
{
    private void FixedUpdate()
    {
        this.transform.Translate(Vector3.up * 2.0f);

        if (this.transform.localPosition.y > 520)
        {
#if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
#else
Application.Quit();
#endif
        }
    }
}