using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class eBulletMovement : MonoBehaviour
{
    private AudioSource audioSource;
    public AudioClip audioClip;

    private float BulletSpeed;
    private float LifeTime;
    [HideInInspector] public bool isAlive;

    private void Awake()
    {
        audioSource = GameObject.Find("BulletMoveSound").GetComponent<AudioSource>();

        isAlive = false;
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

        if (GameObject.Find("PlayerCollider"))
        {
            Vector3 Target = GameObject.Find("PlayerCollider").transform.position;

            if ((Target - this.transform.position).magnitude < 5.0f)
            {
                if (!audioSource.isPlaying)
                {
                    audioSource.clip = audioClip;
                    audioSource.Play();
                }
            }
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        //if (other.name == "PlayerCollider" || other.name == "HitPoint")
        //{
        //    other.GetComponentInParent<PlayerMovement>().Attacked(10);

        //    this.gameObject.SetActive(false);
        //}
    }
}
