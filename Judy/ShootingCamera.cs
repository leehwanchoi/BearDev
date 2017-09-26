using UnityEngine;
using System.Collections;

public class ShootingCamera : MonoBehaviour
{
    public GameObject target;

    VJS vjs;
    Front_Collision f_Collision;
    enemyManager em;

    public float speed = 2f;
    float distance = 2f;

    Vector3 pos;

    void Start()
    {
        vjs = GameObject.Find("JoyStick_BackGround").GetComponent<VJS>();
        f_Collision = GameObject.Find("CollisionBox_F").GetComponent<Front_Collision>();
        em = GameObject.Find("Enemies").GetComponent<enemyManager>();
    }

    void Update ()
    {
        pos = new Vector3(this.gameObject.transform.position.x, this.gameObject.transform.position.y, target.transform.position.z/* + distance*/);

        if (f_Collision.isMove && vjs.Vertical() > 0 && !em.isAttack() && !em.cactusisDead())
        {
            this.gameObject.transform.position = Vector3.MoveTowards(this.gameObject.transform.position, pos, speed * Time.deltaTime);
        }
    }
}
