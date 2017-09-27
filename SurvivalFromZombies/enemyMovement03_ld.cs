using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

[RequireComponent(typeof(NavMeshAgent))]
public class enemyMovement03_ld : MonoBehaviour
{
    private GamePlayManager GPManager;

    public GameObject Target;
    private NavMeshAgent NavMesh;

    private AudioSource audioSource_Enemy;  //적 소리(걷기, 뛰기, 죽기 등)
    private AudioSource audioSource_SFX;    //이펙트 소리(공격, 스킬 등)
    public AudioClip[] audioClips;

    public GameObject BloodingFX;
    public GameObject BoomFX;

    public GameObject ShotFX;
    private GameObject newShotFX;
    private float ShotFXTime;
    private bool isShotFX;
    public GameObject enemyBullet;
    private float ShotTime;
    private Transform ShotPoint;

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

        audioSource_Enemy = this.transform.Find("chr_army4Child").GetComponent<AudioSource>();
        audioSource_SFX = this.GetComponent<AudioSource>();
        audioSource_Enemy.volume = 0.0f;
        audioSource_SFX.volume = 0.8f;

        BloodingFX.SetActive(false);
        BoomFX.SetActive(false);

        newShotFX = Instantiate(ShotFX, Vector3.zero, Quaternion.identity);
        ShotFX.SetActive(false);
        newShotFX.SetActive(false);
        ShotFXTime = 0.0f;
        isShotFX = false;

        ShotTime = 1.5f;
        ShotPoint = this.transform.Find("ShotPoint").transform;

        this.transform.position = new Vector3(this.transform.position.x, 0.25f, this.transform.position.z);
        isRise = false;

        HP = 100;
        isAttacked = false;
        PoolingTime = 0.0f;
        isDie = false;
        isBoomFX = false;
    }

    private void FixedUpdate()
    {
        if (GPManager.stageState == StageState.Stage03)
        {
            if (!isDie)
            {
                if (!isRise)
                {
                    if (this.transform.position.y < 0.333f)
                    {
                        if (GPManager.CreateState03Enemy.isCollision)
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

                    if ((Target.transform.position - this.transform.position).magnitude > 20.0f)
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

                        if (audioSource_Enemy.clip == audioClips[0])
                            audioSource_Enemy.Stop();

                        Shot();
                    }
                }
            }
            else if (isDie)
            {
                if (this.gameObject.tag == "Stage03Enemy_ld")
                {
                    if (this.transform.Find("chr_army4Child").GetComponent<MeshRenderer>().enabled)
                    {
                        this.transform.Find("chr_army4Child").GetComponent<MeshRenderer>().enabled = false;
                    }

                    if (!audioSource_SFX.isPlaying && audioSource_SFX.clip == audioClips[2])
                    {
                        Destroy(this.gameObject);

                        GPManager.DeadStage03Enemy++;
                        isDie = false;
                        isBoomFX = false;

                        Destroy(newShotFX);

                        Destroy(this);
                    }
                }
            }
        }
    }

    private void Shot()
    {
        this.transform.LookAt(Target.transform);

        ShotTime += Time.deltaTime;

        if (isShotFX)
            ShotFXTime += Time.deltaTime;

        if (ShotFXTime > 0.25f)
        {
            isShotFX = false;
            newShotFX.SetActive(false);
            ShotFXTime = 0.0f;
        }

        if (ShotTime > 3.0f)
        {
            ShotTime = 0.0f;

            if (!audioSource_Enemy.isPlaying)
            {
                //[3] 총소리
                audioSource_Enemy.clip = audioClips[3];
                audioSource_Enemy.Play();
            }

            GameObject newBullet = Instantiate(enemyBullet, ShotPoint.transform.position, this.transform.rotation);

            newShotFX.SetActive(true);
            isShotFX = true;
            newShotFX.transform.position = ShotPoint.transform.position;

            Destroy(newBullet, 3.0f);
        }
    }

    private void VolumeResize()
    {
        if ((Target.transform.position - this.transform.position).magnitude > 28.0f)
        {
            audioSource_Enemy.volume = 0.0f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 26.0f)
        {
            audioSource_Enemy.volume = 0.1f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 24.0f)
        {
            audioSource_Enemy.volume = 0.2f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 22.0f)
        {
            audioSource_Enemy.volume = 0.3f;
        }
        else if ((Target.transform.position - this.transform.position).magnitude > 20.0f)
        {
            audioSource_Enemy.volume = 0.4f;
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