using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BulletMovement : MonoBehaviour
{
    private float BulletSpeed;
    private float LifeTime;
    [HideInInspector] public bool isAlive;

    private void Awake()
    {
        isAlive = true;
        BulletSpeed = 20.0f;
        LifeTime = 0.0f;
    }

    private void FixedUpdate()
    {
        if (isAlive)
            LifeTime += Time.deltaTime;

        this.transform.Translate(Vector3.forward * BulletSpeed * Time.fixedDeltaTime);

        if (isAlive && LifeTime > 3.0f)
        {
            LifeTime = 0.0f;
            isAlive = false;
            this.gameObject.SetActive(false);
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Stage01Enemy")
        {
            other.GetComponent<enemyMovement01>().Attacked(10);

            this.gameObject.SetActive(false);
        }

        if (other.tag == "Stage02Enemy")
        {
            other.GetComponent<enemyMovement02>().Attacked(10);

            this.gameObject.SetActive(false);
        }

        if (other.tag == "Stage03Enemy")
        {
            other.GetComponent<enemyMovement03>().Attacked(10);

            this.gameObject.SetActive(false);
        }

        if (other.tag == "Stage03Enemy_ld")
        {
            other.GetComponent<enemyMovement03_ld>().Attacked(10);

            this.gameObject.SetActive(false);
        }
    }
}
