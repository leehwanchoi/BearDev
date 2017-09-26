using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class Done_txt : MonoBehaviour, IDragHandler, IPointerDownHandler, IPointerUpHandler
{
    Button button;
    public bool isDown;
    public bool isNext;
    public int DialougeNumber;

    // Use this for initialization
    void Start ()
    {
        button = GameObject.Find("TextDone_Box").GetComponent<Button>();
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
        Debug.Log("TextDone_Box");
        isDown = true;
    }

    public virtual void OnPointerUp(PointerEventData ped)
    {
        isDown = false;
        isNext = true;

        DialougeNumber++;
    }
}
