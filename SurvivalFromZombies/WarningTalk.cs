using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WarningTalk : MonoBehaviour
{
    private GamePlayManager GPManager;

    public GameObject TalkText;

    private TextMesh textMesh;
    private string Talking;
    private int TalkIndex;

    private float TalkTime;

    private bool isEnd;
    private float AllShowTime;

    private bool isStart;

    private void Awake()
    {
        GPManager = GameObject.Find("GameManager").GetComponent<GamePlayManager>();

        textMesh = this.GetComponentInChildren<TextMesh>();
        textMesh.text = "";

        Talking = "Civilians can't come in.\nGet OUT!";
        TalkIndex = 0;

        TalkTime = 0.0f;

        isEnd = false;
        AllShowTime = 0.0f;

        this.GetComponent<SpriteRenderer>().enabled = false;
        TalkText.SetActive(false);
        isStart = false;
    }

    private void FixedUpdate()
    {
        if (GPManager.CreateState03Enemy.isCollision)
        {
            this.GetComponent<SpriteRenderer>().enabled = true;
            isStart = true;
            TalkText.SetActive(true);
        }

        if (isStart)
        {
            if (!isEnd)
                TalkTime += Time.deltaTime;

            if (!isEnd && TalkTime > 0.2f)
            {
                TalkTime = 0.0f;

                if (TalkIndex < Talking.Length)
                {
                    textMesh.text += Talking[TalkIndex];
                    TalkIndex++;
                }
                else if (TalkIndex == Talking.Length)
                {
                    isEnd = true;
                }
            }

            if (isEnd)
                AllShowTime += Time.deltaTime;

            if (AllShowTime > 5.0f)
            {
                textMesh.text = "";
                TalkIndex = 0;

                isEnd = false;
                AllShowTime = 0.0f;
            }
        }
    }
}
