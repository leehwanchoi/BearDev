using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class BloodingUI : MonoBehaviour
{
    [SerializeField] Image image;
    public Sprite[] bloodImage;
    private float showTime;

    private bool isOnceTime;
    public bool isBlooding;

    private void Start()
    {
        showTime = 0.0f;

        isOnceTime = false;
        isBlooding = false;
    }

    void FixedUpdate()
    {
        if (isBlooding)
        {
            if (!isOnceTime)
            {
                image.enabled = true;
                image.sprite = bloodImage[Random.Range(0, bloodImage.Length)];
                isOnceTime = true;
            }

            showTime += Time.deltaTime;

            if (showTime > 1.0f)
            {
                image.enabled = false;
                isBlooding = false;
                isOnceTime = false;
                showTime = 0.0f;
            }
        }
	}
}