using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class DestroyTower : MonoBehaviour
{
    public Camera camera;

    public GameObject FXBoom;
    public GameObject FXSandStorm;

    public GameObject Building;
    public GameObject DestroyBuilding;

    public GameObject GUI;
    private Image BG;
    private Text TXT;
    private byte alpha;
    private byte TXTalpha;

    private float FXStartTime;
    private bool isFXStart;

    private float ChangeDestroyTower;
    private bool ChangeTower;

    private float changeTxTCount;
    private float SceneChangeTime;

    private void Awake()
    {
        if (PlayerPrefs.GetInt("IsSkipOP", 0) == 0) // false
        {
            Screen.SetResolution(1280, 720, true);
            camera.enabled = true;
        }
        else // true
        {
            SceneManager.LoadScene("Start");
        }

        FXStartTime = 0.0f;
        isFXStart = false;

        ChangeDestroyTower = 0.0f;
        ChangeTower = false;

        changeTxTCount = 0.0f;
        SceneChangeTime = 7.5f;

        BG = GUI.GetComponentInChildren<Image>();
        TXT = GUI.GetComponentInChildren<Text>();
        TXT.enabled = false;
        TXTalpha = 0;
        TXT.color = new Color32((byte)128, (byte)255, (byte)180, TXTalpha);

        alpha = 0;
        BG.color = new Color32((byte)125, (byte)125, (byte)125, alpha);
    }

    private void FixedUpdate()
    {
        if (!ChangeTower && !isFXStart)
            FXStartTime += Time.deltaTime;

        if (FXStartTime > 2.0f)
        {
            isFXStart = true;
            FXStartTime = 0.0f;
        }

        if (isFXStart)
        {
            FXBoom.SetActive(true);

            if (!FXBoom.GetComponent<ParticleSystem>().isPlaying)
            {
                FXSandStorm.SetActive(true);

                if (FXSandStorm.transform.position.x < 103.0f)
                {
                    FXSandStorm.transform.Translate(Vector3.right * 40.0f * Time.deltaTime);
                }

                ChangeDestroyTower += Time.deltaTime;
                if (ChangeDestroyTower > 3.0f && FXSandStorm.transform.position.x > 103.0f)
                {
                    ChangeDestroyTower = 0.0f;
                    ChangeTower = true;
                }
            }
        }

        if (ChangeTower)
        {
            isFXStart = false;
            FXBoom.SetActive(false);

            Building.SetActive(false);
            DestroyBuilding.SetActive(true);

            if (FXSandStorm.transform.position.x > 105.0f)
            {
                GUI.SetActive(true);

                alpha++;

                if (alpha > 200)
                {
                    alpha = 200;
                    TXT.enabled = true;

                    if (TXTalpha < 254)
                        TXTalpha++;
                    if (TXTalpha >= 255)
                        TXTalpha = 254;

                    TXT.color = new Color32((byte)128, (byte)255, (byte)180, TXTalpha);
                }

                BG.color = new Color32((byte)125, (byte)125, (byte)125, alpha);
            }

            if (FXSandStorm.transform.position.x < 200.0f)
            {
                FXSandStorm.transform.Translate(Vector3.right * 20.0f * Time.deltaTime);
            }

            if (FXSandStorm.transform.position.x > 200.0f)
            {
                FXSandStorm.SetActive(false);
            }
        }

        if (TXT.enabled)
        {
            changeTxTCount += Time.deltaTime;

            if (changeTxTCount > SceneChangeTime)
            {
                PlayerPrefs.SetInt("IsSkipOP", 1);
                SceneManager.LoadScene("Start");
            }
        }
    }
}