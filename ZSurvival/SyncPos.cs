using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class SyncPos : NetworkBehaviour
{
	[SyncVar]
	private Vector3 mysyncPos;
    [SyncVar]
    private Vector3 namesyncPos;
    [SyncVar]
    private Vector3 dronesyncPos;

    [SerializeField] Transform myTransform;
    [SerializeField] Transform nameTransform;
    [SerializeField] Transform droneTransform;
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
            myTransform.position = Vector3.Lerp(myTransform.position, mysyncPos, Time.deltaTime * lerpRate);
            nameTransform.position = Vector3.Lerp(nameTransform.position, namesyncPos, Time.deltaTime * lerpRate);
            droneTransform.position = Vector3.Lerp(droneTransform.position, dronesyncPos, Time.deltaTime * lerpRate);
        }
	}

	[Command]
	void CmdProvidePositiontoServer (Vector3 myPos, Vector3 namePos, Vector3 dronePos)
	{
        mysyncPos = myPos;
        namesyncPos = namePos;
        dronesyncPos = dronePos;
    }

	[ClientCallback]
	void TransmitPosition()
	{
		if (isLocalPlayer)
		{
			CmdProvidePositiontoServer (myTransform.position, nameTransform.position, droneTransform.position);
		}
	}
}
