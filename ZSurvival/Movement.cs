using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class Movement : NetworkBehaviour
{
    private VirtualJoystick joyStick;
    private ButtonA buttonA;
    private ButtonB buttonB;
    private GameObject respawnButton;

    private Transform playerTransform;

    [SerializeField] Camera playerCamera;
    [SerializeField] DisableBuidings disableBuidingsScript;

    [SerializeField] Transform playerNameTransform;
    private Vector3 namePos;
    [SerializeField] TextMesh playerNameText;
    [SerializeField] TextMesh playerHPText;

    [SerializeField] NetworkAnimator networkAnimator;
    public Animator animator;

    [SerializeField] Player_Health health;
    [SerializeField] Player_Death death;
    [SerializeField] Player_Respawn respawn;

    [SerializeField] Player_SpearAttack spearAttack;
    [SerializeField] GameObject bloodingParticle;
    [SerializeField] GameObject spearParticle;
    private int attackDelay;
    [HideInInspector] public bool isAttacking;

    private bool isOneAttack = true;
    private bool isTwoAttack = true;
    private bool isThreeAttack = true;

    private float MoveSpeed;
    Vector3 lookDirection;
    Vector2 joystickMove;

    private void Awake()
    {
        joyStick = GameObject.Find("JoyStick").GetComponent<VirtualJoystick>();
        buttonA = GameObject.Find("ButtonA").GetComponent<ButtonA>();
        buttonB = GameObject.Find("ButtonB").GetComponent<ButtonB>();
        playerTransform = this.transform.Find("RangerForLocomotion").transform;
    }

    private void Start()
    {
        Screen.SetResolution(1280, 720, true);

        if (!isLocalPlayer)
        {
            playerNameText.text = "Survivor " + this.GetComponent<Player_ID>().netId.ToString(); // 온라인 유저면 이 형식으로 이름 설정
            playerHPText.text = "100";

            return;
        }

        namePos = new Vector3(-0.22f, 2.2f, 0.0f); // 이름표 추가좌표 설정

        playerCamera.enabled = true; // 로컬이면 카메라 켜기
        playerCamera.GetComponent<AudioListener>().enabled = true; // 로컬이면 오디오 리스너 켜기

        disableBuidingsScript.enabled = true; // 로컬이면 건물 투명화 스크립트 켜기

        playerNameText.text = this.gameObject.name; // 이름 설정
        playerHPText.text = "100";
        networkAnimator.SetParameterAutoSend(0, true); // 네트워크 애니메티너 설정

        attackDelay = 30;
        isAttacking = false;

        MoveSpeed = 5.0f;
    }

    public override void PreStartClient()
    {
        respawnButton = GameObject.Find("GameManager").GetComponent<GameManager_References>().respawnButton;
        networkAnimator.SetParameterAutoSend(0, true);
    }

    private void Update()
    {
        // 죽음 애니메이션 체크 (전부 다 돌려야하기 때문에 로컬, 서버 둘다 업데이트)
        CheckDeath();

        if (!isLocalPlayer)
            return;

        if (health.GetHP() <= 0)
        {
            joyStick.isDown = false;
            return;
        }

        float x = joyStick.Horizontal();
        float z = joyStick.Vertical();

        lookDirection = x * Vector3.right + z * Vector3.forward;
        joystickMove = x * Vector3.right + z * Vector3.forward;

        if (joyStick.isDown && !isAttacking) // 조이스틱을 누른상태고, 어택중이 아니면
        {
            // 애니메이션 내에서 움직임을 변경해주기 때문에 필요없다.
            //playerTransform.Translate(Vector3.forward * MoveSpeed * Time.deltaTime);

            // 애니메이션으로 속도 제어
            if (animator.GetFloat("Speed") <= 1.0f)
                animator.SetFloat("Speed", animator.GetFloat("Speed") + 0.1f);

            playerTransform.rotation = Quaternion.LookRotation(lookDirection); // VJS의 Up 제거함.(초기화시키려면 주석제거해야함.)
        }
        else
        {
            if (animator.GetFloat("Speed") > 0.0f)
                animator.SetFloat("Speed", animator.GetFloat("Speed") - 0.05f);
        }

        // 근접 공격
        if (buttonA.isDown || Input.GetKey(KeyCode.Z))
        {
            Attack();
        }

        // Player_SpawnManager에서 B버튼 눌렀을시 드론에서 총알 나가게 함.
        if (buttonB.isDown)
        {
            // 이 곳에서 사용하지 않음.
        }

        // 이름표 위치 설정
        playerNameTransform.position = playerTransform.position + namePos;
    }

    private void FixedUpdate()
    {
        // HP 설정 (둘다 업데이트)
        if (health.GetHP() > 0)
            playerHPText.text = health.GetHP().ToString();
        else
            playerHPText.text = "Dead";


        if (!isLocalPlayer)
            return;

        // 어택 이펙트
        AttackingEffect();

        // 어택 애니메이션 초기화를 인한 카운트
        if (attackDelay < 1)
            attackDelay++; // 초기화 타이머 카운트 증가

        if (attackDelay >= 1)
        {
            InitAttackState(); // 어택 애니메이션 모두 끄기
        }

        // 죽었으면 어택 끄기
        if (health.GetHP() <= 0)
            isAttacking = false;

        //======================================
        // 적에게 데미지 주기
        //======================================
        // Player_Shoot 에서 주고 있음


        // 공격 초기화 체크
        ChekingAttackInit();
    }

    private void Attack()
    {
        /*
        ================================
        애니메이션 상태 해쉬 값
        Idle Hash : -1437195394
        Attack01 Hash : 826796465
        Attack02 Hash : -1471236085
        Attack03 Hash : -548829027
        Death Hash : 1102808304
        ================================
        무슨 이유인지 모르겠지만, IsName이 먹히질 않는다. 일시적인 버그였을수도,
        그냥 Hash로 찾는거로 해서 다음날 다시 IsName으로 바꾸기 귀찮아서 그냥 Hash로 하고 있다.
        */

        // 카운트 초기화
        attackDelay = 0;
        isAttacking = true;

        // Attack01 애니메이션 실행
        animator.SetBool("Attack01", true); // 826796465

        // 01이 true면 02 애니메이션 실행
        if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash == 826796465)
        {
            animator.SetBool("Attack02", true); // -1471236085
        }

        // 02이 true면 03 애니메이션 실행
        if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash == -1471236085)
        {
            animator.SetBool("Attack03", true); // -548829027
        }
    }

    private void AttackingEffect()
    {
        // 01 애니메이션
        if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash == 826796465)
        {
            // 창 공격 파티클 실행
            if (animator.GetCurrentAnimatorStateInfo(0).normalizedTime < 0.5f)
                spearParticle.SetActive(true);


        }

        // 02 애니메이션
        if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash == -1471236085)
        {
            // 창 공격 파티클 실행
            if (animator.GetCurrentAnimatorStateInfo(0).normalizedTime < 0.5f)
                spearParticle.SetActive(true);


        }

        // 03 애니메이션
        if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash == -548829027)
        {
            // 창 공격 파티클 실행
            if (animator.GetCurrentAnimatorStateInfo(0).normalizedTime < 0.5f)
                spearParticle.SetActive(true);
        }


    }

    private void ChekingAttackInit()
    {
        // 공격 애니메이션 중이 아니면 공격횟수 초기화
        if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash != 826796465 &&
            animator.GetCurrentAnimatorStateInfo(0).fullPathHash != -1471236085 &&
            animator.GetCurrentAnimatorStateInfo(0).fullPathHash != -548829027)
        {
            isOneAttack = true;
            isTwoAttack = true;
            isThreeAttack = true;
        }
    }

    private void InitAttackState()
    {
        animator.SetBool("Attack01", false);
        animator.SetBool("Attack02", false);
        animator.SetBool("Attack03", false);
    }

    private void CheckDeath()
    {
        // 로컬인 경우, 아닌경우 둘다
        // 죽은상태에서 Death 애니메이션 체크
        if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash == 1102808304)
        {
            // 한번만 재생되게 끈다.
            animator.SetBool("Death", false);

            if (health.isDead && animator.GetCurrentAnimatorStateInfo(0).normalizedTime > 0.9f)
            {
                animator.speed = 0;

                if (health.GetHP() <= 0)
                {
                    health.GetCapsuleCollider().enabled = false;
                    //GameObject.Find("GameManager").GetComponent<GameManager_References>().respawnButton.SetActive(true);
                }
            }

            if (health.GetHP() > 0 && animator.speed == 0)
            {
                //GameObject.Find("GameManager").GetComponent<GameManager_References>().respawnButton.SetActive(false);

                // Death 애니메이션에서 0.99f에서 멈추게 해놓음, 부활시에는 스피드 1로 해서 원상복구
                animator.speed = 1;

                health.GetCapsuleCollider().enabled = true;

                health.isDead = false;
            }
        }

        // 로컬 아닌 경우만
        if (!isLocalPlayer)
        {
            if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash == 1102808304)
            {
                if (health.GetHP() > 0 && animator.speed == 0)
                {
                    health.GetCapsuleCollider().enabled = true;
                    health.isDead = false;

                    // Death 애니메이션에서 0.99f에서 멈추게 해놓음, 부활시에는 스피드 1로 해서 원상복구
                    animator.speed = 1;
                }
            }
        }

        // 로컬인 경우만
        if (isLocalPlayer)
        {
            if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash != 1102808304)
            {
                if (health.GetHP() <= 0 && health.shouldDie)
                {
                    health.GetCapsuleCollider().enabled = false;

                    animator.SetBool("Death", true);

                    if (health.isDieDone)
                        health.shouldDie = false;
                }
            }

            if (animator.GetCurrentAnimatorStateInfo(0).fullPathHash == 1102808304)
            {
                if (health.isDead && animator.GetCurrentAnimatorStateInfo(0).normalizedTime > 0.9f)
                {
                    //animator.speed = 0;

                    if (health.GetHP() <= 0)
                    {
                        health.GetCapsuleCollider().enabled = false;

                        if (!respawnButton.activeSelf)
                        {
                            respawnButton.SetActive(true);
                        }
                    }
                }

                if (health.GetHP() > 0 && health.GetCapsuleCollider().enabled && respawnButton.activeSelf)
                {
                    respawnButton.SetActive(false);
                }
            }
        }
    }
}