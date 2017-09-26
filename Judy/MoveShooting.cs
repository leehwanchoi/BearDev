using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MoveShooting : MonoBehaviour
{
    public GameObject FailedBox;

    VJS vjs;
    ButtonA buttonA;
    ButtonB buttonB;
    ShootingGameManager SGM;
    Front_Collision f_Collision;

    Animator animator;

    public GameObject bullet;
    public int bulletMax = 10;
    public int bulletCur = 10;

    public float moveSpeed = 15.0f;
    public float max_hp = 100;
    public float cur_hp = 100;
    bool isDie;
    bool isTrigger;

    public AudioClip Att1_Sound;
    public AudioClip Damaged_Sound;

    // Use this for initialization
    void Start()
    {
        vjs = GameObject.Find("JoyStick_BackGround").GetComponent<VJS>();
        buttonA = GameObject.Find("ButtonA").GetComponent<ButtonA>();
        buttonB = GameObject.Find("ButtonB").GetComponent<ButtonB>();
        SGM = GameObject.Find("ShootingGameManager").GetComponent<ShootingGameManager>();
        f_Collision = GameObject.Find("CollisionBox_F").GetComponent<Front_Collision>();
        animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        if (SGM.DialogueNum > 0) //튜토리얼 대화 확인 후
        {
            if (cur_hp <= 0)
                isDie = true;

            if (!isDie)
            {
                Movement();

                if (buttonA.isDown || Input.GetKey(KeyCode.Space))
                    Shoot();
                else
                    animator.SetInteger("Condition", 0);

                if (isTrigger)
                    animator.SetInteger("Condition", 4);
            }
            else FailedBox.SetActive(true);
        }
    }

    void Movement()
    {
        float x = vjs.Horizontal();
        float z = vjs.Vertical();

        if (vjs.isDown)
            this.transform.position = new Vector3(this.transform.position.x + (x / moveSpeed), this.transform.position.y, this.transform.position.z + (z / (moveSpeed * 2)));
    }

    void Shoot()
    {
        if (bulletCur > 0)
        {
            animator.SetInteger("Condition", 3);

            GetComponent<AudioSource>().clip = Att1_Sound;

            //if (!GetComponent<AudioSource>().isPlaying)
                GetComponent<AudioSource>().Play();

            Instantiate(bullet, GameObject.Find("ShotPoint").GetComponent<Transform>().position, GameObject.Find("ShotPoint").GetComponent<Transform>().rotation);
            bulletCur--;
        }
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.name == "CollisionBox_B")
        {
            f_Collision.isMove = false;
        }

        if (other.name == "eBullet_B(Clone)" || other.name == "frogBullet_R(Clone)")
        {
            cur_hp -= 5;

            GetComponent<AudioSource>().clip = Damaged_Sound;
            if (!GetComponent<AudioSource>().isPlaying)
            {
                isTrigger = true;
                GetComponent<AudioSource>().Play();
            }
            else isTrigger = false;
        }

        if (other.name == "greenBullet_G(Clone)" || other.name == "PurpleBall(Clone)")
        {
            cur_hp -= 5;

            GetComponent<AudioSource>().clip = Damaged_Sound;
            if (!GetComponent<AudioSource>().isPlaying)
            {
                isTrigger = true;
                GetComponent<AudioSource>().Play();
            }
            else isTrigger = false;

            if (other.name == "PurpleBall(Clone)")
                Destroy(other.gameObject);
        }

        if (other.name == "greenBall(Clone)")
        {
            cur_hp -= 10;

            animator.SetInteger("Condition", 4);

            GetComponent<AudioSource>().clip = Damaged_Sound;
            if (!GetComponent<AudioSource>().isPlaying)
            {
                isTrigger = true;
                GetComponent<AudioSource>().Play();
            }
            else isTrigger = false;
        }

        if (other.name == "zipper")
        {
            cur_hp -= 5;

            animator.SetInteger("Condition", 4);

            GetComponent<AudioSource>().clip = Damaged_Sound;
            if (!GetComponent<AudioSource>().isPlaying)
            {
                isTrigger = true;
                GetComponent<AudioSource>().Play();
            }
            else isTrigger = false;
        }

        if (other.name == "LHand" || other.name == "RHand")
        {
            cur_hp -= 20;

            animator.SetInteger("Condition", 4);

            GetComponent<AudioSource>().clip = Damaged_Sound;
            if (!GetComponent<AudioSource>().isPlaying)
            {
                isTrigger = true;
                GetComponent<AudioSource>().Play();
            }
            else isTrigger = false;
        }
    }
}
