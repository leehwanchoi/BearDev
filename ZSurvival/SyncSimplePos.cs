using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class SyncSimplePos : NetworkBehaviour
{
	[SyncVar]
	private Vector3 TargetSyncPos;

    [HideInInspector] public Transform TargetTransform;
	[SerializeField] float lerpRate = 10.0f;

	void FixedUpdate()
	{
		TransmitPosition ();
		LerpPosition ();
	}

	void LerpPosition()
	{
		if (!isLocalPlayer)
        {
            TargetTransform.position = Vector3.Lerp(TargetTransform.position, TargetSyncPos, Time.deltaTime * lerpRate);
        }
	}

	[Command]
	void CmdProvidePositiontoServer (Vector3 Pos)
	{
        TargetSyncPos = Pos;
    }

	[ClientCallback]
	void TransmitPosition()
	{
		if (isLocalPlayer)
		{
			CmdProvidePositiontoServer (TargetTransform.position);
		}
	}
}
