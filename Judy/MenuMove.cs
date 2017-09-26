using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MenuMove : MonoBehaviour
{
    MenuButton menuButton;
    Vector3 movement;
    public float menu_Speed = 1000;
    public GameObject MenuObject;

    public bool isDone;

    void Start ()
    {
        menuButton = GameObject.Find("Menu").GetComponent<MenuButton>();

        this.transform.position = new Vector3(this.transform.position.x, 1050, this.transform.position.z);
        movement = Vector3.down;
        isDone = true;

    }
	
	void Update ()
    {
        this.transform.position += movement * menu_Speed * Time.deltaTime;

        if (this.transform.position.y <= 490 && menuButton.m_State == MenuState.Click)
        {
            this.transform.position = new Vector3(this.transform.position.x, 490, this.transform.position.z);
        }

        if (this.transform.position.y >= 1050 && menuButton.m_State == MenuState.NULL)
        {
            this.transform.position = new Vector3(this.transform.position.x, 1050, this.transform.position.z);
            isDone = true;
        }
        else isDone = false;

        if (!menuButton.isDown)
        {
            if (menuButton.m_State == MenuState.NULL && !isDone)
                movement = Vector3.up;

            if (menuButton.m_State == MenuState.Click && !isDone)
                movement = Vector3.down;

            if (isDone)
                MenuObject.SetActive(false);
        }
    }
}
