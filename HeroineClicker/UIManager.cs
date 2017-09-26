using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIManager : MonoBehaviour
{
    public Text goldDisplayer;
    public Text goldPerClickDisplayer;
    public Text goldperSecDisplayer;

    private void Update()
    {
        goldDisplayer.text = "보유 골드: " + DataController.Instance.gold;
        goldPerClickDisplayer.text = "클릭당 골드: " + DataController.Instance.goldPerClick;
        goldperSecDisplayer.text = "초당 골드: " + DataController.Instance.GetGoldPerSec();
    }
}