using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
using UnityEngine.UI;

public class Player_Health : NetworkBehaviour {

	[SyncVar (hook = "OnHealthChanged")] private int health = 100;
	private Text healthText;
	public bool shouldDie = false;
	public bool isDead = false;

	public delegate void DieDelegate();
	public event DieDelegate EventDie;

	public delegate void RespawnDelegate();
	public event RespawnDelegate EventRespawn;

    public bool isDieDone = false;
    public bool isRespawnDone = false;

    [SerializeField] Animator animator;
    [SerializeField] CapsuleCollider capsuleCollider;

    public override void OnStartLocalPlayer ()
	{
		healthText = GameObject.Find("Health Text").GetComponent<Text>();

        SetHealthText();
	}

	// Update is called once per frame
	void FixedUpdate() 
	{
        CheckCondition();
    }

	void CheckCondition()
	{
		if(health <= 0 && !shouldDie && !isDead)
		{
            if (healthText)
                healthText.text = "<color=#FF0000>Dead</color>";

            shouldDie = true;
            isDieDone = false;
            isRespawnDone = false;
        }

		if(health <= 0 && shouldDie)
		{
            if (healthText)
                healthText.text = "<color=#FF0000>Dead</color>";

            if (EventDie != null)
			{
				EventDie();
                isDieDone = true;
            }

            //Movement에서 처리
			//shouldDie = false;
		}

		if(health > 0 && isDead)
		{
			if(EventRespawn != null)
			{
				EventRespawn();
                isRespawnDone = true;
            }

            //Movement에서 처리
            //isDead = false;
        }
    }

	void SetHealthText()
	{
		if(isLocalPlayer)
		{
            if (healthText)
                healthText.text = "<color=#FF0000>HP </color>" + "<color=#999999>" + health.ToString() + "</color>";
		}
	}

	public void DeductHealth (int dmg)
	{
        // 재생중이 아니면
        if (!SoundManager.Instance.IsEfxPlaying((int)ASN.Player_Damaged))
            SoundManager.Instance.PlayEfx((int)ASN.Player_Damaged, SoundControl.Instance.playerDamagedEfxClips[0]); // 이펙트 재생

        health -= dmg;
	}

	void OnHealthChanged(int hlth)
	{
		health = hlth;
		SetHealthText();
	}

	public void ResetHealth() //Called Player_Respawn
	{
		health = 100;
	}

    public int GetHP()
    {
        return health;
    }

    public Animator GetAnimator()
    {
        return animator;
    }

    public CapsuleCollider GetCapsuleCollider()
    {
        return capsuleCollider;
    }
}
