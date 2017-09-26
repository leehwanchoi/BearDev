using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public GameObject ReadyBox;
    public GameObject ClearBox;
    Knight player;

    // Use this for initialization
    void Start ()
    {
        player = GameObject.Find("Judy").GetComponent<Knight>();
	}
	
	// Update is called once per frame
	void Update ()
    {
        if (player.isDead)
            ReadyBox.SetActive(true);

        if (player.isClear)
            ClearBox.SetActive(true);
    }
}
