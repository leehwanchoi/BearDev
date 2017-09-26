using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class DressesButton : MonoBehaviour
{
    public ClothesManager clothesManager;

    public GameObject DressUI;

    public int DressCode;
    [HideInInspector]
    public string DressName;
    [HideInInspector]
    public bool isPurchased;
    public long DressPrice; 

    private void Awake()
    {
        DressName = this.gameObject.name;
    }

    private void Start()
    {
        DataController.Instance.LoadDressButton(this);
    }

    public void Buy()
    {
        if (isPurchased)
        {
            DressUI.SetActive(true);
            DressUI.GetComponentInChildren<Text>().text = "이미 구매하셨습니다.";
        }

        if (!isPurchased && DataController.Instance.gold >= DressPrice)
        {
            if (DataController.Instance.GetCollectionState(InGameManager.Instance, 3) == 0)
                InGameManager.Instance.achievMessage(3, "지름의 시작!");

            isPurchased = true;

            DataController.Instance.gold -= DressPrice;

            DataController.Instance.SaveDressButton(this);
        }

        if (!isPurchased && DataController.Instance.gold < DressPrice)
        {
            DressUI.SetActive(true);
            DressUI.GetComponentInChildren<Text>().text = "금액이 부족합니다.";
        }
    }

    public void Wear()
    {
        if (DataController.Instance.nowHeroine == 0)
        {
            if (isPurchased)
            {
                DataController.Instance.nowDressCode = DressCode;
                clothesManager.stateNum = DataController.Instance.nowDressCode;
            }
            else if (!isPurchased && DressCode != 14)
            {
                DressUI.SetActive(true);
                DressUI.GetComponentInChildren<Text>().text = "구매하지 않았습니다.";
            }

            if (DressCode == 14)
            {
                DataController.Instance.nowDressCode = DressCode;
                clothesManager.stateNum = DataController.Instance.nowDressCode;
            }
        }

        if (DataController.Instance.nowHeroine == 1)
        {
            if (isPurchased && DressCode == 17)
            {
                InGameManager.Instance.WearPriestCloth(17);
            }
            else if (!isPurchased && DressCode == 17)
            {
                DressUI.SetActive(true);
                DressUI.GetComponentInChildren<Text>().text = "구매하지 않았습니다.";
            }

            if (isPurchased && DressCode == 18)
            {
                InGameManager.Instance.WearPriestCloth(18);
            }
            else if (!isPurchased && DressCode == 18)
            {
                DressUI.SetActive(true);
                DressUI.GetComponentInChildren<Text>().text = "구매하지 않았습니다.";
            }
        }
    }
}