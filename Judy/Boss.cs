using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public enum bossState
{
    Idle,
    Walk,
    Run,
    Jump,
    Attack
}

public class Boss : MonoBehaviour
{
    BossManager aM;

    Transform playerCoor;
    AMove player;
    Animator anim;

    public NavMeshAgent navMeshAgent;

    public AudioClip hit_Sound;
    public AudioClip Damage_Sound;
    public AudioClip die_Sound;

    public GameObject FX;

    float time;
    float speed;
    public int damage = 10;
    public float maxHP = 100;
    public float curHP = 100;

    bool isDie;
    float dieCount;

    // Use this for initialization
    void Start ()
    {
        aM = GameObject.Find("GameManager").GetComponent<BossManager>();

        anim = GameObject.Find("Boss").GetComponent<Animator>();
        player = GameObject.Find("Judy").GetComponent<AMove>();
        playerCoor = player.transform;
    }
	
	// Update is called once per frame
	void Update ()
    {
        DieCheck();

        if (!player.isDie)
            distanceCheck();

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

                aM.enemyDieCount++;
                Destroy(this.gameObject);

                dieCount = 0;
            }
        }
    }

    void MoveUpdate()
    {
        anim.Play("Walk");

        navMeshAgent.SetDestination(playerCoor.position);

        //transform.rotation = Quaternion.LookRotation(new Vector3(playerCoor.position.x, this.transform.position.y, playerCoor.position.z) - transform.position);
        //transform.Translate(Vector3.forward * speed * Time.deltaTime);
    }

    void distanceCheck()
    {
        time += Time.deltaTime;

        if (Vector3.Distance(playerCoor.position, transform.position) <= 30f && Vector3.Distance(playerCoor.position, transform.position) > 5f)
        {
            speed = 3f;
            MoveUpdate();
        }

        if (Vector3.Distance(playerCoor.position, transform.position) <= 5f)
        {
            Debug.Log(time);
            if (time >= 3f)
            {
                speed = 0;
                Attack();

                time = 0;
            }
        }
    }

    void Attack()
    {
        anim.Play("Attack");

        GetComponent<AudioSource>().clip = hit_Sound;
        if (!GetComponent<AudioSource>().isPlaying)
            GetComponent<AudioSource>().Play();

        transform.rotation = Quaternion.LookRotation(new Vector3(playerCoor.position.x, this.transform.position.y, playerCoor.position.z) - transform.position);
        player.cur_hp -= damage;
    }

    public void Damage(int hp)
    {
        GetComponent<AudioSource>().clip = Damage_Sound;
        if (!GetComponent<AudioSource>().isPlaying)
            GetComponent<AudioSource>().Play();

        speed = 0;
        anim.Play("Get_hit");
        curHP -= hp;
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
}
