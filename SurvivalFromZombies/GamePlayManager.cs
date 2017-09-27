using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public enum StageState
{
    None,
    Stage01,
    Stage02,
    Stage03,
    Stage04,
    NextScene,
    Dead
}

public class GamePlayManager : MonoBehaviour
{
    private AudioSource audioSource_UI;     //UI 소리
    public AudioClip[] audioClips;

    [HideInInspector] public StageState stageState;

    public CollisionCheck[] StartTrigger;
    public CollisionCheck CreateState02Enemy;
    public CollisionCheck CreateState03Enemy;
    public CollisionCheck CreateState04;

    public CollisionCheck Stage01ClearCollision;
    public CollisionCheck Stage02ClearCollision;
    public CollisionCheck Stage03ClearCollision;
    public GameObject TextBoxUI;
    private float TextBoxUICount;

    public int DeadStage01Enemy;
    public int DeadStage02Enemy;
    public int DeadStage03Enemy;

    public GameObject TimeLine;
    public TimeLineAnimation timeLineAnimation;

    public GameObject EndingUI;
    public GameObject EyesUI;
    private Image BlackTop;
    private Image BlackDown;
    private float EndingCheckTime;

    private void Awake()
    {
        audioSource_UI = GameObject.Find("UI").GetComponent<AudioSource>();

        stageState = StageState.None;

        TextBoxUI.SetActive(false);
        TextBoxUICount = 0.0f;

        DeadStage01Enemy = 0;
        DeadStage02Enemy = 0;
        DeadStage03Enemy = 0;

        EyesUI.SetActive(false);
        BlackTop = EyesUI.transform.Find("BlackTop").GetComponent<Image>();
        BlackDown = EyesUI.transform.Find("BlackDown").GetComponent<Image>();
        EndingCheckTime = 0.0f;
    }

    private void FixedUpdate()
    {
        //맨 처음 시작 후 적 생성 트리거를 밟기전
        if (stageState == StageState.None)
        {
            for (int i = 0; i < StartTrigger.Length; ++i)
            {
                if (StartTrigger[i].isCollision)
                {
                    stageState = StageState.Stage01;
                    StartTrigger[i].isCollision = false;
                }

                if (stageState == StageState.Stage01)
                {
                    for (int j = 0; j < StartTrigger.Length; ++j)
                    {
                        StartTrigger[j].gameObject.SetActive(false);
                    }
                }
            }
        }

        //스테이지 01 충돌체와 충돌하면
        if (Stage01ClearCollision && Stage01ClearCollision.isCollision)
        {
            TextBoxUI.gameObject.SetActive(true);
            TextBoxUI.GetComponentInChildren<Text>().text = "Not yet possible to move !";

            TextBoxUICount += Time.deltaTime;

            if (TextBoxUICount > 3.0f)
            {
                Stage01ClearCollision.isCollision = false;
                TextBoxUI.gameObject.SetActive(false);
                TextBoxUICount = 0.0f;
            }
        }

        if (DeadStage01Enemy >= 7)
        {
            if (Stage01ClearCollision)
                Destroy(Stage01ClearCollision.gameObject);

            TextBoxUI.gameObject.SetActive(true);
            TextBoxUI.GetComponentInChildren<Text>().text = "Let's move to next stage !";

            TextBoxUICount += Time.deltaTime;

            if (!audioSource_UI.isPlaying && TextBoxUICount < 0.5f)
            {
                //[0] 기계 소리 (문열리는 소리)
                audioSource_UI.clip = audioClips[0];
                audioSource_UI.Play();
            }

            if (TextBoxUICount > 3.0f)
            {
                if (Stage01ClearCollision)
                    Stage01ClearCollision.isCollision = false;

                TextBoxUI.gameObject.SetActive(false);
                TextBoxUICount = 0.0f;
                DeadStage01Enemy = 0;

                if (Stage01ClearCollision)
                    Stage01ClearCollision = null;

                //스테이지02로 변경
                stageState = StageState.Stage02;
            }
        }

        //스테이지 02 충돌체와 충돌하면
        if (Stage02ClearCollision && Stage02ClearCollision.isCollision)
        {
            TextBoxUI.gameObject.SetActive(true);
            TextBoxUI.GetComponentInChildren<Text>().text = "Not yet possible to move !";

            TextBoxUICount += Time.deltaTime;

            if (TextBoxUICount > 3.0f)
            {
                Stage02ClearCollision.isCollision = false;
                TextBoxUI.gameObject.SetActive(false);
                TextBoxUICount = 0.0f;
            }
        }

        if (DeadStage02Enemy >= 5)
        {
            if (Stage02ClearCollision)
                Destroy(Stage02ClearCollision.gameObject);

            TextBoxUI.gameObject.SetActive(true);
            TextBoxUI.GetComponentInChildren<Text>().text = "Let's move to next stage !";

            TextBoxUICount += Time.deltaTime;

            if (!audioSource_UI.isPlaying && TextBoxUICount < 0.5f)
            {
                //[0] 기계 소리 (문열리는 소리)
                audioSource_UI.clip = audioClips[0];
                audioSource_UI.Play();
            }

            if (TextBoxUICount > 3.0f)
            {
                if (Stage02ClearCollision)
                    CreateState02Enemy.isCollision = false;
                Stage02ClearCollision.isCollision = false;
                TextBoxUI.gameObject.SetActive(false);
                TextBoxUICount = 0.0f;
                DeadStage02Enemy = 0;

                if (Stage02ClearCollision)
                    Stage02ClearCollision = null;

                //스테이지03로 변경
                stageState = StageState.Stage03;
            }
        }

        //스테이지 03 충돌체와 충돌하면
        if (Stage03ClearCollision && Stage03ClearCollision.isCollision)
        {
            TextBoxUI.gameObject.SetActive(true);
            TextBoxUI.GetComponentInChildren<Text>().text = "Not yet possible to move !";

            TextBoxUICount += Time.deltaTime;

            if (TextBoxUICount > 3.0f)
            {
                Stage03ClearCollision.isCollision = false;
                TextBoxUI.gameObject.SetActive(false);
                TextBoxUICount = 0.0f;
            }
        }

        if (DeadStage03Enemy >= 8) //근접 5 + 원거리 3
        {
            if (Stage03ClearCollision)
                Destroy(Stage03ClearCollision.gameObject);

            TextBoxUI.gameObject.SetActive(true);
            TextBoxUI.GetComponentInChildren<Text>().text = "Let's move to next stage !";

            TextBoxUICount += Time.deltaTime;

            if (!audioSource_UI.isPlaying && TextBoxUICount < 0.5f)
            {
                //[0] 기계 소리 (문열리는 소리)
                audioSource_UI.clip = audioClips[0];
                audioSource_UI.Play();
            }

            if (TextBoxUICount > 3.0f)
            {
                CreateState03Enemy.isCollision = false;

                if (Stage03ClearCollision)
                    Stage03ClearCollision.isCollision = false;
                TextBoxUI.gameObject.SetActive(false);
                TextBoxUICount = 0.0f;
                DeadStage03Enemy = 0;

                if (Stage03ClearCollision)
                    Stage03ClearCollision = null;
            }
        }

        if (CreateState04.isCollision)
        {
            CreateState04.isCollision = false;
            stageState = StageState.Stage04;
        }

        if (stageState == StageState.Stage04)
        {
            TimeLine.SetActive(true);
        }

        if (timeLineAnimation.isEnding)
        {
            EyesUI.SetActive(true);

            if (BlackTop.color.a <= 1.0f)
            {
                BlackTop.color = new Color(BlackTop.color.r, BlackTop.color.g, BlackTop.color.b, BlackTop.color.a + 0.0025f);
            }
            if (BlackDown.color.a <= 1.0f)
            {
                BlackDown.color = new Color(BlackDown.color.r, BlackDown.color.g, BlackDown.color.b, BlackDown.color.a + 0.002f);
            }

            if (BlackTop.transform.localPosition.y > 200)
            {
                BlackTop.transform.Translate(Vector3.down * 1.5f);
            }
            else
            {
                //BlackTop기준으로 다 내려왔으면 EndingUI 출력
                EndingUI.SetActive(true);

                EndingCheckTime += Time.deltaTime;
            }

            if (BlackDown.transform.localPosition.y < -200)
            {
                BlackDown.transform.Translate(Vector3.up * 1.5f);
            }

            if (EndingCheckTime > 3.0f)
            {
                SceneManager.LoadScene("EndingScene");
            }
        }
    }
}
