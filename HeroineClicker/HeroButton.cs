using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class HeroButton : MonoBehaviour
{
    public AudioSource audioSource;
    public AudioClip audioClip;

    public Color upgradeableColor = Color.blue;
    public Color notUpgradeableColor = Color.red;

    public Image colorImage;

    public Text itemDisplayer;

    public CanvasGroup canvasGroup;
    public Slider slider;

    public string itemName;
    public int level;

    [HideInInspector]
    public int currentCost;
    public int startCurrentCost = 1;

    [HideInInspector]
    public int goldPerSec;
    public int startGoldPerSec = 1;

    public float costPow = 3.14f;
    public float upgradePow = 1.07f;

    [HideInInspector]
    public bool isPurchased = false;

    private void Start()
    {
        DataController.Instance.LoadHeroButton(this);

        StartCoroutine("AddGoldLoop");
        UpdateUI();
    }

    public void PurchaseItem()
    {
        if (DataController.Instance.gold >= currentCost && level < 10)
        {
            if (DataController.Instance.GetCollectionState(InGameManager.Instance, 2) == 0)
                InGameManager.Instance.achievMessage(2, "첫번째 친구!");

            audioSource.clip = audioClip;
            audioSource.Play();

            isPurchased = true;
            DataController.Instance.gold -= currentCost;
            level++;

            UpdateItem();
            UpdateUI();
            DataController.Instance.SaveHeroButton(this);
        }
    }

    private IEnumerator AddGoldLoop()
    {
        while(true)
        {
            if (isPurchased)
            {
                DataController.Instance.gold += goldPerSec;
            }

            yield return new WaitForSeconds(1.0f);
        }
    }

    public void UpdateItem()
    {
        goldPerSec = goldPerSec + startGoldPerSec * (int)Mathf.Pow(upgradePow, level);
        currentCost = startCurrentCost * (int)Mathf.Pow(costPow, level);
    }

    public void UpdateUI()
    {
        if (level < 10)
            itemDisplayer.text = itemName + " 레벨:" + level + "\n가격: " + currentCost + "\n초당골드: " + goldPerSec;
        else if (level ==10)
            itemDisplayer.text = itemName + " 레벨: MAX" + "\n가격: " + currentCost + "\n초당골드: " + goldPerSec;

        slider.minValue = 0;
        slider.maxValue = currentCost;

        slider.value = DataController.Instance.gold;

        if (isPurchased)
        {
            canvasGroup.alpha = 1.0f;
        }
        else
        {
            canvasGroup.alpha = 0.6f;
        }

        if (currentCost <= DataController.Instance.gold)
        {
            colorImage.color = upgradeableColor;
        }
        else
        {
            colorImage.color = notUpgradeableColor;
        }
    }

    private void Update()
    {
        UpdateUI();
    }
}