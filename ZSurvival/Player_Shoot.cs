using UnityEngine;
using System.Collections;
using UnityEngine.Networking;

public class Player_Shoot : NetworkBehaviour {

    private VirtualJoystick joyStick;

    private float range = 2.0f;
	[SerializeField] Transform playerTransform;
    [SerializeField] Movement movement;
    [SerializeField] Player_SpearAttack spearAttack;
	private RaycastHit hit;
    private Collider hitCollider = null;

    private bool isOneAttack = true;
    private bool isTwoAttack = true;
    private bool isThreeAttack = true;

    private bool isOneDeduct = true;
    private bool isTwoDeduct = true;
    private bool isThreeDeduct = true;

    private bool isThreeRay = true;

    [SerializeField] GameObject bloodingParticle;

    private void Awake()
    {
        joyStick = GameObject.Find("JoyStick").GetComponent<VirtualJoystick>();
    }

    // Use this for initialization
    void Start ()
    {
        if (!isLocalPlayer)
            return;

    }
	
	// Update is called once per frame
	void FixedUpdate() 
	{
		CheckIfShooting();
	}

	void CheckIfShooting()
	{
		if(!isLocalPlayer)
			return;

        EfxPlay(); // 이펙트사운드만 따로 재생

        if (movement.isAttacking) // 공격중이면
        {
            Shoot();
        }

        // 공격 애니메이션 중이 아니면 공격횟수 초기화
        if (movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash != 826796465 &&
            movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash != -1471236085 &&
            movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash != -548829027)
        {
            isOneAttack = true;
            isTwoAttack = true;
            isThreeAttack = true;

            isOneDeduct = true;
            isTwoDeduct = true;
            isThreeDeduct = true;

            isThreeRay = true;

            // 무기 콜리더 다시 켜기
            spearAttack.GetComponent<BoxCollider>().enabled = true;

            // 적 타겟 초기화
            spearAttack.InitHitCollider();
            hitCollider = null;

            if (joyStick.isDown && !movement.isAttacking)
            {
                // 걷기 소리 재생
                if (!SoundManager.Instance.IsEfxPlaying((int)ASN.Player_Hit_Walk))
                    SoundManager.Instance.PlayEfx((int)ASN.Player_Hit_Walk, SoundControl.Instance.playerHitEfxClips[4]);
            }
        }
    }

	void Shoot()
	{
        //Drone bullet은 1 Damage
        //Zombie Attack은 3 Damage ~5 Damage
        //Attack01일때는 5 Damage, Attack02일때는 35 Damage, Attack03일때는 100 Damage

        // 01 애니메이션
        if (movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash == 826796465)
        {
            if (spearAttack.GetHitCollider())
            {
                if (spearAttack.GetHitCollider().tag == "Zombie")
                {
                    if (isOneDeduct)
                    {
                        // 혈흔 효과
                        GameObject ptc = Instantiate(bloodingParticle, spearAttack.GetHitCollider().transform.position, spearAttack.GetHitCollider().transform.rotation);

                        string uIdentity = spearAttack.GetHitCollider().transform.name;
                        CmdTellServerWhichZombieWasShot(uIdentity, 5); // 5 데미지

                        //Debug.Log(spearAttack.GetHitCollider().tag + " 5 데미지");
                        isOneDeduct = false;
                    }
                }
            }

            // 어택 초기화
            movement.isAttacking = false;

            // 무기 콜리더 다시 켜기
            spearAttack.GetComponent<BoxCollider>().enabled = true;

            // 적 타겟 초기화
            spearAttack.InitHitCollider();
        }

        // 02 애니메이션
        if (movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash == -1471236085)
        {
            if (spearAttack.GetHitCollider())
            {
                if (spearAttack.GetHitCollider().tag == "Zombie")
                {
                    if (isTwoDeduct)
                    {
                        // 혈흔 효과
                        GameObject ptc = Instantiate(bloodingParticle, spearAttack.GetHitCollider().transform.position, spearAttack.GetHitCollider().transform.rotation);

                        string uIdentity = spearAttack.GetHitCollider().transform.name;
                        CmdTellServerWhichZombieWasShot(uIdentity, 35); // 35 데미지

                        //Debug.Log(spearAttack.GetHitCollider().tag + " 35 데미지");
                        isTwoDeduct = false;
                    }
                }
            }

            // 어택 초기화
            movement.isAttacking = false;

            // 무기 콜리더 다시 켜기
            spearAttack.GetComponent<BoxCollider>().enabled = true;

            // 적 타겟 초기화
            spearAttack.InitHitCollider();
        }

        // 03 애니메이션
        if (movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash == -548829027)
        {
            // 소리를 두개 이상 사용해서 isAttacking 체크하는 밖에서 설정

            if (isThreeRay)
            {
                if (Physics.Raycast(playerTransform.position, playerTransform.forward, out hit))
                {
                    hitCollider = hit.collider;
                }

                if (spearAttack.GetHitCollider() || hitCollider)
                {
                    if (spearAttack.GetHitCollider())
                    {
                        if (spearAttack.GetHitCollider().tag == "Zombie")
                        {
                            if (isThreeDeduct)
                            {
                                // 혈흔 효과
                                GameObject ptc = Instantiate(bloodingParticle, spearAttack.GetHitCollider().transform.position, spearAttack.GetHitCollider().transform.rotation);

                                string uIdentity = spearAttack.GetHitCollider().transform.name;
                                CmdTellServerWhichZombieWasShot(uIdentity, 100); // 100 데미지

                                //Debug.Log(spearAttack.GetHitCollider().tag + " 100 데미지");
                                isThreeDeduct = false;
                                isThreeRay = false;
                            }
                        }
                    }

                    if (hitCollider)
                    {
                        if (hitCollider.tag == "Zombie")
                        {
                            if (isThreeDeduct)
                            {
                                // 혈흔 효과
                                GameObject ptc = Instantiate(bloodingParticle, hitCollider.transform.position, hitCollider.transform.rotation);

                                string uIdentity = hitCollider.transform.name;
                                CmdTellServerWhichZombieWasShot(uIdentity, 100); // 100 데미지

                                //Debug.Log(hitCollider.tag + " 100 데미지");
                                isThreeDeduct = false;
                                isThreeRay = false;
                            }
                        }
                    }
                }
            }

            // 어택 초기화
            movement.isAttacking = false;

            // 무기 콜리더 다시 켜기
            spearAttack.GetComponent<BoxCollider>().enabled = true;

            // 적 타겟 초기화
            spearAttack.InitHitCollider();
            hitCollider = null;
        }
	}

    private void EfxPlay()
    {
        // 01 애니메이션
        if (movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash == 826796465)
        {
            // 1번 공격이고, 이펙트가 재생중이 아니거나
            if (isOneAttack && !SoundManager.Instance.IsEfxPlaying((int)ASN.Player_Hit_Walk))
            {
                SoundManager.Instance.PlayEfx((int)ASN.Player_Hit_Walk, SoundControl.Instance.playerHitEfxClips[0]); // 이펙트 재생
                isOneAttack = false;
            }

            if (spearAttack.GetHitCollider())
            {
                if (spearAttack.GetHitCollider().tag == "Zombie")
                {
                    // 적 피격 SFX가 재생중이 아니고, 애니메이션이 끝나기 전이면
                    if (!SoundManager.Instance.IsEfxPlaying((int)ASN.Enemy_Damaged) && movement.animator.GetCurrentAnimatorStateInfo(0).normalizedTime < 0.8f)
                        SoundManager.Instance.PlayEfx((int)ASN.Enemy_Damaged, SoundControl.Instance.enemyDamagedEfxClips[0]); // 이펙트 재생
                }
            }
        }

        // 02 애니메이션
        if (movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash == -1471236085)
        {
            // 2번 공격이고, SFX가 재생중이 아니면
            if (isTwoAttack && !SoundManager.Instance.IsEfxPlaying((int)ASN.Player_Hit_Walk))
            {
                SoundManager.Instance.PlayEfx((int)ASN.Player_Hit_Walk, SoundControl.Instance.playerHitEfxClips[1]); // 이펙트 재생
                isTwoAttack = false;
            }

            if (spearAttack.GetHitCollider())
            {
                if (spearAttack.GetHitCollider().tag == "Zombie")
                {
                    // 2번 애니 적 피격 SFX가 재생중이 아니면
                    if (!SoundManager.Instance.IsEfxPlaying((int)ASN.Enemy_Damaged) && !SoundManager.Instance.IsSameEfx((int)ASN.Enemy_Damaged, SoundControl.Instance.enemyDamagedEfxClips[1]))
                        SoundManager.Instance.PlayEfx((int)ASN.Enemy_Damaged, SoundControl.Instance.enemyDamagedEfxClips[1]); // 이펙트 재생
                }
            }
        }

        // 3번 공격이고, 이펙트가 재생중이 아니면
        if (isThreeAttack)
        {
            // 3번 애니메이션 중이고
            if (movement.animator.GetCurrentAnimatorStateInfo(0).fullPathHash == -548829027)
            {
                // 3번 공격 애니메이션이 발차기 동작이 재생중이면
                if (movement.animator.GetCurrentAnimatorStateInfo(0).normalizedTime < 0.2f)
                {
                    // 재생중이 아니고, 발차기 소리가 아니면
                    if (!SoundManager.Instance.IsEfxPlaying((int)ASN.Player_Hit_Walk))
                    {
                        SoundManager.Instance.PlayEfx((int)ASN.Player_Hit_Walk, SoundControl.Instance.playerHitEfxClips[3]); // 이펙트 재생
                    }
                }

                // 현재 애니메이션이 중간이상 재생됬으면
                if (movement.animator.GetCurrentAnimatorStateInfo(0).normalizedTime > 0.45f)
                {
                    // 맞은 적 콜리더가 있으면
                    if (spearAttack.GetHitCollider() && spearAttack.GetHitCollider().tag == "Zombie")
                    {
                        // 3번 애니 적 피격 SFX가 재생중이 아니면
                        if (!SoundManager.Instance.IsEfxPlaying((int)ASN.Enemy_Damaged) && !SoundManager.Instance.IsSameEfx((int)ASN.Enemy_Damaged, SoundControl.Instance.enemyDamagedEfxClips[2]))
                            SoundManager.Instance.PlayEfx((int)ASN.Enemy_Damaged, SoundControl.Instance.enemyDamagedEfxClips[2]); // 이펙트 재생
                    }

                    // 발차기 소리 무시하고 3번 공격 애니메이션이 창 사용 동작 소리 재생
                    SoundManager.Instance.PlayEfx((int)ASN.Player_Hit_Walk, SoundControl.Instance.playerHitEfxClips[2]); // 이펙트 재생
                    isThreeAttack = false;
                }
            }
        }
    }

	[Command]
	void CmdTellServerWhoWasShot (string uniqueID, int dmg)
	{
		GameObject go = GameObject.Find(uniqueID);
		go.GetComponent<Player_Health>().DeductHealth(dmg);
	}

	[Command]
	void CmdTellServerWhichZombieWasShot (string uniqueID, int dmg)
	{
		GameObject go = GameObject.Find(uniqueID);
		go.GetComponent<Zombie_Health>().DeductHealth(dmg);
	}

}
