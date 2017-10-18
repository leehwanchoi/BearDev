using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
using UnityEngine.UI;

public class Player_Respawn : NetworkBehaviour {

    [SerializeField] Player_Health healthScript;
	private GameObject respawnButton;
    [SerializeField] CapsuleCollider capsuleCollider;

    public override void PreStartClient ()
	{
        respawnButton = GameObject.Find("GameManager").GetComponent<GameManager_References>().respawnButton;
        healthScript = GetComponent<Player_Health>();
		healthScript.EventRespawn += EnablePlayer;
	}

	public override void OnStartLocalPlayer ()
	{
		SetRespawnButton();
	}

    private void SetRespawnButton ()
	{
		if(isLocalPlayer)
		{
			//respawnButton = GameObject.Find("GameManager").GetComponent<GameManager_References>().respawnButton;
			respawnButton.GetComponent<Button>().onClick.AddListener(CommenceRespawn);
			respawnButton.SetActive(false);
		}
	}

	public override void OnNetworkDestroy ()
	{
		healthScript.EventRespawn -= EnablePlayer;
	}

    private void EnablePlayer()
	{
        // 애니메이션을 보여줘야 해서 Death에서 끄지 않았기 때문에 할 필요없다.
        //healthScript.transform.Find("RangerForLocomotion").gameObject.SetActive(true);
        capsuleCollider.enabled = true;

		if(isLocalPlayer)
		{
            //GetComponent<Movement>().enabled = true;
            respawnButton.SetActive(false);
		}
	}

    private void CommenceRespawn()
	{
		CmdRespawnOnServer();
	}

	[Command]
    private void CmdRespawnOnServer()
	{
		healthScript.ResetHealth();
	}

}
