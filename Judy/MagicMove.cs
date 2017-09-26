using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MagicMove : MonoBehaviour
{
    float time;
    public float speed = 1.0f;

	// Use this for initialization
	void Start ()
    {

    }
	
	// Update is called once per frame
	void Update ()
    {
        time += Time.deltaTime;

        transform.Translate(Vector3.forward * speed * Time.deltaTime);

        if (time >= 7.0f)
            Destroy(gameObject);
	}

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Enemy5aim")
        {
            other.GetComponent<enemy5anim>().Damage(10);
        }

        if (other.tag == "Enemy4aim")
        {
            other.GetComponent<enemy4anim>().Damage(10);
        }

        if (other.tag == "Enemy")
        {
            GameObject.Find("Boss").GetComponent<Boss>().Damage(10);
        }
    }
}
