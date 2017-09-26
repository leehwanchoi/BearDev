using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class CreditsUP : MonoBehaviour
{
    public int TextSpeed = 20;
    float y;

    public GameObject Credit_BG;

    Menu03Button menu03;
    Fade fade;

    // Use this for initialization
    void Start ()
    {
        menu03 = GameObject.Find("MenuBox").GetComponentInChildren<Menu03Button>();
        fade = GameObject.Find("Credit_BackGround").GetComponent<Fade>();

        this.transform.position = new Vector3(640, -400, 0);
        y = this.transform.position.y;
    }
	
	// Update is called once per frame
	void Update ()
    {
        y++;
        this.transform.position = new Vector3(this.transform.position.x, y, this.transform.position.z);

        if (this.transform.position.y >= 1100)
        {
            fade.isEnd = true;

            if (fade.isDone)
            {
                this.transform.position = new Vector3(640, -400, 0);
                y = this.transform.position.y;
                fade.alpha = 100;
                fade.isEnd = false;
                fade.isDone = false;

                Credit_BG.SetActive(false);
            }
        }
        else if (menu03.isDoubleClick == 0)
        {
            this.transform.position = new Vector3(640, -400, 0);
            y = this.transform.position.y;
            fade.alpha = 100;
            fade.isEnd = false;
            fade.isDone = false;

            Credit_BG.SetActive(false);
        }
    }
}
