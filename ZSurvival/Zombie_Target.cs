using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
using UnityEngine.AI;

public class Zombie_Target : NetworkBehaviour {

	private NavMeshAgent agent;
	private Transform myTransform;
	public Transform targetTransform;
    public Transform targetScriptTransform;
	private LayerMask raycastLayer;
	private float radius = 100;

    private float serachingRange = 15.0f; // 탐지범위

    [SerializeField] Animator animator;

    private GameManager_References _gameManager;

    // Use this for initialization
    void Start () 
	{
        _gameManager = GameObject.Find("GameManager").GetComponent<GameManager_References>();

        agent = GetComponent<NavMeshAgent>();
		myTransform = transform;
		raycastLayer = 1<<LayerMask.NameToLayer("Player");

		if(isServer)
		{
			StartCoroutine(DoCheck());
		}
    }

    private void OnDisable()
    {
        StopCoroutine(DoCheck());
    }

	void SearchForTarget()
	{
		if(!isServer)
		{
			return;
		}

        // 타겟이 없으면
		if(targetTransform == null)
		{
			Collider[] hitColliders = Physics.OverlapSphere(myTransform.position, radius, raycastLayer);

			if(hitColliders.Length>0)
			{
				int randomint = Random.Range(0, hitColliders.Length);
				targetTransform = hitColliders[randomint].transform;

                // 이건 어택에서 필요한 것
                targetScriptTransform = targetTransform.GetComponentInParent<Player_Health>().transform;
            }
		}

        // 타겟이 죽었으면
		if(targetTransform != null && targetTransform.GetComponent<CapsuleCollider>().enabled == false)
		{
			targetTransform = null;
		}

        // 죽지않은 상태에서 타겟이 있을때
        if (targetTransform != null && targetTransform.GetComponent<CapsuleCollider>().enabled == true)
        {
            // 탐지범위 밖이면
            if (Vector3.Distance(targetTransform.position, myTransform.position) > serachingRange)
            {
                // agent 멈추고
                agent.isStopped = true;

                // 초기화
                targetTransform = null;
                targetScriptTransform = null;
            }
        }
	}

	void MoveToTarget()
	{
		if(targetTransform != null && isServer)
		{
            this.GetComponent<NetworkAnimator>().animator.SetBool("Attack", false);
            SetNavDestination(targetTransform);
		}
	}

	void SetNavDestination(Transform dest)
	{
        // 멈춰 있으면
        if (agent.isStopped)
            agent.isStopped = false;

        if (agent)
		    agent.SetDestination(dest.position);
	}

	IEnumerator DoCheck()
	{
		for(;;)
		{
            if (!_gameManager.isSurvived)
            {
                SearchForTarget();
                MoveToTarget();
            }

			yield return new WaitForSeconds(2.5f);
		}
	}

}
