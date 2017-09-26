using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Attack : MonoBehaviour
{
    public GameObject FX1;
    public GameObject FX2;

    Image Fill;
    AMove player;

    public float upposition = 0.5f;
    public float time;

	// Use this for initialization
	void Start ()
    {
        player = GameObject.Find("Judy").GetComponent<AMove>();
        Fill = GameObject.Find("Fill").GetComponent<Image>();
        time = 3.0f;
    }
	
	// Update is called once per frame
	void Update ()
    {
        time += Time.deltaTime;

        if (time >= 4.0f)
            time = 4.0f;

        Fill.fillAmount = time / 4;

        if (player.ADown() && time >= 4.0f)
        {
            Instantiate(FX1, new Vector3(player.transform.position.x, player.transform.position.y + upposition, player.transform.position.z), transform.rotation);
            time = 0;
        }

        if (player.BDown() && time >= 4.0f)
        {
            Instantiate(FX2, new Vector3(player.transform.position.x, player.transform.position.y + upposition, player.transform.position.z), transform.rotation);
            time = 0;
        }
    }
}