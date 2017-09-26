using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MenuManager : MonoBehaviour
{
    Menu01Button menu01;
    Menu02Button menu02;
    Menu03Button menu03;

    public GameObject Credit_BG;

    // Use this for initialization
    void Start ()
    {
        menu01 = GameObject.Find("MenuBox").GetComponentInChildren<Menu01Button>();
        menu02 = GameObject.Find("MenuBox").GetComponentInChildren<Menu02Button>();
        menu03 = GameObject.Find("MenuBox").GetComponentInChildren<Menu03Button>();
    }
	
	// Update is called once per frame
	void Update ()
    {
        Menu01Function();
        Menu02Function();
        Menu03Function();
    }

    void Menu01Function()
    {
        if (menu01.isDown)
        {

        }
    }

    void Menu02Function()
    {
        if (menu02.isDown)
        {

        }
    }

    void Menu03Function()
    {
        if (menu03.isDown && menu03.isDoubleClick == 1)
        {
            Credit_BG.SetActive(true);
        }
        else if (menu03.isDoubleClick > 1)
        {
            menu03.isDoubleClick = 0;
        }
    }
}
