using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class OptionSetting : MonoBehaviour
{
    public Toggle toggle;
    public Toggle toggle2;
    public Slider slider;

    public AudioSource BGM;

    public AudioSource Voice00;
    public AudioSource Voice01;

    private void Awake()
    {
        toggle.isOn = true;
        toggle2.isOn = true;
        slider.value = 1.0f;
    }

    public void OnOffBGM()
    {
        if (!toggle.isOn)
        {
            BGM.Stop();
        }
        else
        {
            BGM.Play();
        }
    }

    public void OnOffVoice()
    {
        if (!toggle2.isOn)
        {
            Voice00.mute = true;
            Voice01.mute = true;
        }
        else
        {
            Voice00.mute = false;
            Voice01.mute = false;
        }
    }

    public void SetSlideVolume()
    {
        BGM.volume = slider.value;
    }

    public void ExitButton()
    {
        Application.Quit();
    }
}