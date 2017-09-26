using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class txt_read : MonoBehaviour
{
    public Text str;
    public string str1;

    public int TextSpeed = 20;
    public int count;
    public int index;

    public bool isEnd;

    // Use this for initialization
    void Start()
    {
        str.text = "";

        count = 0;
        index = 0;

        isEnd = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (!isEnd)
            count++;

        if (count % (100 / TextSpeed) == 0 && !isEnd)
        {
            count = 0;
            str.text += str1[index];
            index++;

            if (index == str1.Length)
            {
                isEnd = true;

                index = str1.Length - 1;
            }
        }
    }
}
