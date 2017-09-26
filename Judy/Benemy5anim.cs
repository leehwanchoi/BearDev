using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Benemy5anim : MonoBehaviour
{
    float speed;
    public int damage = 10;

    AdvenManager aM;

    Transform playerCoor;
    AMove player;
    Animator anim;

    public AudioClip hit_Sound;
    public AudioClip Damage_Sound;
    public AudioClip die_Sound;

    public GameObject FX;

    public float maxHP = 100;
    public float curHP = 100;

    bool isDie;
    bool isDamage;
    float time;
    float count;
    float dieCount;

    void Start()
    {
        aM = GameObject.Find("GameManager").GetComponent<AdvenManager>();

        player = GameObject.Find("Judy").GetComponent<AMove>();
        playerCoor = player.transform;
        anim = this.GetComponent<Animator>();
    }

    void Update()
    {
        DieCheck();

        if (Vector3.Distance(playerCoor.position, transform.position) <= 30f && !player.isDie && !isDie && !isDamage)
            distanceCheck();

        if (isDamage)
        {
            anim.SetBool("Run", false);

            count += Time.deltaTime;

            if (count >= 2.0f)
                isDamage = false;
        }

        if (isDie)
        {
            dieCount += Time.deltaTime;

            if (dieCount >= 0.5f)
            {
                GetComponent<AudioSource>().clip = die_Sound;
                if (!GetComponent<AudioSource>().isPlaying)
                    GetComponent<AudioSource>().Play();
            }

            if (dieCount >= 1.0f)
            {
                FX.SetActive(false);

                Destroy(gameObject);

                aM.enemyDieCount++;
                dieCount = 0;
            }
        }
    }

    void MoveUpdate()
    {
        transform.rotation = Quaternion.LookRotation(new Vector3(playerCoor.position.x, this.transform.position.y, playerCoor.position.z) - transform.position);
        transform.Translate(Vector3.forward * speed * Time.deltaTime);
        anim.SetBool("Run", true);
    }

    void distanceCheck()
    {
        time += Time.deltaTime;

        if (Vector3.Distance(playerCoor.position, transform.position) >= 5f)
        {
            speed = 3f;
            MoveUpdate();
        }
        else
        {
            if (time >= 4f)
            {
                speed = 0;
                Attack();

                time = 0;
            }
        }
    }

    void Attack()
    {
        GetComponent<AudioSource>().clip = hit_Sound;
        if (!GetComponent<AudioSource>().isPlaying)
            GetComponent<AudioSource>().Play();

        transform.rotation = Quaternion.LookRotation(new Vector3(playerCoor.position.x, this.transform.position.y, playerCoor.position.z) - transform.position);
        anim.SetBool("Run", false);
        anim.SetTrigger("Attack");
        player.cur_hp -= damage;
    }

    void DieCheck()
    {
        if (curHP <= 0)
        {
            FX.SetActive(true);
            FX.transform.position = transform.position;

            isDie = true;
            curHP = 0;
            anim.SetBool("Die", true);
        }
    }

    public void Damage(int hp)
    {
        isDamage = true;

        GetComponent<AudioSource>().clip = Damage_Sound;
        if (!GetComponent<AudioSource>().isPlaying)
            GetComponent<AudioSource>().Play();

        speed = 0;
        anim.SetBool("Run", false);
        anim.SetTrigger("Damage");
        curHP -= hp;
    }
}
