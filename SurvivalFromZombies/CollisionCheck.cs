using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CollisionCheck : MonoBehaviour
{
    [HideInInspector] public bool isCollision;

    private void Awake()
    {
        isCollision = false;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (this.gameObject.name == "StartTrigger")
        {
            if (other.name == "PlayerCollider")
            {
                isCollision = true;
            }
        }

        if (this.gameObject.name == "CreateState02Enemy")
        {
            if (other.name == "PlayerCollider")
            {
                isCollision = true;
            }
        }

        if (this.gameObject.name == "CreateState03Enemy")
        {
            if (other.name == "PlayerCollider")
            {
                isCollision = true;
            }
        }

        if (this.gameObject.name == "CreateState04")
        {
            if (other.name == "PlayerCollider")
            {
                isCollision = true;
            }
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (this.gameObject.name == "Stage01Clear")
        {
            if (collision.gameObject.name == "PlayerCollider")
            {
                isCollision = true;
            }
        }

        if (this.gameObject.name == "Stage02Clear")
        {
            if (collision.gameObject.name == "PlayerCollider")
            {
                isCollision = true;
            }
        }

        if (this.gameObject.name == "Stage03Clear")
        {
            if (collision.gameObject.name == "PlayerCollider")
            {
                isCollision = true;
            }
        }
    }
}
