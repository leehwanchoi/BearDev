using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

[RequireComponent(typeof(NavMeshAgent))]
public class enemyMovement01 : MonoBehaviour
{
    private GamePlayManager GPManager;

    public GameObject Target;
    private NavMeshAgent NavMesh;

    private AudioSource audioSource_Enemy;  //적 소리(걷기, 뛰기, 죽기 등)
    private AudioSource audioSource_SFX;    //이펙트 소리(공격, 스킬 등)
    public AudioClip[] audioClips;

    public GameObject BloodingFX;
    public GameObject BoomFX;

    public GameObject RHand;
    public GameObject LHand;

    private static int LHandDirection;
    private static int RHandDirection;

    private bool isRise;

    public int HP;
    private bool isAttacked;
    private float PoolingTime;
    [HideInInspector] public bool isDie;
    private bool isBoomFX;

    private void Awake()
    {
        GPManager = GameObject.Find("GameManager").GetComponent<GamePlayManager>();

        NavMesh = this.GetComponent<NavMeshAgent>();
        NavMesh.SetDestination(Target.transform.position);
        NavMesh.enabled = false;

        audioSource_Enemy = this.transform.Find("chr_EnemyChild").GetComponent<AudioSource>();
        audioSource_SFX = this.GetComponent<AudioSource>();
        audioSource_Enemy.volume = 0.0f;
        audioSource_SFX.volume = 0.8f;

        BloodingFX.SetActive(false);
        BoomFX.SetActive(false);

        LHandDirection = 1;
        RHandDirection = -1;

        this.transform.position = new Vector3(this.transform.position.x, -2.2f, this.transform.position.z);
        isRise = false;

        HP = 100;
        isAttacked = false;
        PoolingTime = 0.0f;
        isDie = false;
        isBoomFX = false;
    }

    private void FixedUpdate()
    {
        if (GPManager.stageState == StageState.Stage01)
        {
            if (!isDie)
            {
                if (!isRise)
                {
                    if (this.transform.position.y < 0.333f)
                    {
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
                    HandAnimation();

                    if ((Target.transform.position - this.transform.position).magnitude > 3.0f)
                    {
                        NavMesh.isStopped = false;

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
                        NavMesh.isStopped = true;
                        audioSource_Enemy.Stop();
                    }
                }
            }
            else if (isDie)
            {
                if (this.gameObject.tag == "Stage01Enemy")
                {
                    if (this.transform.Find("chr_EnemyChild").GetComponent<MeshRenderer>().enabled)
                    {
                        this.transform.Find("chr_EnemyChild").GetComponent<MeshRenderer>().enabled = false;
                        LHand.SetActive(false);
                        RHand.SetActive(false);
                    }

                    if (!audioSource_SFX.isPlaying && audioSource_SFX.clip == audioClips[2])
                    {
                        Destroy(this.gameObject);

                        GPManager.DeadStage01Enemy++;
                        isDie = false;
                        isBoomFX = false;

                        Destroy(this);
                    }
                }
            }
        }
    }

    private void HandAnimation()
    {
        LHand.transform.Rotate(Vector3.left * LHandDirection);
        RHand.transform.Rotate(Vector3.left * RHandDirection);

        //손 모션 제한 (왼손 기준)
        if (LHand.transform.localRotation.x > 0.3f)
        {
            LHandDirection = -LHandDirection;
            RHandDirection = -RHandDirection;
        }
        else if (LHand.transform.localRotation.x < -0.3f)
        {
            LHandDirection = -LHandDirection;
            RHandDirection = -RHandDirection;
        }
    }

    private void VolumeResize()
    {
        if ((Target.transform.position - this.transform.position).magnitude > 12.0f)
        {
            audioSource_Enemy.volume = 0.0f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 10.0f)
        {
            audioSource_Enemy.volume = 0.2f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 8.0f)
        {
            audioSource_Enemy.volume = 0.3f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 6.0f)
        {
            audioSource_Enemy.volume = 0.5f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 4.0f)
        {
            audioSource_Enemy.volume = 0.6f;
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

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "PlayerBullet")
        {
            GameObject newBloodingFX = Instantiate(BloodingFX, other.transform.position, Quaternion.identity);
            newBloodingFX.SetActive(true);
            Destroy(newBloodingFX, 2.5f);
        }
    }
}