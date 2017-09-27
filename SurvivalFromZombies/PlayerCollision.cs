using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerCollision : MonoBehaviour
{
    private PlayerMovement playerMovement;

    private AudioSource audioSource_Player;  //플레이어 소리(걷기, 뛰기, 숨쉬기 등)
    public AudioSource audioSource_SFX;      //이펙트 소리(총소리, 리로드 등)
    public AudioSource auioSource_Attacked; //피격 소리
    public AudioClip[] audioClips;

    public GameObject BloodUI;
    private Image BloodImage;
    public Sprite[] sprites;

    private bool isBlood;
    private float BloodTime;

    private void Awake()
    {
        playerMovement = GameObject.Find("Player").GetComponent<PlayerMovement>();

        audioSource_Player = this.GetComponent<AudioSource>();

        BloodImage = BloodUI.GetComponent<Image>();
        BloodImage.sprite = null;

        isBlood = false;
        BloodTime = 0.0f;
    }

    private void FixedUpdate()
    {
        if (isBlood)
        {
            BloodTime += Time.deltaTime;

            if (BloodTime >= 1.5f)
            {
                BloodTime = 0.0f;
                isBlood = false;
                BloodUI.SetActive(false);
                audioSource_Player.Stop();
            }
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (this.gameObject.name == "PlayerCollider")
        {
            if (other.name == "RHand" || other.name == "LHand")
            {
                if (!isBlood && !playerMovement.isDie)
                {
                    isBlood = true;
                    BloodUI.SetActive(true);

                    auioSource_Attacked.clip = audioClips[Random.Range(0, audioClips.Length)];
                    auioSource_Attacked.Play();

                    BloodImage.sprite = sprites[Random.Range(0, sprites.Length)];

                    playerMovement.Attacked(20);
                }
            }

            if (other.name == "ArmL" || other.name == "ArmR" || other.name == "HandL" || other.name == "HandR")
            {
                if (other.GetComponentInParent<enemyMovement02>().isAttack) // 공격중인지 체크후
                {
                    if (!isBlood && !playerMovement.isDie)
                    {
                        isBlood = true;
                        BloodUI.SetActive(true);

                        auioSource_Attacked.clip = audioClips[Random.Range(0, audioClips.Length)];
                        auioSource_Attacked.Play();

                        BloodImage.sprite = sprites[Random.Range(0, sprites.Length)];

                        playerMovement.Attacked(20);

                        //행동이 다 끝나면, false
                        other.GetComponentInParent<enemyMovement02>().isAttack = false;
                        other.GetComponentInParent<Animator>().SetBool("Attack01", false);
                        other.GetComponentInParent<Animator>().SetBool("Attack02", false);
                    }
                }
            }
        }

        if (this.gameObject.name == "HitPoint")
        {
            if (other.tag == "EnemyBullet")
            {
                if (!isBlood && !playerMovement.isDie)
                {
                    isBlood = true;
                    BloodUI.SetActive(true);

                    auioSource_Attacked.clip = audioClips[Random.Range(0, audioClips.Length)];
                    auioSource_Attacked.Play();

                    BloodImage.sprite = sprites[Random.Range(0, sprites.Length)];

                    playerMovement.Attacked(20);
                }
            }
        }

        if (other.name == "DeadZone")
        {
            playerMovement.Attacked(1000);
        }
    }
}