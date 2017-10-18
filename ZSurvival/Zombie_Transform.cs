using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Zombie_Transform : MonoBehaviour
{
    private Vector3 zombieRealPos;

    private void Start()
    {
        zombieRealPos = this.GetComponentInParent<Zombie_ID>().transform.position;
    }

    private void Update()
    {
        this.transform.localPosition = new Vector3(this.transform.localPosition.x, -0.5f, this.transform.localPosition.z);
    }
}