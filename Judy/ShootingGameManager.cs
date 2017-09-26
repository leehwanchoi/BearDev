using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ShootingGameManager : MonoBehaviour
{
    public GameObject textBox;
    public GameObject NPC_2DImage;
    public GameObject JoyStick_Button;
    public GameObject NameBox_text;

    TextDone_Button textDone;
    strText strtext;
    public int DialogueNum; //대화 체크

	// Use this for initialization
	void Start ()
    {
        //튜토리얼 대화 실행
        textBox.SetActive(true);
        NPC_2DImage.SetActive(true);

        textDone = GameObject.Find("TextDone_Box").GetComponent<TextDone_Button>();
        strtext = GameObject.Find("TextBox_Text").GetComponent<strText>();

        DialogueNum = 0;
        NameBox_text.GetComponent<Text>().text = "주디";
        strtext.str1 = "마법왕국으로 가려면 고난의 숲을 지나야만 해! \r\n마법지팡이를 타고 숲의 괴물들을 피해가자!";
    }
	
	// Update is called once per frame
	void Update ()
    {
        if (textDone.isDown)
        {
            DialogueNum++;
            NPC_2DImage.SetActive(false);
        }
	}
}
