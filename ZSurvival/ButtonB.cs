using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class ButtonB : MonoBehaviour, IDragHandler, IPointerDownHandler, IPointerUpHandler
{
    Button button;
    public bool isDown;

    // Use this for initialization
    void Start ()
    {
        button = GameObject.Find("ButtonB").GetComponent<Button>();
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
    }
}
