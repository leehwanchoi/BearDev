using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class EndingManager : MonoBehaviour
{
    public GameObject imageBox;
    public GameObject textBox;
    public GameObject NPCBox;

    Image JudyImage;
    public Sprite image1;
    public Sprite image2;
    public Sprite image3;
    public Sprite image4;
    public Sprite image5;
    public Sprite image6;

    Image NPCImage;
    public Sprite NPCimage1;
    public Sprite NPCimage2;

    txt_read txt;
    Done_txt doneBox;

    public bool endDialouge;
    int endNum;

    // Use this for initialization
    void Start ()
    {
        endNum = 5;
        txt = GameObject.Find("TextBox_Text").GetComponent<txt_read>();
        doneBox = GameObject.Find("TextDone_Box").GetComponent<Done_txt>();

        JudyImage = GameObject.Find("Judy_2D_01").GetComponent<Image>();
        NPCImage = GameObject.Find("NPC_Image").GetComponent<Image>();

        NPCBox.SetActive(false);
    }
	
	// Update is called once per frame
	void Update ()
    {
        DialougeInit();
        DialougeUpdate();
        endFunc();
    }

    void DialougeInit()
    {
        if (doneBox.isDown)
        {
            //strtext 초기화
            txt.str.text = "";
            txt.count = 0;
            txt.index = 0;
            txt.isEnd = false;
        }
    }

    void endFunc()
    {
        if (endDialouge)
        {
            textBox.SetActive(false);
            imageBox.SetActive(false);
        }
    }

    void changeName(string name)
    {
        GameObject.Find("NameBox_Text").GetComponent<Text>().text = name;
    }

    void DialougeUpdate()
    {
        if (doneBox.DialougeNumber == 0)
        {
            JudyImage.sprite = image2;
            changeName("주디");
            txt.str1 = "괜찮으신가요? 폐하!";
        }

        if (doneBox.DialougeNumber == 1)
        {
            NPCBox.SetActive(true);

            NPCImage.sprite = NPCimage1;

            changeName("폐하");
            txt.str1 = "그래 나는 괜찮다.";
        }

        if (doneBox.DialougeNumber == 2)
        {
            JudyImage.sprite = image1;
            changeName("주디");
            txt.str1 = "다행입니다!";
        }

        if (doneBox.DialougeNumber == 3)
        {
            changeName("폐하");
            txt.str1 = "그대의 공을 높게 평가하네, 그대를 우리왕국의 정식마법사로 발탁하겠네.";
        }

        if (doneBox.DialougeNumber == 4)
        {
            JudyImage.sprite = image4;
            changeName("주디");
            txt.str1 = "감..감사합니다 !";
        }

        if (doneBox.DialougeNumber == endNum)
        {
            endDialouge = true;
        }
    }
}
