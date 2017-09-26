using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Fade : MonoBehaviour
{
    public byte alpha;

    public bool isEnd;
    public bool isDone;

	// Use this for initialization
	void Start ()
    {
        alpha = 100;
    }
	
	// Update is called once per frame
	void Update ()
    {
        if (!isEnd)
        {
            alpha++;
            if (alpha >= 200)
                alpha = 200;

            this.gameObject.GetComponent<Image>().color = new Color32(0, 0, 0, alpha);
        }
        else
        {
            alpha--;
            if (alpha <= 0)
            {
                alpha = 0;
                isDone = true;
            }

            this.gameObject.GetComponent<Image>().color = new Color32(0, 0, 0, alpha);
        }
	}
}
