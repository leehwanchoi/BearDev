using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;

public class Player_SpawnManager : NetworkBehaviour
{
    private ButtonA buttonA;
    private ButtonB buttonB;

    [SerializeField] Transform droneShotPoint;

    [SerializeField] GameObject bulletPrefab;
    [HideInInspector] private int fullBulletNum;
    [HideInInspector] private int curBulletNum;
    private bool isDroneCoolTime;
    private float CoolTime;
    private float ReloadTime;

    private Image coolTimeImage;
    private Text coolTimeText;

    [SerializeField] Player_Health health;

    void Start ()
    {
        if (!isLocalPlayer)
            return;

        buttonA = GameObject.Find("ButtonA").GetComponent<ButtonA>();
        buttonB = GameObject.Find("ButtonB").GetComponent<ButtonB>();

        coolTimeImage = GameObject.Find("CoolTime").GetComponent<Image>();
        coolTimeText = coolTimeImage.GetComponentInChildren<Text>();
        coolTimeImage.fillAmount = 0;
        coolTimeText.text = "";

        ReloadTime = 5.0f;
        isDroneCoolTime = false;
        CoolTime = 0.0f;

        fullBulletNum = 50; // 총 총알 갯수
        curBulletNum = 50; // 현재 총알 갯수
    }
	
	void FixedUpdate()
    {
        if (!isLocalPlayer)
            return;

        //// 로컬이 아니거나 죽었으면 나가기.
        //if (health.shouldDie || health.isDead)
        //    return;

        if (buttonA.isDown)
        {

        }

        if ((buttonB.isDown && curBulletNum > 0) || (Input.GetKey(KeyCode.X) && curBulletNum > 0))
        {
            // 드론이 근처 적을 찾아서 위치를 잡아줌.
            droneShotPoint.GetComponentInParent<DroneMovement>().BulletLookAtEnemy(); // 이곳에서 isDroneAttack = true 해줌.

            // 쿨타임 아니면
            if (!isDroneCoolTime)
            {
                if (droneShotPoint.GetComponentInParent<DroneMovement>().enemy)
                {
                    if (Vector3.Distance(droneShotPoint.GetComponentInParent<DroneMovement>().enemy.position, droneShotPoint.GetComponentInParent<DroneMovement>().GetPlayerPos()) < 10.0f)
                    {
                        CmdSpawnBullet(droneShotPoint.position, droneShotPoint.rotation);

                        if (!SoundManager.Instance.IsEfxPlaying((int)ASN.UI_ETC) ||
                           (SoundManager.Instance.IsEfxPlaying((int)ASN.UI_ETC) && SoundManager.Instance.GetEfxCurClipTime((int)ASN.UI_ETC) > 0.1f))
                            SoundManager.Instance.PlayEfx((int)ASN.UI_ETC, SoundControl.Instance.UI_ETC[0]); // 이펙트 재생

                        curBulletNum--;

                        if (curBulletNum == 0)
                        {
                            isDroneCoolTime = true;

                            curBulletNum = fullBulletNum;
                        }
                    }
                }
            }
        }
        else
        {
            // 드론 어택 해제
            droneShotPoint.GetComponentInParent<DroneMovement>().isDroneAttack = false;
        }

        //if (!isClient || isServer || isLocalPlayer)
        //    return;

        // 쿨타임 체크
        CheckCoolTime();
    }

    [Command]
    void CmdSpawnBullet(Vector3 pos, Quaternion rot)
    {
        GameObject go = Instantiate(bulletPrefab, pos, rot);
        NetworkServer.Spawn(go);
    }

    [ClientCallback]
    void CheckCoolTime()
    {
        // 쿨타임 체크
        if (isDroneCoolTime)
        {
            SoundManager.Instance.SetEfxVolume((int)ASN.UI_ETC, 0.5f);

            if (!SoundManager.Instance.IsEfxPlaying((int)ASN.UI_ETC))
                SoundManager.Instance.PlayEfx((int)ASN.UI_ETC, SoundControl.Instance.UI_ETC[1]); // 이펙트 재생

            CoolTime += Time.deltaTime;

            coolTimeImage.fillAmount = (CoolTime / ReloadTime);
            coolTimeText.text = "Reloading...\n" + ((CoolTime / ReloadTime) * 100).ToString("N1") + "%";

            if (CoolTime > ReloadTime)
            {
                CoolTime = 0.0f;
                isDroneCoolTime = false;
                coolTimeImage.fillAmount = 0;
                coolTimeText.text = "";

                SoundManager.Instance.SetEfxVolume((int)ASN.UI_ETC, 1.0f);
            }
        }
    }
}