using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class DroneMovement : NetworkBehaviour
{
    [SerializeField] Transform playerTransform;
    [SerializeField] Transform droneTransform;

    private float moveSpeed;

    private Vector3 dest;
    private Vector3 Dir;
    private Vector3 cur;

    [HideInInspector] public bool isDroneAttack;
    private bool isSearchingDone;

    [HideInInspector] public Transform enemy = null;

    private void Start()
    {
        if (!isLocalPlayer)
            return;

        moveSpeed = 2.0f;
        droneTransform.position = new Vector3(droneTransform.position.x, 2.0f, droneTransform.position.z);

        isDroneAttack = false;
        isSearchingDone = false;
    }

    private void FixedUpdate()
    {
        if (!isLocalPlayer)
            return;

        if (isDroneAttack && enemy && Vector3.Distance(enemy.position, playerTransform.position) < 10.0f)
            return;

        if (Vector3.Distance(droneTransform.position, playerTransform.position) > 2.5f)
        {
            dest = new Vector3(playerTransform.position.x, 2.0f, playerTransform.position.z);
            cur = new Vector3(droneTransform.position.x, 2.0f, droneTransform.position.z);
            Dir = dest - cur;

            droneTransform.Translate(Dir * moveSpeed * Time.deltaTime);
            droneTransform.position = new Vector3(droneTransform.position.x, 2.0f, droneTransform.position.z);

            droneTransform.LookAt(playerTransform);
        }
    }

    // Player_SpawnManager에서 사용함.
    [ClientCallback]
    public void BulletLookAtEnemy()
    {
        isDroneAttack = true;

        if (enemy == null || Vector3.Distance(enemy.position, playerTransform.position) > 10.0f)
        {
            GameObject[] gos = GameObject.FindGameObjectsWithTag("Zombie");

            for (int i = 0; i < gos.Length; i++)
            {
                enemy = gos[i].transform;

                if (Vector3.Distance(enemy.position, playerTransform.position) < 10.0f)
                    break;
            }
        }

        droneTransform.LookAt(enemy);
    }

    public Vector3 GetPlayerPos()
    {
        return playerTransform.position;
    }
}