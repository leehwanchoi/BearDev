using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ActiveToggle : MonoBehaviour
{
    public GameObject Target;
    public GameObject OtherTarget;
    public GameObject OtherTarget2;
    public GameObject OtherTarget3;

    public AudioSource audioSource;

    public AudioClip[] audioClips;

    public GameObject Content;
    private bool isClick;

    private void Awake()
    {
        isClick = false;
        Target.SetActive(false);
    }

    public void ToggleActive()
    {
        if (OtherTarget.activeSelf == false && OtherTarget2.activeSelf == false && OtherTarget3.activeSelf == false)
        {
            if (!audioSource.isPlaying)
            {
                //0: 클릭소리
                audioSource.clip = audioClips[0];
                audioSource.Play();
            }

            isClick = !isClick;

            if (Target.activeSelf && isClick == false)
                Content.transform.localPosition = Vector3.zero;

            Target.SetActive(isClick);
        }
    }
}