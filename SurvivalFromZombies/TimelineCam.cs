using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TimelineCam : MonoBehaviour
{
    public GameObject TargetHuman;
    public GameObject TargetCar;

    private Vector3 vPos;

    private void Awake()
    {
        vPos = Vector3.zero;
    }

    private void FixedUpdate()
    {
        if (TargetHuman.gameObject.activeSelf)
            this.transform.LookAt(TargetHuman.transform);
        else
        {
            if (TargetCar.gameObject.activeSelf)
            {
                this.transform.LookAt(TargetCar.transform);
                vPos = TargetCar.transform.position;
            }
            else
            {
                this.transform.LookAt(vPos);
            }
        }
    }
}
