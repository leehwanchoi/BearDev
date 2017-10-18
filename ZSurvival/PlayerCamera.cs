using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerCamera : MonoBehaviour
{
    [SerializeField] Transform playerTransfrom;
    [SerializeField] AudioListener audioListener;

    public float dampTime = 0.15f;
    private Vector3 velocity = Vector3.zero;

    private void FixedUpdate()
    {
        Vector3 destination = playerTransfrom.position;
        destination.x = playerTransfrom.position.x;
        destination.y = 7.0f;
        destination.z = playerTransfrom.position.z - 5.5f;
        transform.position = Vector3.SmoothDamp(transform.position, destination, ref velocity, dampTime);
    }
}