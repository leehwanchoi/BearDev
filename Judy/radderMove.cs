using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class radderMove : MonoBehaviour
{
    public bool isForward;
    public float startCoor;
    public float endCoor;
    public float speed = 1;

    public Vector3 movement;

    void Start()
    {
        movement = Vector3.forward;
    }

	void Update ()
    {
        if (isForward)
        {
            if (transform.position.x < startCoor && transform.position.x < endCoor)
            {
                movement = Vector3.back;
            }
            else if (transform.position.x > endCoor && transform.position.x > startCoor)
            {
                movement = Vector3.forward;
            }
        }
        else
        {
            if (transform.position.z < startCoor && transform.position.z < endCoor)
            {
                movement = Vector3.back;
            }
            else if (transform.position.z > endCoor && transform.position.z > startCoor)
            {
                movement = Vector3.forward;
            }
        }

        transform.Translate(movement * speed * Time.deltaTime);
    }
}
