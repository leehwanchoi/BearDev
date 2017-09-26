using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class CreditEnding : MonoBehaviour
{
    EndingManager em;
    Image image;
    Text text;
    byte alpha;
    bool blackon;

    public int txtSpeed = 20;

    // Use this for initialization
    void Start ()
    {
        em = GameObject.Find("DialogueManager").GetComponent<EndingManager>();
        image = GameObject.Find("Black").GetComponent<Image>();
        text = GameObject.Find("Credit_Text").GetComponent<Text>();
    }
	
	// Update is called once per frame
	void Update ()
    {
		if (em.endDialouge)
        {
            alpha++;
            if (alpha >= 200)
            {
                alpha = 200;
                blackon = true;
            }
            image.color = new Color32(0, 0, 0, alpha);

            if (blackon)
            {
                text.transform.Translate(Vector3.up * txtSpeed * Time.deltaTime);
            }
        }
	}
}
