using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class HPBar : MonoBehaviour
{
    MoveShooting player;
    Text txt;

    // Use this for initialization
    void Start ()
    {
        player = GameObject.Find("Judy").GetComponent<MoveShooting>();
        txt = this.GetComponentInChildren<Text>();
    }
	
	// Update is called once per frame
	void Update ()
    {
        txt.text = "HP: " + player.cur_hp;
    }
}
