using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player_SpearAttack : MonoBehaviour
{
    [SerializeField] Transform playerTransform;

    private Collider hitCollider;

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Zombie")
        {
            hitCollider = other;

            this.GetComponent<BoxCollider>().enabled = false;
        }
    }

    public Collider GetHitCollider()
    {
        return hitCollider;
    }

    public void InitHitCollider()
    {
        hitCollider = null;
    }
}