using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class Raycast1 : MonoBehaviour
{
    public float upPosition = 0.5f;
    public float Range = 3.0f;
    public RaycastHit hit;
    Knight player;
    Vector3 Ray_position;

    strText strtext;

    void Start()
    {
        player = GameObject.Find("Judy").GetComponent<Knight>();

        Ray_position = new Vector3(this.transform.position.x, this.transform.position.y + upPosition, this.transform.position.z);
    }

	// Update is called once per frame
	void Update ()
    {
        Ray_position = new Vector3(this.transform.position.x, this.transform.position.y + upPosition, this.transform.position.z);
        Debug.DrawRay(Ray_position, -this.transform.up * Range, Color.red);

        if (Physics.Raycast(Ray_position, -transform.up, out hit, Range))
        {
            if (hit.collider.name == "fbPlanks1x")
            {
                Debug.Log(hit.collider.name);

                player.isRadder = true;
                player.isForward = hit.collider.GetComponent<radderMove>().isForward;
                player.startCoor = hit.collider.GetComponent<radderMove>().startCoor;
                player.endCoor = hit.collider.GetComponent<radderMove>().endCoor;

                if (hit.collider.GetComponent<radderMove>().isForward)
                {
                    if (hit.collider.GetComponent<radderMove>().movement == Vector3.forward)
                        player.movement = Vector3.forward;
                    else player.movement = Vector3.back;

                    player.transform.position = hit.collider.transform.position;
                }
                else
                {
                    if (hit.collider.GetComponent<radderMove>().movement == Vector3.forward)
                        player.movement = Vector3.left;
                    else player.movement = Vector3.right;

                    player.transform.position = hit.collider.transform.position;
                }
            }
            else if (hit.collider.name == "ClearBox")
            {
                player.isClear = true;
            }
            else if (hit.collider.name == "fbBrickFloor_A" || hit.collider.name == "fbBrickFloor_B" || hit.collider.name == "fbBrickFloorTrap")
            {
                player.isRadder = false;
                player.isForward = false;

                player.transform.position = new Vector3(hit.collider.transform.position.x, hit.collider.transform.position.y + 0.45f, hit.collider.transform.position.z);
            }
            else
            {
                player.isRadder = false;
                player.isForward = false;
            }

            if (hit.collider.name == "fbBrickBars")
            {
                player.isRadder = false;
                player.isForward = false;

                player.dontMove = true;
            }
            else
                player.dontMove = false;

            if (hit.collider.name == "frontdontmove")
            {
                player.frontdontMove = true;
                player.isRadder = false;
            }
            else player.frontdontMove = false;
        }
    }
}