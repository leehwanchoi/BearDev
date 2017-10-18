using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class SyncRot : NetworkBehaviour
{
	[SyncVar]
    private Quaternion syncPlayerRotation;
    [SyncVar]
    private Quaternion syncDroneRotation;

    [SerializeField] private Transform playerTransform;
    [SerializeField] private Transform droneTransform;

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
			playerTransform.rotation = Quaternion.Lerp(playerTransform.rotation, syncPlayerRotation, Time.deltaTime * lerpRate);
            droneTransform.rotation = Quaternion.Lerp(droneTransform.rotation, syncDroneRotation, Time.deltaTime * lerpRate);
        }
	}

	[Command]
	void CmdProvideRotationsToServer(Quaternion playerRot, Quaternion droneRot)
	{
		syncPlayerRotation = playerRot;
        syncDroneRotation = droneRot;
    }

	[Client]
	void TransmitRotations()
	{
		if (isLocalPlayer)
		{
			CmdProvideRotationsToServer (playerTransform.rotation, droneTransform.rotation);
		}	
	}
}