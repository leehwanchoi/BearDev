using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ShootingBullet : MonoBehaviour
{
    enemyManager em;
    MoveShooting player;

    public Transform ShotPoint;
    public float bulletSpeed = 5.0f;
    public float bulletRange = 3.0f;
    public int att = 5;

	void Start ()
    {
        em = GameObject.Find("Enemies").GetComponent<enemyManager>();
        player = GameObject.Find("Judy").GetComponent<MoveShooting>();

        ShotPoint = GameObject.Find("ShotPoint").GetComponent<Transform>();
    }
	
	void Update ()
    {
        ShotPoint = GameObject.Find("ShotPoint").GetComponent<Transform>();

        float speed = bulletSpeed * Time.deltaTime;

        transform.Translate(Vector3.up * speed);

        if (transform.position.z > ShotPoint.position.z + bulletRange)
        {
            Destroy(gameObject);

            if (player.bulletCur < player.bulletMax)
                player.bulletCur++;
        }
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.name == "BlueJelly")
        {
            em.blueDamage(att);
        }

        if (other.name == "GreenJelly")
        {
            em.greenDamage(att);
        }

        if (other.name == "Stone_Frog_Red")
        {
            em.redDamage(att);
        }

        if (other.name == "zipper")
        {
            em.zipDamage(att);
        }

        if (other.name == "PurpleJelly")
        {
            em.purpleDamage(att);
        }

        if (other.name == "Cactus")
        {
            em.cactusDamage(att);
        }
    }
}
