using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Move1 : MonoBehaviour
{
    //public float MoveSpeed;
    Vector3 lookDirection;

    VJS vjs;
    ButtonA buttonA;
    ButtonB buttonB;
    Animator animator;
    Raycast1 raycast1;

    public float max_hp;
    public float cur_hp;
    bool isDie;

    public bool isRadder;
    public bool isForward;
    public float startCoor;
    public float endCoor;
    public float speed;
    public Vector3 movement;

    public AudioClip Idle_Sound;
    public AudioClip Att1_Sound;
    public AudioClip Att2_Sound;
    public AudioClip Damaged_Sound;

    float count;

    void Start()
    {
        vjs = GameObject.Find("JoyStick_BackGround").GetComponent<VJS>();
        buttonA = GameObject.Find("ButtonA").GetComponent<ButtonA>();
        buttonB = GameObject.Find("ButtonB").GetComponent<ButtonB>();

        raycast1 = GameObject.Find("Judy").GetComponent<Raycast1>();
        animator = GameObject.Find("Judy").GetComponent<Animator>();
    }

    void Update()
    {
        count += Time.deltaTime;

        if (!isDie)
        {
            float x = vjs.Horizontal();
            float z = vjs.Vertical();

            lookDirection = z * Vector3.left + x * Vector3.forward;
            this.transform.rotation = Quaternion.LookRotation(lookDirection); // VJS의 Up 제거함.(초기화시키려면 주석제거해야함.)
        }

        //if (vjs.isDown)      // Animation에서 속도값를 줘서 현재는 주석상태. (속도값이 없는 Animation시에는 주석해제)
        //    this.transform.Translate(Vector3.forward * MoveSpeed * Time.deltaTime);

        Animation(); //Animation Function

        if (isRadder)
        {

        }
    }

    void Animation()
    {
        //Die Animation -> cur_hp <= 0
        if (cur_hp <= 0)
        {
            animator.SetInteger("Condition", 5);
            isDie = true;
        }

        if (!isDie)
        {
            //Idle Aniamtion
            if (!vjs.isDown && !buttonA.isDown && !buttonB.isDown)
            {
                animator.SetInteger("Condition", 0);

                if (count >= 15.0f)
                {
                    GetComponent<AudioSource>().clip = Idle_Sound;
                    if (!GetComponent<AudioSource>().isPlaying)
                        GetComponent<AudioSource>().Play();

                    count = 0;
                }
            }

            //Run Animation -> Virtual Joystick Function
            if (vjs.isDown)
            {
                animator.SetInteger("Condition", 1);

                count = 0;
            }

            //Attack Animation -> ButtonA Function
            if (buttonA.isAllow)
            {
                GetComponent<AudioSource>().clip = Att1_Sound;
                if (!GetComponent<AudioSource>().isPlaying)
                    GetComponent<AudioSource>().Play();

                animator.SetInteger("Condition", 2);

                count = 0;
            }

            //Skill Animation -> ButtonB Function
            if (buttonB.isDown)
            {
                GetComponent<AudioSource>().clip = Att2_Sound;
                if (!GetComponent<AudioSource>().isPlaying)
                    GetComponent<AudioSource>().Play();

                animator.SetInteger("Condition", 3);

                count = 0;
            }

            //Hurt Aniamtion
            //밑에 Damaged()를 사용. (적 스크립트에서 불러오는 식으로 사용)
        }
    }

    public void Damaged(int hp)
    {
        cur_hp -= hp;

        animator.SetInteger("Condition", 4);

        GetComponent<AudioSource>().clip = Damaged_Sound;
        if (!GetComponent<AudioSource>().isPlaying)
            GetComponent<AudioSource>().Play();
    }
}