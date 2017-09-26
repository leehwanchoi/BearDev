using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using UnityEngine.SceneManagement;

public class AReadyBox_Button : MonoBehaviour, IDragHandler, IPointerDownHandler, IPointerUpHandler
{
    Button button;
    public bool isDown;

    public string SceneName;
    public GameObject ReadyBox;

    public AudioClip Clear_Sound;

	// Use this for initialization
	void Start ()
    {
        button = this.GetComponent<Button>();

        GetComponent<AudioSource>().clip = Clear_Sound;
        GetComponent<AudioSource>().Play();
    }
	
	// Update is called once per frame
	void Update ()
    {

    }

    public virtual void OnDrag(PointerEventData ped)
    {

    }

    public virtual void OnPointerDown(PointerEventData ped)
    {
        Debug.Log(button.name);
        isDown = true;

        if (this.name == "ReadyBox_Button_Start")
        {
            SceneManager.LoadScene(SceneName);
        }

        if (this.name == "ReadyBox_Button_Cancel")
            ReadyBox.SetActive(false);
    }

    public virtual void OnPointerUp(PointerEventData ped)
    {
        isDown = false;
    }
}
