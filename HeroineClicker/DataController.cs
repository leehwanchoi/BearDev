using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Text;

public class DataController : MonoBehaviour
{
    public ClothesManager clothesManager;
    public FaceManager faceManager;
    public HairManager hairManager;

    private static DataController instance;

    public static DataController Instance
    {
        get
        {
            if (instance == null)
            {
                instance = FindObjectOfType<DataController>();

                if (instance == null)
                {
                    GameObject container = new GameObject("DataController");

                    instance = container.AddComponent<DataController>();
                }
            }

            return instance;
        }
    }

    private HeroButton[] heroButtons;
    private DressesButton[] dressesButton;
    private HairButton[] hairButton;

    //마지막 플레이 시간을 알아낸다.
    DateTime GetLastPlayDate()
    {
        if (!PlayerPrefs.HasKey("Time"))
        {
            return DateTime.Now;
        }

        string TimeBinaryInString = PlayerPrefs.GetString("Time");
        long timeBinaryInLong = Convert.ToInt64(TimeBinaryInString);

        return DateTime.FromBinary(timeBinaryInLong);
    }

    //마지막 플레이 시간을 저장한다.
    private void UpdateLastPlayDate()
    {
        PlayerPrefs.SetString("Time", DateTime.Now.ToBinary().ToString());
    }

    public void OnApplicationQuit()
    {
        UpdateLastPlayDate();
    }

    public long gold
    {
        get
        {
            if (!PlayerPrefs.HasKey("Gold"))
            {
                return 0;
            }

            string tmpGold = PlayerPrefs.GetString("Gold");
            return long.Parse(tmpGold);
        }
        set
        {
            PlayerPrefs.SetString("Gold", value.ToString());
        }
    }
    public int goldPerClick
    {
        get
        {
            return PlayerPrefs.GetInt("GoldPerClick", 1);
        }
        set
        {
            PlayerPrefs.SetInt("GoldPerClick", value);
        }
    }
    public int nowDressCode
    {
        get
        {
            return PlayerPrefs.GetInt("nowDressCode", 14);
        }
        set
        {
            PlayerPrefs.SetInt("nowDressCode", value);
        }
    }
    public int nowHairCode
    {
        get
        {
            return PlayerPrefs.GetInt("nowHairCode", 14);
        }
        set
        {
            PlayerPrefs.SetInt("nowHairCode", value);
        }
    }
    public int nowHeroine
    {
        get
        {
            return PlayerPrefs.GetInt("nowHeroine", 0);
        }
        set
        {
            PlayerPrefs.SetInt("nowHeroine", value);
        }
    }

    //현재시간에서 마지막 플레이 시간을 뺀 값을 리턴해주는 함수
    public int timeAfterLastPlay
    {
        get
        {
            DateTime currentTime = DateTime.Now;
            DateTime lastPlayDate = GetLastPlayDate();

            return (int)currentTime.Subtract(lastPlayDate).TotalSeconds;
        }
    }

    private void Awake()
    {
        heroButtons = FindObjectsOfType<HeroButton>();
        //dressesButton = FindObjectsOfType<DressesButton>();
        //hairButton = FindObjectsOfType<HairButton>();
    }

    private void Start()
    {
        //gold += GetGoldPerSec() * timeAfterLastPlay; //마지막 접속시간 기준으로 다음번 접속시 자동으로 골드 계산해서 추가해준다. (자동 골드 획득 기능)
        //InvokeRepeating("UpdateLastPlayDate", 0f, 5f); //5초 간격으로 마지막플레이 시간을 저장해주는 함수 호출

        clothesManager.stateNum = nowDressCode;
        hairManager.stateNum = nowHairCode;
    }

    public void LoadUpgradeButton(UpgradeButton upgradeButton)
    {
        string key = upgradeButton.upgradeName;

        upgradeButton.level = PlayerPrefs.GetInt(key + "_level", 1);
        upgradeButton.goldByUpgrade = PlayerPrefs.GetInt(key + "_goldByUpgrade", upgradeButton.startGoldByUpgrade);
        upgradeButton.currentCost = PlayerPrefs.GetInt(key + "_cost", upgradeButton.startCurrentCost);
    }

    public void SaveUpgradeButton(UpgradeButton upgradeButton)
    {
        string key = upgradeButton.upgradeName;

        PlayerPrefs.SetInt(key + "_level", upgradeButton.level);
        PlayerPrefs.SetInt(key + "_goldByUpgrade", upgradeButton.goldByUpgrade);
        PlayerPrefs.SetInt(key + "_cost", upgradeButton.currentCost);
    }

    public void LoadHeroButton(HeroButton HeroButton)
    {
        string key = HeroButton.itemName;

        HeroButton.level = PlayerPrefs.GetInt(key + "_level");
        HeroButton.currentCost = PlayerPrefs.GetInt(key + "_cost", HeroButton.startCurrentCost);
        HeroButton.goldPerSec = PlayerPrefs.GetInt(key + "_goldPerSec");

        if (PlayerPrefs.GetInt(key + "_isPurchased") == 1)
        {
            HeroButton.isPurchased = true;
        }
        else
        {
            HeroButton.isPurchased = false;
        }
    }

    public void SaveHeroButton(HeroButton HeroButton)
    {
        string key = HeroButton.itemName;

        PlayerPrefs.SetInt(key + "_level", HeroButton.level);
        PlayerPrefs.SetInt(key + "_cost", HeroButton.currentCost);
        PlayerPrefs.SetInt(key + "_goldPerSec", HeroButton.goldPerSec);

        if (HeroButton.isPurchased == true)
        {
            PlayerPrefs.SetInt(key + "_isPurchased", 1);
        }
        else
        {
            PlayerPrefs.SetInt(key + "_isPurchased", 0);
        }
    }

    public int GetGoldPerSec()
    {
        int goldPerSec = 0;
        for (int i = 0; i < heroButtons.Length; i++)
        {
            if (heroButtons[i].isPurchased == true)
                goldPerSec += heroButtons[i].goldPerSec;
        }

        return goldPerSec;
    }

    public void LoadDressButton(DressesButton DressButton)
    {
        string key = DressButton.DressName;

        if (PlayerPrefs.GetInt(key + "_isPurchased") == 1)
        {
            DressButton.isPurchased = true;
        }
        else
        {
            DressButton.isPurchased = false;
        }
    }

    public void SaveDressButton(DressesButton DressButton)
    {
        string key = DressButton.DressName;

        if (DressButton.isPurchased == true)
        {
            PlayerPrefs.SetInt(key + "_isPurchased", 1);
        }
        else
        {
            PlayerPrefs.SetInt(key + "_isPurchased", 0);
        }
    }

    public void LoadHairButton(HairButton HairButton)
    {
        string key = HairButton.HairName;

        if (PlayerPrefs.GetInt(key + "_isPurchased") == 1)
        {
            HairButton.isPurchased = true;
        }
        else
        {
            HairButton.isPurchased = false;
        }
    }

    public void SaveHairButton(HairButton HairButton)
    {
        string key = HairButton.HairName;

        if (HairButton.isPurchased == true)
        {
            PlayerPrefs.SetInt(key + "_isPurchased", 1);
        }
        else
        {
            PlayerPrefs.SetInt(key + "_isPurchased", 0);
        }
    }

    public void LoadDressPriest(int DressCode)
    {
        if (DressCode == 17)
        {
            string key = InGameManager.Instance.BreastUnderWear.name;

            if (PlayerPrefs.GetInt(key + "_isWear17", 1) == 1)
            {
                InGameManager.Instance.isWear17 = true;
            }
            else
            {
                InGameManager.Instance.isWear17 = false;
            }
        }

        if (DressCode == 18)
        {
            string key = InGameManager.Instance.Hat.name;

            if (PlayerPrefs.GetInt(key + "_isWear18", 1) == 1)
            {
                InGameManager.Instance.isWear18 = true;
            }
            else
            {
                InGameManager.Instance.isWear18 = false;
            }
        }
    }

    public void SaveDressPriest(int DressCode)
    {
        if (DressCode == 17)
        {
            string key = InGameManager.Instance.BreastUnderWear.name;

            if (InGameManager.Instance.isWear17)
            {
                PlayerPrefs.SetInt(key + "_isWear17", 1);
            }
            else
            {
                PlayerPrefs.SetInt(key + "_isWear17", 0);
            }
        }

        if (DressCode == 18)
        {
            string key = InGameManager.Instance.Hat.name;

            if (InGameManager.Instance.isWear18)
            {
                PlayerPrefs.SetInt(key + "_isWear18", 1);
            }
            else
            {
                PlayerPrefs.SetInt(key + "_isWear18", 0);
            }
        }
    }

    public void LoadFirstClick(ClickButton clickButton)
    {
        string key = clickButton.name;

        if (PlayerPrefs.GetInt(key + "_isFirstClick", 0) == 1)
        {
            clickButton.isFirstClick = true;
        }
        else
        {
            clickButton.isFirstClick = false;
        }
    }

    public void SaveFirstClick(ClickButton clickButton)
    {
        string key = clickButton.name;

        if (clickButton.isFirstClick == true)
        {
            PlayerPrefs.SetInt(key + "_isFirstClick", 1);
        }
        else
        {
            PlayerPrefs.SetInt(key + "_isFirstClick", 0);
        }
    }

    public void LoadInGameManager(InGameManager inGameManager, int keyIndex)
    {
        string key = "Collection" + keyIndex;

        inGameManager.CollectionState[keyIndex] = PlayerPrefs.GetInt(key + "_Anchiev", 0);
    }

    public void SaveInGameManager(InGameManager inGameManager, int keyIndex)
    {
        string key = "Collection" + keyIndex;

        PlayerPrefs.SetInt(key + "_Anchiev", inGameManager.CollectionState[keyIndex]);
    }

    public int GetCollectionState(InGameManager inGameManager, int keyIndex)
    {
        string key = "Collection" + keyIndex;

        inGameManager.CollectionState[keyIndex] = PlayerPrefs.GetInt(key + "_Anchiev", 0);

        return inGameManager.CollectionState[keyIndex];
    }
}