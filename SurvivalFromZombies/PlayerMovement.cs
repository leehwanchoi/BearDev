using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerMovement : MonoBehaviour
{
    private VJS vjs;
    private ButtonA buttonA;

    public GameObject PlayerCollider;
    private float MoveSpeed;
    private float RotateSpeed;

    private bool isMoving;

    public GameObject LeftHandPivot;
    public GameObject RightHandPivot;
    private static int LHandDirection;
    private static int RHandDirection;
    private static int LReloadDirection;
    private static int RReloadDirection;

    private AudioSource audioSource_Player; //플레이어 소리(걷기, 뛰기, 숨쉬기 등)
    private AudioSource audioSource_SFX;    //이펙트 소리(총소리, 리로드 등)
    public AudioClip[] audioClips;

    [HideInInspector] public int HP;
    [HideInInspector] public bool isDie;
    private bool isDamaged;
    private float DamagedTime;
    private static int CamDirection;
    private Camera MainCam;
    public GameObject DieUI;
    public GameObject EyesUI;
    private Image BlackTop;
    private Image BlackDown;

    [HideInInspector] public bool isAttack;
    private int BulletMax;
    private int BulletCount;
    private bool LastBullet;
    private float AttackCoolTime;
    public GameObject LShotPoint;
    public GameObject RShotPoint;
    public GameObject Bullet;
    public GameObject ShotFX;
    public GameObject Reload;
    public Text BulletCurrent;
    private bool isPlayReload;
    private float ReloadSoundTime;
    private bool isLReload;
    private bool isRReload;

    private GameObject[] newLBullet;
    private GameObject[] newRBullet;
    private GameObject newLShotFX;
    private GameObject newRShotFX;
    private float ShotPointTime;

    public GameObject HP100;
    public GameObject HP80;
    public GameObject HP60;
    public GameObject HP40;
    public GameObject HP20;
    public GameObject BulletCounter;
    public GameObject HPBar;
    public GameObject ReloadBar;
    public GameObject Joystick;
    public GameObject JoyButtonA;

    private void Awake()
    {
        vjs = GameObject.Find("JoyStick_BackGround").GetComponent<VJS>();
        buttonA = GameObject.Find("ButtonA").GetComponent<ButtonA>();
        vjs.gameObject.SetActive(false);
        buttonA.gameObject.SetActive(false);

        MoveSpeed = 5.0f;
        RotateSpeed = 75.0f;

        isMoving = false;

        LHandDirection = 1;
        RHandDirection = -1;

        LReloadDirection = 1;
        RReloadDirection = -1;

        HP = 100;
        isDie = false;
        isDamaged = false;
        DamagedTime = 0.0f;
        CamDirection = 1;
        MainCam = this.transform.GetComponentInChildren<Camera>();

        DieUI.SetActive(false);
        EyesUI.SetActive(false);
        BlackTop = EyesUI.transform.Find("BlackTop").GetComponent<Image>();
        BlackDown = EyesUI.transform.Find("BlackDown").GetComponent<Image>();

        isAttack = false;
        BulletMax = 20;
        BulletCount = 0;
        LastBullet = false;
        AttackCoolTime = 1.0f;

        audioSource_Player = this.transform.Find("PlayerCollider").GetComponent<AudioSource>();
        audioSource_SFX = this.GetComponent<AudioSource>();

        audioSource_Player.volume = 0.75f;
        audioSource_SFX.volume = 0.8f;

        Reload.SetActive(false);
        isPlayReload = false;
        ReloadSoundTime = 0.0f;
        isLReload = false;
        isRReload = false;

        BulletCurrent.text = 20 + "      ";

        newLBullet = new GameObject[20];
        newRBullet = new GameObject[20];

        for (int i = 0; i < BulletMax; ++i)
        {
            newLBullet[i] = Instantiate(Bullet, Vector3.zero, Quaternion.identity);
            newRBullet[i] = Instantiate(Bullet, Vector3.zero, Quaternion.identity);
        }

        newLShotFX = Instantiate(ShotFX, LShotPoint.transform.position, Quaternion.identity);
        newRShotFX = Instantiate(ShotFX, RShotPoint.transform.position, Quaternion.identity);

        newLShotFX.transform.localScale = new Vector3(0.25f, 0.25f, 0.25f);
        newRShotFX.transform.localScale = new Vector3(0.25f, 0.25f, 0.25f);

        newLShotFX.SetActive(false);
        newRShotFX.SetActive(false);
    }

    private void FixedUpdate()
    {
        //초기화
        isMoving = false;
        isAttack = false;

        if (!isDie)
        {
            if (vjs.enabled && buttonA.enabled)
            {
                float x = vjs.Vertical();
                float z = vjs.Horizontal();

                if (vjs.isDown && x > 0 && (z <= 0.75f && z >= -0.75f))
                {
                    PlayerCollider.transform.Translate(Vector3.forward * MoveSpeed * Time.deltaTime);
                    isMoving = true;
                }
                if (vjs.isDown && x < 0 && (z <= 0.75f && z >= -0.75f))
                {
                    PlayerCollider.transform.Translate(Vector3.back * MoveSpeed * Time.deltaTime);
                    isMoving = true;
                }

                if (vjs.isDown && (z > 0.3f && z < 1.0f))
                {
                    PlayerCollider.transform.Rotate(Vector3.up * RotateSpeed * Time.deltaTime);
                    isMoving = true;
                }
                if (vjs.isDown && (z < -0.3f && z > -1.0f))
                {
                    PlayerCollider.transform.Rotate(Vector3.down * RotateSpeed * Time.deltaTime);
                    isMoving = true;
                }

                if (!vjs.isDown)
                {
                    x = 0.0f;
                    z = 0.0f;
                }

                if (buttonA.isDown)
                {
                    isAttack = true;
                }
            }

            if (Input.GetKey(KeyCode.UpArrow) || Input.GetKey(KeyCode.W))
            {
                PlayerCollider.transform.Translate(Vector3.forward * MoveSpeed * Time.deltaTime);
                isMoving = true;
            }
            else if (Input.GetKey(KeyCode.DownArrow) || Input.GetKey(KeyCode.S))
            {
                PlayerCollider.transform.Translate(Vector3.back * MoveSpeed * Time.deltaTime);
                isMoving = true;
            }

            if (Input.GetKey(KeyCode.LeftArrow) || Input.GetKey(KeyCode.A))
            {
                PlayerCollider.transform.Rotate(Vector3.down * RotateSpeed * Time.deltaTime);
                isMoving = true;
            }
            else if (Input.GetKey(KeyCode.RightArrow) || Input.GetKey(KeyCode.D))
            {
                PlayerCollider.transform.Rotate(Vector3.up * RotateSpeed * Time.deltaTime);
                isMoving = true;
            }

            if (/*Input.GetKey(KeyCode.Mouse0) ||*/ Input.GetKey(KeyCode.Z))
            {
                isAttack = true;
            }

            if (isDamaged)
            {
                DamagedTime += Time.deltaTime;

                if (DamagedTime >= 0.5f)
                {
                    isDamaged = false;
                    DamagedTime = 0.0f;

                    MainCam.transform.localPosition = new Vector3(0, this.transform.position.y + 0.8f, 0);
                    MainCam.transform.localRotation = Quaternion.identity;
                }
                else
                {
                    MainCam.transform.Translate(Vector3.up * CamDirection * 0.1f);
                    MainCam.transform.Rotate(Vector3.up * CamDirection * 5.0f);

                    if (MainCam.transform.localPosition.y > 0.9f)
                        CamDirection = -CamDirection;
                    else if (MainCam.transform.localPosition.y < 0.7f)
                        CamDirection = -CamDirection;
                }
            }

            Shot();
            HandAnimation();
            SoundnSFX();
        }
        else if (isDie)
        {
            EyesUI.SetActive(true);

            if (BlackTop.color.a <= 1.0f)
            {
                BlackTop.color = new Color(BlackTop.color.r, BlackTop.color.g, BlackTop.color.b, BlackTop.color.a + 0.0025f);
            }
            if (BlackDown.color.a <= 1.0f)
            {
                BlackDown.color = new Color(BlackDown.color.r, BlackDown.color.g, BlackDown.color.b, BlackDown.color.a + 0.002f);
            }

            if (BlackTop.transform.localPosition.y > 200)
            {
                BlackTop.transform.Translate(Vector3.down * 1.5f);
            }
            else
            {
                //BlackTop기준으로 다 내려왔으면 DieUI 출력
                DieUI.SetActive(true);
            }

            if (BlackDown.transform.localPosition.y < -200)
            {
                BlackDown.transform.Translate(Vector3.up * 1.5f);
            }

            if (this.transform.localRotation.eulerAngles.x < 70)
            {
                this.transform.Rotate(this.transform.localRotation.x + 0.5f, this.transform.localRotation.y, this.transform.localRotation.z);
            }
        }

        //Die와 상관없이 진행되야 되기 때문에 밖에서 돌린다.
        HPUpdate();
    }

    private void HPUpdate()
    {
        if (HP < 100)
            HP100.SetActive(false);
        if (HP < 80)
            HP80.SetActive(false);
        if (HP < 60)
            HP60.SetActive(false);
        if (HP < 40)
            HP40.SetActive(false);
        if (HP < 20)
        {
            HP20.SetActive(false);
            BulletCounter.SetActive(false);
            HPBar.SetActive(false);
            ReloadBar.SetActive(false);
            Joystick.SetActive(false);
            JoyButtonA.SetActive(false);
        }
    }

    private void SoundnSFX()
    {
        if (isMoving)
        {
            //[2] 걷는 소리
            if (!audioSource_Player.isPlaying)
            {
                audioSource_Player.clip = audioClips[2];
                audioSource_Player.Play();
            }
        }
        else
        {
            audioSource_Player.Stop();
        }
    }

    private void Shot()
    {
        AttackCoolTime += Time.deltaTime;

        if (isAttack && AttackCoolTime >= 0.1f && BulletCount < BulletMax)
        {
            ShotPointTime += Time.deltaTime;

            //[0] 총쏘는 소리
            audioSource_SFX.clip = audioClips[0];
            audioSource_SFX.Play();

            newLBullet[BulletCount].SetActive(true);
            newLBullet[BulletCount].GetComponent<BulletMovement>().isAlive = true;
            newRBullet[BulletCount].SetActive(true);
            newRBullet[BulletCount].GetComponent<BulletMovement>().isAlive = true;


            newLBullet[BulletCount].transform.position = LShotPoint.transform.position;
            newRBullet[BulletCount].transform.position = RShotPoint.transform.position;
            newLBullet[BulletCount].transform.rotation = PlayerCollider.transform.rotation;
            newRBullet[BulletCount].transform.rotation = PlayerCollider.transform.rotation;

            newLShotFX.SetActive(true);
            newRShotFX.SetActive(true);

            newLShotFX.transform.position = LShotPoint.transform.position;
            newRShotFX.transform.position = RShotPoint.transform.position;

            BulletCount++;

            BulletCurrent.text = (20 - BulletCount) + "      ";

            //기존 동적할당 방식 (메모리 소모 및 성능저하의 이유로 사용하지 않음) (Awake 때 20개를 미리 생성후 재활용함)
            //GameObject newLBullet = Instantiate(Bullet, LShotPoint.transform.position, PlayerCollider.transform.rotation);
            //GameObject newRBullet = Instantiate(Bullet, RShotPoint.transform.position, PlayerCollider.transform.rotation);
            //Destroy(newLBullet, 5.0f);
            //Destroy(newRBullet, 5.0f);

            AttackCoolTime = 0.0f;

            if (BulletCount == BulletMax)
                LastBullet = true;
        }

        //리로드 UI 표시
        if (isAttack && AttackCoolTime < 0.1f && LastBullet)
        {
            Reload.SetActive(true);
            isPlayReload = true;
        }

        if (isPlayReload)
        {
            ReloadSoundTime += Time.deltaTime;

            if (audioSource_SFX.clip != audioClips[1] && ReloadSoundTime > 0.5f)
            {
                //[1] 리로드 소리
                audioSource_SFX.clip = audioClips[1];
                audioSource_SFX.Play();

                isLReload = true;
            }

            if (ReloadSoundTime > 2.5f)
            {
                //[1] 리로드 소리
                audioSource_SFX.clip = audioClips[1];
                audioSource_SFX.Play();

                ReloadSoundTime = 0.0f;
                isPlayReload = false;
            }
        }

        //총알을 다쓴 상태고, 쿨타임이 5.0f가 되었다면, 리로드
        if (BulletCount >= BulletMax && LastBullet && AttackCoolTime >= 5.0f)
        {
            LastBullet = false;
            BulletCount = 0;
            BulletCurrent.text = (20 - BulletCount) + "      ";
            AttackCoolTime = 0.0f;

            Reload.transform.Find("Fill").GetComponent<Image>().fillAmount = 0.0f;
            Reload.SetActive(false);
        }
        else if (BulletCount >= BulletMax && LastBullet && AttackCoolTime < 5.0f)
        {
            Reload.transform.Find("Fill").GetComponent<Image>().fillAmount += AttackCoolTime % 5.0f * Time.deltaTime * 0.08f;

            ReloadAnimation();

            if (!isLReload && !isRReload)
            {
                LeftHandPivot.transform.localRotation = Quaternion.Euler(0, 20.0f, 0);
                RightHandPivot.transform.localRotation = Quaternion.Euler(0, 340.0f, 0);
            }
        }

        if (!isAttack || LastBullet)
        {
            newLShotFX.SetActive(false);
            newRShotFX.SetActive(false);
        }
    }

    private void ReloadAnimation()
    {
        if (isLReload)
        {
            LeftHandPivot.transform.Rotate(Vector3.up * LReloadDirection * 0.725f);
            LeftHandPivot.transform.Find("LeftHand").transform.Rotate(Vector3.forward * LReloadDirection * 0.725f);

            if (LeftHandPivot.transform.localRotation.eulerAngles.y > 55.0f)
            {
                LReloadDirection = -LReloadDirection;
            }

            if (LeftHandPivot.transform.localRotation.eulerAngles.y < 20.0f && LReloadDirection == -1)
            {
                LeftHandPivot.transform.localRotation = Quaternion.Euler(0, 20.0f, 0);
                LeftHandPivot.transform.Find("LeftHand").transform.localRotation = Quaternion.Euler(0, 0, 0);

                isRReload = true;
            }
        }
        
        if (isRReload)
        {
            RightHandPivot.transform.Rotate(Vector3.up * RReloadDirection * 0.725f);
            RightHandPivot.transform.Find("RightHand").transform.Rotate(Vector3.forward * RReloadDirection * 0.725f);

            if (RightHandPivot.transform.localRotation.eulerAngles.y < 305.0f && RReloadDirection == -1)
            {
                RReloadDirection = -RReloadDirection;
            }

            if (RightHandPivot.transform.localRotation.eulerAngles.y > 340.0f && RReloadDirection == 1)
            {
                RightHandPivot.transform.localRotation = Quaternion.Euler(0, 340.0f, 0);
                RightHandPivot.transform.Find("RightHand").transform.localRotation = Quaternion.Euler(0, 0, 0);

                isRReload = false;
                isLReload = false;

                RReloadDirection = -1;
                LReloadDirection = 1;
            }
        }
    }

    private void HandAnimation()
    {
        if (isAttack && BulletCount < BulletMax)
        {
            LeftHandPivot.transform.Rotate(Vector3.left * LHandDirection);
            RightHandPivot.transform.Rotate(Vector3.left * RHandDirection);

            //손 모션 제한 (왼손 기준)
            if (LeftHandPivot.transform.localRotation.eulerAngles.x < 350.0f)
            {
                LHandDirection = -LHandDirection;
                RHandDirection = -RHandDirection;
            }

            if (LeftHandPivot.transform.localRotation.eulerAngles.x < 10.0f)
            {
                LHandDirection = -LHandDirection;
                RHandDirection = -RHandDirection;
            }
        }
        else
        {
            LeftHandPivot.transform.rotation = new Quaternion(0, LeftHandPivot.transform.rotation.y, 0, LeftHandPivot.transform.rotation.w);
            RightHandPivot.transform.rotation = new Quaternion(0, RightHandPivot.transform.rotation.y, 0, RightHandPivot.transform.rotation.w);
        }
    }

    public void Attacked(int DamagedHP)
    {
        HP -= DamagedHP;
        isDamaged = true;

        if (HP <= 0)
        {
            HP = 0;
            isDie = true;
        }
    }
}
