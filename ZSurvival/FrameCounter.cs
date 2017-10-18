using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class FrameCounter : MonoBehaviour
{
    [SerializeField] Text FrameText;

    float deltaTime = 0.0f;
    float msec = 0.0f;
    float fps = 0.0f;
    string text;

    void Update()
    {
        deltaTime += (Time.deltaTime - deltaTime) * 0.1f;
    }

    private void OnGUI()
    {
        msec = deltaTime * 1000.0f;
        fps = 1.0f / deltaTime;
        text = string.Format("FPS: {1:0.} ({0:0.0} ms)", msec, fps);
        FrameText.text = text;
    }
}