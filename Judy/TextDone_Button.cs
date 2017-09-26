using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class TextDone_Button : MonoBehaviour, IDragHandler, IPointerDownHandler, IPointerUpHandler
{
    Button button;
    public bool isDown;

    public GameObject textBox;
    public GameObject NPCImageBox;
    strText strtext;

    // Use this for initialization
    void Start ()
    {
        button = GameObject.Find("TextDone_Box").GetComponent<Button>();
        strtext = GameObject.Find("TextBox_Text").GetComponent<strText>();
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

        //strtext 초기화
        strtext.str.text = "";
        strtext.count = 0;
        strtext.index = 0;
        strtext.isEnd = false;

        //strtext 끄기
        textBox.SetActive(false);
        NPCImageBox.SetActive(false);
    }

    public virtual void OnPointerUp(PointerEventData ped)
    {
        isDown = false;
    }
}
