using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class PuzzleR : MonoBehaviour, IDragHandler, IPointerDownHandler, IPointerUpHandler
{
    Button button;
    public bool isDown;
    public bool isAllow;

	// Use this for initialization
	void Start ()
    {
        button = GameObject.Find("PuzzleR").GetComponent<Button>();
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
        Debug.Log("PuzzleR");
        isDown = true;
    }

    public virtual void OnPointerUp(PointerEventData ped)
    {
        isDown = false;
        isAllow = false;
    }
}
