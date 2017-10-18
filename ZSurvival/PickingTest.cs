using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.Networking;

public class PickingTest : NetworkBehaviour
{
    private NavMeshAgent navMeshAgent;
    private Vector3 startPos;
    private Vector3 destPos;

    private void Awake()
    {
        navMeshAgent = this.GetComponentInChildren<NavMeshAgent>();
    }

    private void FixedUpdate()
    {
		if (!isLocalPlayer)
			return;

        if (Input.GetKey(KeyCode.Mouse0))
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

            RaycastHit hit;
            if (Physics.Raycast(ray, out hit) /*&& hit.transform.tag == "Ground"*/)
            {
                startPos = this.transform.position;
                destPos = hit.point;
            }
        }

        navMeshAgent.SetDestination(destPos);
    }
}