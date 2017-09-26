using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AchievMovement : MonoBehaviour
{
    private bool isDown;
    private float WaitTime;

    private void Awake()
    {
        isDown = false;
        WaitTime = 0.0f;
    }

    private void FixedUpdate()
    {
        if (!isDown && this.transform.localPosition.y > 640)
            this.transform.Translate(Vector2.down, Space.Self);
        else
        {
            isDown = true;
            WaitTime += Time.deltaTime;

            if (isDown && WaitTime > 3.0f)
            {
                this.transform.Translate(Vector2.up, Space.Self);
            }

            if (isDown && this.transform.localPosition.y > 740)
            {
                isDown = false;
                WaitTime = 0.0f;

                this.transform.localPosition = new Vector3(0.0f, 740.0f, 0.0f);

                this.gameObject.SetActive(false);
            }
        }
    }
}