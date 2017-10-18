using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class BGMVolume : MonoBehaviour
{
    private Slider slider;

	void Start ()
    {
        slider = this.GetComponent<Slider>();
    }
	
	void FixedUpdate()
    {
        SoundManager.Instance.SetMusicVolume(slider.value);
	}
}