using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FXSoundManager : MonoBehaviour
{
    public AudioClip FX1_Sound;
    public AudioClip FX2_Sound;

    AMove player;
    Attack pAttack;

    // Use this for initialization
    void Start ()
    {
        player = GameObject.Find("Judy").GetComponent<AMove>();
        pAttack = GameObject.Find("Judy").GetComponent<Attack>();
    }
	
	// Update is called once per frame
	void Update ()
    {
        if (player.ADown() && pAttack.time >= 4.0f)
        {
            GetComponent<AudioSource>().clip = FX1_Sound;
            if (!GetComponent<AudioSource>().isPlaying)
                GetComponent<AudioSource>().Play();
        }

        if (player.BDown() && pAttack.time >= 4.0f)
        {
            GetComponent<AudioSource>().clip = FX2_Sound;
            if (!GetComponent<AudioSource>().isPlaying)
                GetComponent<AudioSource>().Play();
        }
    }
}
