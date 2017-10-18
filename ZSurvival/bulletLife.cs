using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class bulletLife : NetworkBehaviour
{
    private float bulletSpeed;
    private float lifeTime;
    private bool isDie;

    private int bulletDamage;

    private void Start()
    {
        bulletSpeed = 10.0f;
        bulletDamage = 1;
    }

    private void FixedUpdate()
    {
        this.transform.Translate(Vector3.forward * bulletSpeed * Time.deltaTime);

        if (!isDie)
            lifeTime += Time.deltaTime;

        if (lifeTime >= 2.0f)
        {
            isDie = true;
        }

        if (isDie)
        {
            lifeTime = 0.0f;
            isDie = false;

            //this.GetComponent<SphereCollider>().enabled = false;
            //this.GetComponent<MeshRenderer>().enabled = false;
            //this.GetComponent<bulletLife>().enabled = false;

            Destroy(this.gameObject);
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Zombie")
        {
            other.GetComponent<Zombie_Health>().DeductHealth(bulletDamage);

            lifeTime = 0.0f;
            isDie = false;

            //this.GetComponent<SphereCollider>().enabled = false;
            //this.GetComponent<MeshRenderer>().enabled = false;
            //this.GetComponent<bulletLife>().enabled = false;

            Destroy(this.gameObject);
        }
    }
}