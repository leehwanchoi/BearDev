using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossManager : MonoBehaviour
{
    public GameObject ClearUI;
    public GameObject DieUI;

    AMove player;

    public int enemyDieCount;

    // Use this for initialization
    void Start()
    {
        player = GameObject.Find("Judy").GetComponent<AMove>();
    }

    // Update is called once per frame
    void Update()
    {
        if (enemyDieCount >= 1)
        {
            ClearUI.SetActive(true);
        }

        if (player.cur_hp <= 0)
        {
            DieUI.SetActive(true);
        }
    }
}
