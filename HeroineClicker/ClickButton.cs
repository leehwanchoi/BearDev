using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ClickButton : MonoBehaviour
{
    public GameObject Collections;
    public GameObject Dresses;
    public GameObject Hair;
    public GameObject Heroine;
    public GameObject Option;

    private AudioSource audioSource;
    public AudioClip[] audioClips;

    private GameObject target;

    public ClothesManager clothesManager;
    public FaceManager faceManager;
    public HairManager hairManager;

    public InGameManager inGameManager;

    private bool isShy;
    private float shyTime;

    private bool isSoundCoolTime;
    private float soundCoolTime;

    private bool isChangeFace;
    private float ChangeFaceTime;

    private bool isRandomFace;
    private float RandomFaceTime;

    private int mouse = 0;

    [HideInInspector]
    public bool isFirstClick;

    private void Awake()
    {
        audioSource = this.GetComponent<AudioSource>();
        audioSource.volume = 0.5f;

        isShy = false;
        shyTime = 0.0f;
        isSoundCoolTime = false;
        soundCoolTime = 0.0f;
        isChangeFace = false;
        ChangeFaceTime = 0.0f;

        isRandomFace = false;
        RandomFaceTime = 0.0f;
    }

    private void Start()
    {
        DataController.Instance.LoadFirstClick(this);
    }

    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            DataController.Instance.OnApplicationQuit();
        }

        // 핸드폰용
        // 터치가 있는 경우만
        if (Input.touchCount > 0)
        {
            if (!isFirstClick)
            {
                isFirstClick = true;
                DataController.Instance.SaveFirstClick(this);

                inGameManager.achievMessage(0, "클릭의 시작!");
            }

            Touch[] touches = Input.touches;

            for (int i = 0; i < touches.Length; i++)
            {
                CastRay_Touch();

                if (touches[i].phase == TouchPhase.Began)
                {
                    //랜덤 표정을 위한 클릭시 초기화 (미클릭시에만 설정)
                    isRandomFace = false;
                    RandomFaceTime = 0.0f;

                    if (target && !Collections.activeSelf && !Dresses.activeSelf && !Hair.activeSelf && !Option.activeSelf && !Heroine.activeSelf)
                    {
                        DataController.Instance.gold += DataController.Instance.goldPerClick;

                        if (target.name == "Sensitive")
                        {
                            if (!audioSource.isPlaying && !isSoundCoolTime)
                            {
                                audioSource.clip = audioClips[0];
                                audioSource.Play();
                                isSoundCoolTime = true;

                                //재생 할때 같이 표정 변화
                                faceManager.ChangeMouse(7);
                            }

                            isShy = true;
                            isChangeFace = true;
                        }
                        else if (target.name == "Boobs")
                        {
                            if (!audioSource.isPlaying && !isSoundCoolTime)
                            {
                                audioSource.clip = audioClips[1];
                                audioSource.Play();
                                isSoundCoolTime = true;

                                //재생 할때 같이 표정 변화
                                faceManager.ChangeMouse(7);
                            }

                            isShy = true;
                            isChangeFace = true;
                        }
                    }

                    if (!target && !Collections.activeSelf && !Dresses.activeSelf && !Hair.activeSelf && !Option.activeSelf && !Heroine.activeSelf)
                    {
                        DataController.Instance.gold += 1;
                    }
                }
            }
        }

        //PC용
        if (Input.GetMouseButtonDown(0))
        {
            if (!isFirstClick)
            {
                isFirstClick = true;
                DataController.Instance.SaveFirstClick(this);

                inGameManager.achievMessage(0, "클릭의 시작!");
            }

            CastRay();

            //랜덤 표정을 위한 클릭시 초기화 (미클릭시에만 설정)
            isRandomFace = false;
            RandomFaceTime = 0.0f;


            if (target && !Collections.activeSelf && !Dresses.activeSelf && !Hair.activeSelf && !Option.activeSelf && !Heroine.activeSelf)
            {
                DataController.Instance.gold += DataController.Instance.goldPerClick;

                if (target.name == "Sensitive")
                {
                    if (!audioSource.isPlaying && !isSoundCoolTime)
                    {
                        audioSource.clip = audioClips[0];
                        audioSource.Play();
                        isSoundCoolTime = true;

                        //재생 할때 같이 표정 변화
                        faceManager.ChangeMouse(7);
                    }

                    isShy = true;
                    isChangeFace = true;
                }
                else if (target.name == "Boobs")
                {
                    if (!audioSource.isPlaying && !isSoundCoolTime)
                    {
                        audioSource.clip = audioClips[1];
                        audioSource.Play();
                        isSoundCoolTime = true;

                        //재생 할때 같이 표정 변화
                        faceManager.ChangeMouse(7);
                    }

                    isShy = true;
                    isChangeFace = true;
                }
            }

            if (!target && !Collections.activeSelf && !Dresses.activeSelf && !Hair.activeSelf && !Option.activeSelf && !Heroine.activeSelf)
            {
                DataController.Instance.gold += 1;
            }
        }

        // 발그레 효과
        if (isShy)
            shyTime += Time.deltaTime;

        if (isShy && shyTime < 3.0f)
        {
            faceManager.shyFlag = true;
            faceManager.shy.SetActive(true);
        }
        else
        {
            faceManager.shyFlag = false;
            faceManager.shy.SetActive(false);
            shyTime = 0.0f;
            isShy = false;
        }

        // 사운드 쿨타임
        if (isSoundCoolTime)
            soundCoolTime += Time.deltaTime;

        if (soundCoolTime > 3.0f)
        {
            soundCoolTime = 0.0f;
            isSoundCoolTime = false;
        }

        // 얼굴 표정 변경
        if (isChangeFace)
            ChangeFaceTime += Time.deltaTime;

        if (ChangeFaceTime > 3.0f)
        {
            faceManager.ChangeMouse(0);
            isChangeFace = false;
            ChangeFaceTime = 0.0f;
        }

        // 랜덤 표정
        RandomFaceTime += Time.deltaTime;

        if (!Input.GetMouseButtonDown(0) && RandomFaceTime > 10.0f)
        {
            isRandomFace = true;
        }

        if (isRandomFace)
        {
            int RndNum = Random.Range(0, faceManager.mouseImages.Length);

            if (RndNum == 1 || RndNum == 2 || RndNum == 4 || RndNum == 8 || RndNum == 10)
            {
                if (!audioSource.isPlaying)
                {
                    audioSource.clip = audioClips[Random.Range(2, 4)];
                    audioSource.Play();
                }
            }
            else
            {
                if (!audioSource.isPlaying)
                {
                    audioSource.clip = audioClips[4];
                    audioSource.Play();
                }
            }

            faceManager.ChangeMouse(RndNum);

            isRandomFace = false;
            RandomFaceTime = 0.0f;
        }
    }

    void CastRay() // 유닛 히트처리 부분.  레이를 쏴서 처리합니다. 
    {
        target = null;

        Vector2 pos = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        RaycastHit2D hit = Physics2D.Raycast(pos, Vector2.zero, 0f);

        if (hit.collider != null)
        { //히트되었다면 여기서 실행
            //Debug.Log (hit.collider.name);  //이 부분을 활성화 하면, 선택된 오브젝트의 이름이 찍혀 나옵니다. 
            target = hit.collider.gameObject;  //히트 된 게임 오브젝트를 타겟으로 지정
        }
    }

    void CastRay_Touch()
    {
        target = null;

        // 터치 스크린을 터치한 정보를 배열에 담는다.
        Touch[] touches = Input.touches;

        for (int i = 0; i < touches.Length; ++i)
        {
            Vector2 pos = Camera.main.ScreenToWorldPoint(touches[i].position);
            RaycastHit2D hit = Physics2D.Raycast(pos, Vector2.zero, 0f);

            if (hit.collider != null)
            { //히트되었다면 여기서 실행
              //Debug.Log (hit.collider.name);  //이 부분을 활성화 하면, 선택된 오브젝트의 이름이 찍혀 나옵니다. 
                target = hit.collider.gameObject;  //히트 된 게임 오브젝트를 타겟으로 지정
            }
        }
    }
}