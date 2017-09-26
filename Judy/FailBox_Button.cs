using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using UnityEngine.SceneManagement;

public class FailBox_Button : MonoBehaviour, IDragHandler, IPointerDownHandler, IPointerUpHandler
{
    Button button;
    public bool isDown;

    public string SceneName;
    public GameObject ReadyBox;

    AMove player;

	// Use this for initialization
	void Start ()
    {
        player = GameObject.Find("Judy").GetComponent<AMove>();
        button = this.GetComponent<Button>();
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

        if (this.name == "FailBox_Button_Start")
        {
            SceneManager.LoadScene(SceneName);
        }
    }

    public virtual void OnPointerUp(PointerEventData ped)
    {
        isDown = false;
    }
}
