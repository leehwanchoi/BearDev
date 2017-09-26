using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class VJS : MonoBehaviour, IDragHandler, IPointerUpHandler, IPointerDownHandler
{
    Image BG;
    Image Stick;
    Vector3 inputVector;

    public bool isDown; //누른 상태인지 체크

    void Start()
    {
        BG = GetComponent<Image>();
        Stick = transform.GetChild(0).GetComponent<Image>();
    }

    public virtual void OnDrag(PointerEventData ped)
    {
        Vector2 pos;
        if (RectTransformUtility.ScreenPointToLocalPointInRectangle(BG.rectTransform, ped.position, ped.pressEventCamera, out pos))
        {
            pos.x = pos.x / BG.rectTransform.sizeDelta.x;
            pos.y = pos.y / BG.rectTransform.sizeDelta.y;

            inputVector = new Vector3(pos.x * 2, 0, pos.y * 2);
            inputVector = (inputVector.magnitude > 1.0f) ? inputVector.normalized : inputVector;

            Stick.rectTransform.anchoredPosition = new Vector3(inputVector.x * (BG.rectTransform.sizeDelta.x / 5), inputVector.z * (BG.rectTransform.sizeDelta.y / 5));
        }
    }

    public virtual void OnPointerDown(PointerEventData ped)
    {
        OnDrag(ped);

        isDown = true;
    }

    public virtual void OnPointerUp(PointerEventData ped)
    {
        //inputVector = Vector3.zero;                           //초기화 시키려면 주석제거
        Stick.rectTransform.anchoredPosition = Vector3.zero;

        isDown = false;
    }

    public float Horizontal()
    {
        if (inputVector.x != 0)
            return inputVector.x;
        else
            return Input.GetAxis("Horizontal");
    }

    public float Vertical()
    {
        if (inputVector.z != 0)
            return inputVector.z;
        else
            return Input.GetAxis("Vertical");
    }
}
