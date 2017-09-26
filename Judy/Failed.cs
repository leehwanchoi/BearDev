using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Failed : MonoBehaviour
{
    public string sceneName;
    public AudioClip fail_Sound;

    void Start()
    {
        GetComponent<AudioSource>().clip = fail_Sound;
        GetComponent<AudioSource>().Play();
    }
	
    public void failed()
    {
        SceneManager.LoadScene(sceneName);
    }
}
