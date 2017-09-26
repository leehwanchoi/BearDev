using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class ARaycast : MonoBehaviour
{
    public float upPosition = 1.0f;
    public float Range = 3.0f;
    public RaycastHit hit;
    Vector3 Ray_position;
    ButtonA buttonA;

    strText strtext;

    public GameObject textBox;
    public GameObject ReadyBox;

    void Start()
    {
        buttonA = GameObject.Find("ButtonA").GetComponent<ButtonA>();

        Ray_position = new Vector3(this.transform.position.x, this.transform.position.y + upPosition, this.transform.position.z);
    }

	// Update is called once per frame
	void Update ()
    {
        Ray_position = new Vector3(this.transform.position.x, this.transform.position.y + upPosition, this.transform.position.z);
        Debug.DrawRay(Ray_position, this.transform.forward * Range, Color.red);

        if (buttonA.isDown)
        {
            if (Physics.Raycast(Ray_position, transform.forward, out hit, Range))
            {
                if (hit.collider.tag == "NPC")
                {
                    Debug.Log(hit.collider.name);
                    Dialogue();  //Dialogue Function
                }
                else
                    buttonA.isAllow = true;

                if (hit.collider.name == "ReadyCheckLine")
                {
                    ReadyBox.SetActive(true);
                }
            }
            else
                buttonA.isAllow = true; //buttonA.isAllow의 false는 buttonA Function에서 변경
        }
    }

    void Dialogue()
    {
        if (hit.collider.gameObject.tag == "NPC")
        {
            textBox.SetActive(true);

            strtext = GameObject.Find("TextBox_Text").GetComponent<strText>();
            strtext.str1 = hit.collider.gameObject.GetComponent<NPCstr>().npcstr;

            GameObject.Find("NameBox_Text").GetComponent<Text>().text = hit.collider.gameObject.GetComponent<NPCNameText>().str1;
        }
    }
}