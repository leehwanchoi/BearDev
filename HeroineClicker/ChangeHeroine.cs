using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChangeHeroine : MonoBehaviour
{
    public GameObject Button00;
    public GameObject Button01;

    public GameObject Heroine00;
    public GameObject Heroine01;

    public void ClickChangeButton()
    {
        if (this.gameObject.name == "SelectButton00")
        {
            Heroine00.SetActive(true);
            Heroine01.SetActive(false);

            DataController.Instance.nowHeroine = 0;
        }
        else if (this.gameObject.name == "SelectButton01")
        {
            Heroine00.SetActive(false);
            Heroine01.SetActive(true);

            DataController.Instance.nowHeroine = 1;

            DataController.Instance.LoadDressPriest(17);
            DataController.Instance.LoadDressPriest(18);

            if (InGameManager.Instance.isWear17)
                InGameManager.Instance.BreastUnderWear.SetActive(true);
            else
                InGameManager.Instance.BreastUnderWear.SetActive(false);

            if (InGameManager.Instance.isWear18)
                InGameManager.Instance.Hat.SetActive(true);
            else
                InGameManager.Instance.Hat.SetActive(false);
        }
    }
}