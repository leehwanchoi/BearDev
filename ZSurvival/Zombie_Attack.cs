using UnityEngine;
using System.Collections;
using UnityEngine.Networking;

public class Zombie_Attack : NetworkBehaviour {

	private float attackRate  = 5.0f;
	private float nextAttack;
	private int damage = 3;
	private float minDistance = 2.5f;
	private float currentDistance;
	private Transform myTransform;
	private Zombie_Target targetScript;

    [HideInInspector] public bool isDeduct = false;

    [SerializeField] Animator animator;
    [SerializeField] NetworkAnimator networkAnimator;

    [SerializeField] private Material zombieGreen;
	[SerializeField] private Material zombieRed;

    [SerializeField] Zombie_HandAttack leftHandAttack;
    [SerializeField] Zombie_HandAttack rightHandAttack;

    [SerializeField] Zombie_Health health;
    private bool isToggleDeathAnim = false;

    private GameManager_References _gameManager;

    // Use this for initialization
    private void Start () 
	{
        _gameManager = GameObject.Find("GameManager").GetComponent<GameManager_References>();

        myTransform = transform;
		targetScript = GetComponent<Zombie_Target>();

        networkAnimator.SetParameterAutoSend(0, true); // 네트워크 애니메티너 설정
        StartCoroutine(Attack());

        //if (isServer)
		//{
        //    StartCoroutine(Attack());
        //}
    }

    private void OnDisable()
    {
        StopCoroutine(ChangeZombieMat());
        StopCoroutine(Attack());
    }

    private void CheckIfTargetInRange()
	{
		if(targetScript.targetTransform != null && !health.isDie)
		{
            if (targetScript.targetTransform.GetComponentInParent<Player_Health>().GetHP() > 0)
            {
                currentDistance = Vector3.Distance(targetScript.targetTransform.position, myTransform.position);

                if (currentDistance < minDistance && Time.time > nextAttack)
                {
                    nextAttack = Time.time + attackRate;

                    if (leftHandAttack.GetHitCollider() || rightHandAttack.GetHitCollider())
                    {
                        if (!animator.GetBool("Attack"))
                        {
                            animator.SetBool("Attack", true);
                            isDeduct = true;
                        }
                        else
                            animator.SetBool("Attack", false);

                        StartCoroutine(ChangeZombieMat()); //For the host player.
                    }
                    RpcChangeZombieAppearance();
                }
            }
		}
        else
        {
            animator.SetBool("Attack", false);
            isDeduct = false;
        }
	}
        
    private IEnumerator ChangeZombieMat()
	{
        // 매터리얼 색상 변경 대신 애니메이션으로 변경해야 함.
        // 공격중에는 NavMeshAgent Stop했다가 혹은 껏다가 다시 시작

        //GetComponent<Renderer>().material = zombieRed;
        if (health.isDie)
        {
            StopCoroutine(ChangeZombieMat());
            yield return null;
        }
        else
            yield return new WaitForSeconds(0.5f);
        //GetComponent<Renderer>().material = zombieGreen;

        if (isDeduct)
        {
            if (animator.GetCurrentAnimatorStateInfo(0).IsName("Attack"))
            {
                // 조금 더 자연스러운 소리를 내기 위해서 적은 2개의 공격 AS를 사용

                // 공격 소리 재생 중이면 적 공격 AS 2번에서 재생
                if (SoundManager.Instance.IsEfxPlaying((int)ASN.Enemy_Hit_Walk) || animator.GetCurrentAnimatorStateInfo(0).normalizedTime > 0.5f)
                {
                    SoundManager.Instance.PlayEfx((int)ASN.Enemy_Hit_Walk2, SoundControl.Instance.enemyHitEfxClips[0]); // 이펙트 재생
                }

                // 공격 소리가 아니면 적 공격 AS 1번에서 재생
                if (!SoundManager.Instance.IsEfxPlaying((int)ASN.Enemy_Hit_Walk) || animator.GetCurrentAnimatorStateInfo(0).normalizedTime > 0.5f)
                {
                    SoundManager.Instance.PlayEfx((int)ASN.Enemy_Hit_Walk, SoundControl.Instance.enemyHitEfxClips[0]); // 이펙트 재생
                }

                damage = Random.Range(3, 5); // 3~5의 랜덤하게 데미지 주기.
                targetScript.targetScriptTransform.GetComponent<Player_Health>().DeductHealth(damage);

                leftHandAttack.GetComponent<BoxCollider>().enabled = true;
                rightHandAttack.GetComponent<BoxCollider>().enabled = true;

                isDeduct = false;
            }
        }
    }

	[ClientRpc]
    private void RpcChangeZombieAppearance()
	{
		StartCoroutine(ChangeZombieMat());
	}

    private IEnumerator Attack()
	{
        for (;;)
        {
            if (!_gameManager.isSurvived)
                CheckIfTargetInRange();

            if (health.isDie)
            {
                break;
            }
            else
                yield return new WaitForSeconds(attackRate);
        }

        StartCoroutine(waitDestroyObject());
    }

    IEnumerator waitDestroyObject()
    {
        // Death 애니메이션 -1546996312

        if (!isToggleDeathAnim)
        {
            _gameManager.ZombieDeadCounter++; // 좀비 데스 카운터 증가

            animator.SetBool("Death", true);
            isToggleDeathAnim = true;
        }

        // 한프레임 기다렸다가 데스 애니메이션 바로 끄기
        yield return null;
        animator.SetBool("Death", false);

        // 한프레임 기다림.
        yield return null;

        Destroy(this.gameObject, 1.5f);
    }
}
