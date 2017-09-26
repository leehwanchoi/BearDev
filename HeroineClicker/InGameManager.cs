using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class InGameManager : MonoBehaviour
{
    private static InGameManager instance;

    public static InGameManager Instance
    {
        get
        {
            if (instance == null)
            {
                instance = FindObjectOfType<InGameManager>();

                if (instance == null)
                {
                    GameObject container = new GameObject("InGameManager");

                    instance = container.AddComponent<InGameManager>();
                }
            }

            return instance;
        }
    }

    public ClickButton clickButton;
    public GameObject achievementUI;

    public GameObject Heroine00;
    public GameObject Heroine01;

    [HideInInspector] public bool isWear17;
    [HideInInspector] public bool isWear18;
    public GameObject BreastUnderWear;
    public GameObject Hat;

    public int[] CollectionState; //0이면 업적미완료, 1이면 완료
    public GameObject CollectionsParent;
    private int AllCollectionNum;

    private void Awake()
    {
        achievementUI.SetActive(false);
    }

    private void Start()
    {
        for (int i = 0; i < CollectionState.Length; i++)
        {
            DataController.Instance.LoadInGameManager(this, i);

            if (CollectionState[i] == 1)
            {
                CollectionsParent.transform.Find("Collection0" + i).transform.Find("Collection Image").gameObject.SetActive(true);
                CollectionsParent.transform.Find("Collection0" + i).transform.Find("Collection Image_Lock").gameObject.SetActive(false);
                CollectionsParent.transform.Find("Collection0" + i).transform.Find("Explanation").gameObject.SetActive(true);
                CollectionsParent.transform.Find("Collection0" + i).transform.Find("Explanation_Lock").gameObject.SetActive(false);
            }
        }

        if (DataController.Instance.nowHeroine == 0)
        {
            Heroine00.SetActive(true);
            Heroine01.SetActive(false);
        }
        else if (DataController.Instance.nowHeroine == 1)
        {
            Heroine00.SetActive(false);
            Heroine01.SetActive(true);

            DataController.Instance.LoadDressPriest(17);
            DataController.Instance.LoadDressPriest(18);

            if (isWear17)
                BreastUnderWear.SetActive(true);
            else
                BreastUnderWear.SetActive(false);

            if (isWear18)
                Hat.SetActive(true);
            else
                Hat.SetActive(false);
        }
    }

    public void achievMessage(int achievIndex, string achievMsg)
    {
        achievementUI.SetActive(true);
        achievementUI.GetComponentInChildren<Text>().text = achievMsg; //메시지 출력

        CollectionState[achievIndex] = 1; //업적 완료
        DataController.Instance.SaveInGameManager(this, achievIndex);

        CollectionsParent.transform.Find("Collection0" + achievIndex).transform.Find("Collection Image").gameObject.SetActive(true);
        CollectionsParent.transform.Find("Collection0" + achievIndex).transform.Find("Collection Image_Lock").gameObject.SetActive(false);
        CollectionsParent.transform.Find("Collection0" + achievIndex).transform.Find("Explanation").gameObject.SetActive(true);
        CollectionsParent.transform.Find("Collection0" + achievIndex).transform.Find("Explanation_Lock").gameObject.SetActive(false);
    }

    private void Update()
    {
        if (DataController.Instance.gold > 100000000)
        {
            if (DataController.Instance.GetCollectionState(InGameManager.Instance, 5) == 0)
                InGameManager.Instance.achievMessage(5, "여유로운 삶!");
        }

        if (DataController.Instance.GetCollectionState(InGameManager.Instance, 6) == 0)
        {
            for (int i = 0; i < CollectionState.Length; i++)
            {
                if (CollectionState[i] == 1)
                {
                    AllCollectionNum++;
                }
            }

            if (AllCollectionNum >= 6)
            {
                InGameManager.Instance.achievMessage(6, "컬렉션 마스터");
            }
            else
            {
                AllCollectionNum = 0;
            }
        }
    }

    public void WearPriestCloth(int DressCode)
    {
        if (DressCode == 17)
        {
            isWear17 = !isWear17;
            BreastUnderWear.SetActive(isWear17);
            DataController.Instance.SaveDressPriest(17);
        }
        else if (DressCode == 18)
        {
            isWear18 = !isWear18;
            Hat.SetActive(isWear18);
            DataController.Instance.SaveDressPriest(18);
        }
    }
}