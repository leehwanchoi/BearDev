using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Resize : MonoBehaviour
{
    private void Awake()
    {
        Screen.SetResolution(1280, 800, true);
    }
}