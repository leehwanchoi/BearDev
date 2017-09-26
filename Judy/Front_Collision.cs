using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Front_Collision : MonoBehaviour
{
    public bool isMove;

    void OnTriggerEnter(Collider other)
    {
        if (other.name == "Judy")
        {
            isMove = true;
        }
    }
}
