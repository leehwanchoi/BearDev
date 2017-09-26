using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AMove : MonoBehaviour
{
    //public float MoveSpeed;
    Vector3 lookDirection;

    VJS vjs;
    ButtonA buttonA;
    ButtonB buttonB;
    MenuButton menuButton;
    Animator animator;
    Raycast raycast;

    public GameObject MenuObject;

    public float max_hp;
    public float cur_hp;
    public bool isDie;

    public AudioClip Idle_Sound;
    public AudioClip Att1_Sound;
    public AudioClip Att2_Sound;
    public AudioClip Damaged_Sound;

    float count;
    float attackTime;

    void Start()
    {
        vjs = GameObject.Find("JoyStick_BackGround").GetComponent<VJS>();
        buttonA = GameObject.Find("ButtonA").GetComponent<ButtonA>();
        buttonB = GameObject.Find("ButtonB").GetComponent<ButtonB>();
        menuButton = GameObject.Find("Menu").GetComponent<MenuButton>();

        raycast = GameObject.Find("Judy").GetComponent<Raycast>();
        animator = GameObject.Find("Judy").GetComponent<Animator>();

        attackTime = 3.0f;
    }

    void Update()
    {
        count += Time.deltaTime;
        attackTime += Time.deltaTime;

        if (!isDie)
        {
            float x = vjs.Horizontal();
            float z = vjs.Vertical();

            lookDirection = x * Vector3.right + z * Vector3.forward;
            this.transform.rotation = Quaternion.LookRotation(lookDirection); // VJS의 Up 제거함.(초기화시키려면 주석제거해야함.)
        }

        //if (vjs.isDown)      // Animation에서 속도값를 줘서 현재는 주석상태. (속도값이 없는 Animation시에는 주석해제)
        //    this.transform.Translate(Vector3.forward * MoveSpeed * Time.deltaTime);

        Animation(); //Animation Function
        GUI();       //Menu Function
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
            if (buttonA.isAllow && attackTime >= 4.0f)
            {
                GetComponent<AudioSource>().clip = Att1_Sound;
                if (!GetComponent<AudioSource>().isPlaying)
                    GetComponent<AudioSource>().Play();

                animator.SetInteger("Condition", 2);

                count = 0;
                attackTime = 0;
            }

            //Skill Animation -> ButtonB Function
            if (buttonB.isDown && attackTime >= 4.0f)
            {
                GetComponent<AudioSource>().clip = Att2_Sound;
                if (!GetComponent<AudioSource>().isPlaying)
                    GetComponent<AudioSource>().Play();

                animator.SetInteger("Condition", 3);

                count = 0;
                attackTime = 0;
            }

            //Hurt Aniamtion
            //밑에 Damaged()를 사용. (적 스크립트에서 불러오는 식으로 사용)
        }
    }

    void GUI()
    {
        if (menuButton.isDown)
            Debug.Log(menuButton.m_State);

        //click
        if (menuButton.isDown && menuButton.m_State != MenuState.DoubleClick)
            MenuObject.SetActive(true);
        
        //double click
        if (menuButton.m_State == MenuState.DoubleClick)
        {
            menuButton.m_State = MenuState.NULL;
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

    public bool ADown()
    {
        return buttonA.isDown;
    }

    public bool BDown()
    {
        return buttonB.isDown;
    }
}