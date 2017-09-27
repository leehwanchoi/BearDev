using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

[RequireComponent(typeof(NavMeshAgent))]
public class enemyMovement02 : MonoBehaviour
{
    private GamePlayManager GPManager;

    public GameObject Target;
    private NavMeshAgent NavMesh;

    private Animator Anim;
    private float AttackCoolTime;
    private int AttackState;

    public AudioSource audioSource_Enemy;  //적 소리(걷기, 뛰기, 죽기 등)
    public AudioSource audioSource_SFX;    //이펙트 소리(공격, 스킬 등)
    public AudioClip[] audioClips;

    public GameObject BloodingFX;
    public GameObject BoomFX;

    private bool isRise;

    public int HP;
    [HideInInspector] public bool isAttack;
    private bool isAttacked;
    private float PoolingTime;
    [HideInInspector] public bool isDie;
    private float ExceptionDieTime;
    private bool isBoomFX;

    private void Awake()
    {
        GPManager = GameObject.Find("GameManager").GetComponent<GamePlayManager>();

        NavMesh = this.GetComponent<NavMeshAgent>();
        NavMesh.enabled = false;

        Anim = this.GetComponentInChildren<Animator>();
        AttackCoolTime = 2.5f;
        AttackState = 0;

        audioSource_Enemy.volume = 0.0f;
        audioSource_SFX.volume = 0.8f;

        this.transform.position = new Vector3(this.transform.position.x, -2.2f, this.transform.position.z);
        isRise = false;

        HP = 100;
        isAttack = false;
        isAttacked = false;
        PoolingTime = 0.0f;
        isDie = false;
        ExceptionDieTime = 0.0f;
        isBoomFX = false;
    }

    private void FixedUpdate()
    {
        if (GPManager.stageState == StageState.Stage02)
        {
            if (!isDie)
            {
                if (!isRise)
                {
                    if (this.transform.position.y < 0.333f)
                    {
                        if (GPManager.CreateState02Enemy.isCollision)
                            this.transform.position = new Vector3(this.transform.position.x, this.transform.position.y + 0.75f * Time.deltaTime, this.transform.position.z);
                    }
                    else
                    {
                        isRise = true;
                        NavMesh.enabled = true;
                    }
                }
                else
                {
                    VolumeResize();

                    if ((Target.transform.position - this.transform.position).magnitude > 1.91f)
                    {
                        AttackCoolTime = 2.5f;

                        NavMesh.isStopped = false;

                        Anim.SetBool("Attack01", false);
                        Anim.SetBool("Attack02", false);
                        Anim.SetBool("Walk", true);

                        if (!audioSource_Enemy.isPlaying)
                        {
                            //[0] 좀비 걷는 소리
                            audioSource_Enemy.clip = audioClips[0];
                            audioSource_Enemy.Play();
                        }

                        if (!isAttacked)
                            NavMesh.SetDestination(Target.transform.position);
                        else
                        {
                            this.transform.Translate(Vector3.back * 0.1f);

                            NavMesh.isStopped = true;
                            PoolingTime += Time.deltaTime;

                            if (PoolingTime > 0.5f)
                            {
                                isAttacked = false;
                                PoolingTime = 0.0f;
                            }
                        }
                    }
                    else
                    {
                        Anim.SetBool("Walk", false);

                        AttackCoolTime += Time.deltaTime;

                        if (AttackCoolTime >= 2.5f)
                        {
                            isAttack = true;

                            AttackCoolTime = 0.0f;
                            AttackState = Random.Range(0, 1);

                            if (AttackState == 0)
                                Anim.SetBool("Attack01", true);
                            else
                                Anim.SetBool("Attack02", true);
                        }

                        NavMesh.isStopped = true;
                        audioSource_Enemy.Stop();
                    }
                }
            }
            else if (isDie)
            {
                if (this.gameObject.tag == "Stage02Enemy")
                {
                    if (NavMesh)
                    {
                        NavMesh.enabled = false;
                    }

                    Anim.SetBool("Attack01", false);
                    Anim.SetBool("Attack02", false);
                    Anim.SetBool("Walk", false);
                    Anim.SetBool("Die", true);

                    if (this.transform.localRotation.eulerAngles.x < 70.0f)
                        this.transform.Rotate(Vector3.right * 5.0f);

                    if (this.transform.localRotation.eulerAngles.x > 70.0f)
                    {
                        if (!audioSource_SFX.isPlaying && audioSource_SFX.clip == audioClips[2])
                        {
                            Destroy(this.gameObject);

                            GPManager.DeadStage02Enemy++;
                            isDie = false;

                            Destroy(this);
                        }
                    }

                    ExceptionDieTime += Time.deltaTime;

                    if (ExceptionDieTime >= 2.0f)
                    {
                        Destroy(this.gameObject);

                        GPManager.DeadStage02Enemy++;
                        isDie = false;

                        Destroy(this);
                    }
                }
            }
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "PlayerBullet")
        {
            GameObject newBloodingFX = Instantiate(BloodingFX, other.transform.position, other.transform.rotation);
            newBloodingFX.SetActive(true);
            Destroy(newBloodingFX, 2.5f);
        }
    }

    public void Attacked(int DamagedHP)
    {
        if (HP <= 0)
        {
            HP = 0;
            isDie = true;

            if (!isBoomFX)
            {
                isBoomFX = true;

                GameObject newBoomFX = Instantiate(BoomFX, this.transform.localPosition, Quaternion.identity);
                newBoomFX.SetActive(true);
                Destroy(newBoomFX, 2.5f);
            }

            if (audioSource_SFX.clip != audioClips[2])
            {
                //[2] 폭발소리
                audioSource_SFX.clip = audioClips[2];
                audioSource_SFX.Play();
            }
        }
        else
        {
            isAttacked = true;

            //[1] 크리쳐 피격소리
            if (!audioSource_SFX.isPlaying)
            {
                audioSource_SFX.clip = audioClips[1];
                audioSource_SFX.Play();
            }

            HP -= DamagedHP;
        }
    }

    private void VolumeResize()
    {
        if ((Target.transform.position - this.transform.position).magnitude > 15.0f)
        {
            audioSource_Enemy.volume = 0.0f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 13.0f)
        {
            audioSource_Enemy.volume = 0.2f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 10.0f)
        {
            audioSource_Enemy.volume = 0.3f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 8.0f)
        {
            audioSource_Enemy.volume = 0.5f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 6.0f)
        {
            audioSource_Enemy.volume = 0.6f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 5.0f)
        {
            audioSource_Enemy.volume = 0.7f;
        }
    }
}