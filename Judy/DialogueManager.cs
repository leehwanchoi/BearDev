using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class DialogueManager : MonoBehaviour
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

    public string sceneName;

    txt_read txt;
    Done_txt doneBox;

    bool endDialouge;
    int endNum;

    // Use this for initialization
    void Start ()
    {
        endNum = 9;
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

            SceneManager.LoadScene(sceneName);
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
            changeName("주디");
            txt.str1 = "휴 통과했다! 이제 어디로 가야하지?";
        }

        if (doneBox.DialougeNumber == 1)
        {
            NPCBox.SetActive(true);

            changeName("시녀");
            txt.str1 = "늦으셨군요... 성안이 악마의 기운에 휩싸이고 있습니다.";
        }

        if (doneBox.DialougeNumber == 2)
        {
            JudyImage.sprite = image2;
            changeName("주디");
            txt.str1 = "그게 무슨말이에요!? 무슨일이 일어난거죠?";
        }

        if (doneBox.DialougeNumber == 3)
        {
            changeName("시녀");
            txt.str1 = "폐하께서 죽음의 신 '하데스'에게 납치당하셨습니다...";
        }

        if (doneBox.DialougeNumber == 4)
        {
            changeName("주디");
            txt.str1 = "이.. 이럴수가... 제가 폐하를 구출하러 가겠습니다!";
        }

        if (doneBox.DialougeNumber == 5)
        {
            changeName("시녀");
            txt.str1 = "혼자서는 힘드실수 있습니다..";
        }

        if (doneBox.DialougeNumber == 6)
        {
            changeName("시녀");
            txt.str1 = "직접 폐하를 만나신적이 없으시죠?";
        }

        if (doneBox.DialougeNumber == 7)
        {
            NPCImage.sprite = NPCimage2;
            changeName("시녀");
            txt.str1 = "폐하께서는 이렇게 생기셨습니다. 꼭 구출하셔서 이 곳의 평화를 내려주세요...";
        }

        if (doneBox.DialougeNumber == 8)
        {
            JudyImage.sprite = image4;
            changeName("주디");
            txt.str1 = "네! 꼭 폐하를 구출하겠습니다!";
        }

        if (doneBox.DialougeNumber == endNum)
        {
            endDialouge = true;
        }
    }
}
