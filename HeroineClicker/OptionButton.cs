using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OptionButton : MonoBehaviour
{
    public GameObject Target;

    public AudioSource audioSource;
    public AudioClip audioClip;

    private bool isClick;

    private void Awake()
    {
        isClick = false;
        Target.SetActive(false);
    }

    public void ToggleActive()
    {
        if (!audioSource.isPlaying)
        {
            audioSource.clip = audioClip;
            audioSource.Play();
        }

        isClick = !isClick;

        Target.SetActive(isClick);
    }
}