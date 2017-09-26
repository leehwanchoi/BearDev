using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UpgradeButton : MonoBehaviour
{
    public AudioSource audioSource;
    public AudioClip audioClip;

    public Text upgradeDisPlayer;
    public string upgradeName;

    [HideInInspector]
    public int goldByUpgrade;
    public int startGoldByUpgrade;

    [HideInInspector]
    public int currentCost = 1;
    public int startCurrentCost = 1;

    [HideInInspector]
    public int level = 1;
    public float upgradePow = 1.07f;
    public float costPow = 3.14f;

    private void Start()
    {
        DataController.Instance.LoadUpgradeButton(this);

        UpdateUI();
    }

    public void PurchaseUpgrade()
    {
        if (DataController.Instance.gold >= currentCost && level < 20)
        {
            if (DataController.Instance.GetCollectionState(InGameManager.Instance, 1) == 0)
                InGameManager.Instance.achievMessage(1, "업그레이드의 시작!");

            audioSource.clip = audioClip;
            audioSource.Play();

            DataController.Instance.gold -= currentCost;
            level++;
            DataController.Instance.goldPerClick += goldByUpgrade;

            UpdateUpgrade();
            UpdateUI();
            DataController.Instance.SaveUpgradeButton(this);
        }
    }

    public void UpdateUpgrade()
    {
        goldByUpgrade = startGoldByUpgrade * (int)Mathf.Pow(upgradePow, level);
        currentCost = startCurrentCost * (int)Mathf.Pow(costPow, level);
    }

    public void UpdateUI()
    {
        if (level < 20)
            upgradeDisPlayer.text = upgradeName + "\n레벨: " + level + "\n가격: " + currentCost + "\n클릭당 골드: " + goldByUpgrade;
        else if (level == 20)
            upgradeDisPlayer.text = upgradeName + "\n레벨: MAX" + "\n가격: " + currentCost + "\n클릭당 골드: " + goldByUpgrade;
    }
}