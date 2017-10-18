using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class ButtonA : MonoBehaviour, IDragHandler, IPointerDownHandler, IPointerUpHandler
{
    Button button;
    public bool isDown;
    //public bool isAllow;

	// Use this for initialization
	void Start ()
    {
        button = GameObject.Find("ButtonA").GetComponent<Button>();
    }

    public virtual void OnDrag(PointerEventData ped)
    {

    }

    public virtual void OnPointerDown(PointerEventData ped)
    {
        isDown = true;
    }

    public virtual void OnPointerUp(PointerEventData ped)
    {
        isDown = false;
        //isAllow = false;
    }
}
