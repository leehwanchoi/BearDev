using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class HairButton : MonoBehaviour
{
    public HairManager hairManager;

    public GameObject DressUI;

    public int HairCode;
    [HideInInspector]
    public string HairName;
    [HideInInspector]
    public bool isPurchased;
    public long HairPrice;

    private void Awake()
    {
        HairName = this.gameObject.name;
    }

    private void Start()
    {
        DataController.Instance.LoadHairButton(this);
    }

    public void Buy()
    {
        if (isPurchased)
        {
            DressUI.SetActive(true);
            DressUI.GetComponentInChildren<Text>().text = "이미 구매하셨습니다.";
        }

        if (!isPurchased && DataController.Instance.gold >= HairPrice)
        {
            if (DataController.Instance.GetCollectionState(InGameManager.Instance, 4) == 0)
                InGameManager.Instance.achievMessage(4, "헤어스타일의 변화!");

            isPurchased = true;

            DataController.Instance.gold -= HairPrice;

            DataController.Instance.SaveHairButton(this);
        }

        if (!isPurchased && DataController.Instance.gold < HairPrice)
        {
            DressUI.SetActive(true);
            DressUI.GetComponentInChildren<Text>().text = "금액이 부족합니다.";
        }
    }

    public void Wear()
    {
        if (isPurchased)
        {
            DataController.Instance.nowHairCode = HairCode;
            hairManager.stateNum = DataController.Instance.nowHairCode;
        }
        else if (!isPurchased && HairCode != 2)
        {
            DressUI.SetActive(true);
            DressUI.GetComponentInChildren<Text>().text = "구매하지 않았습니다.";
        }

        if (HairCode == 2)
        {
            DataController.Instance.nowHairCode = HairCode;
            hairManager.stateNum = DataController.Instance.nowHairCode;
        }
    }
}