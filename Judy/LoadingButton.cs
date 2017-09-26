using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class LoadingButton : MonoBehaviour, IDragHandler, IPointerDownHandler, IPointerUpHandler
{
    Button button;
    public bool isDown;

	// Use this for initialization
	void Start ()
    {
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
    }

    public virtual void OnPointerUp(PointerEventData ped)
    {
        isDown = false;
    }
}
