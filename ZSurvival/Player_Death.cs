using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
using UnityEngine.UI;

public class Player_Death : NetworkBehaviour {

    [SerializeField] Player_Health healthScript;
    [SerializeField] CapsuleCollider capsuleCollider;


    public override void PreStartClient ()
	{
		healthScript = GetComponent<Player_Health>();
		healthScript.EventDie += DisablePlayer;
	}
	
	public override void OnNetworkDestroy ()
	{
		healthScript.EventDie -= DisablePlayer;
	}

    private void DisablePlayer ()
	{
        capsuleCollider.enabled = false;

		healthScript.isDead = true;

		//if(isLocalPlayer)
		//{
  //          //GetComponent<Movement>().enabled = false;
  //          // 죽는 애니메이션을 보여줘야하기 때문에 gameObject를 끄지 않는다.
  //          //GetComponentInChildren<CapsuleCollider>().gameObject.SetActive(false);
  //          //healthScript.transform.Find("RangerForLocomotion").gameObject.SetActive(false); // 위에 CapsuleCollider의 gameObject랑 같지만, 찾는 비용이 큼

  //          // Movement에서 해줌
  //          //GameObject.Find("GameManager").GetComponent<GameManager_References>().respawnButton.SetActive(true);
  //      }
	}
}
