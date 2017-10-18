using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Zombie_HandAttack : MonoBehaviour
{
    private Collider hitCollider;

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Player")
        {
            hitCollider = other;

            this.GetComponent<BoxCollider>().enabled = false;
        }
    }

    public Collider GetHitCollider()
    {
        return hitCollider;
    }
}