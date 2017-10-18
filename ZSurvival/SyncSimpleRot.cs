using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class SyncSimpleRot : NetworkBehaviour
{
	[SyncVar] private Quaternion TargetSyncRotation;

	[HideInInspector] public Transform TargetTransform;
	[SerializeField] private float lerpRate = 10.0f;

	void FixedUpdate()
	{
		TransmitRotations ();
		LerpRotations ();
	}

	void LerpRotations()
	{
		if (!isLocalPlayer)
		{
            TargetTransform.rotation = Quaternion.Lerp (TargetTransform.rotation, TargetSyncRotation, Time.deltaTime * lerpRate);
		}
	}

	[Command]
	void CmdProvideRotationsToServer(Quaternion Rot)
	{
        TargetSyncRotation = Rot;
	}

	[Client]
	void TransmitRotations()
	{
		if (isLocalPlayer)
		{
			CmdProvideRotationsToServer (TargetTransform.rotation);
		}	
	}
}