using UnityEngine;
using System.Collections;

public class FollowCamera : MonoBehaviour {

    public GameObject target;   //카메라가 추적해야 하는 대상
    public float distance = 5f; //카메라가 추적해야 하는 대상와의 top 뷰에서의 거리
    public float height = 8f;   //카메라가 추적해야 하는 대상으로부터의 높이
    public float speed = 2f;    //카메라가 오브젝트를 따라갈 속도

    //지금의 카메라의 포지션과 목적지를 향하는 포지션이
    //다를 경우 부드러운 이동을 위해 둘 사이의 포지션을 
    //저장하기 위한 변수
    Vector3 pos;
    
	void Update ()
    {
        //임시 저장용 변수 pos 에 타겟의 포지션에서 아까 정한 distance와 height 만큼 떨어뜨린 포지션을 넣어 준뒤
        pos = new Vector3(target.transform.position.x, height, target.transform.position.z - distance);

        //MoveTowards 함수를 이용하면 카메라가 그쪽을 향해 이동하는 구문
        this.gameObject.transform.position =
            Vector3.MoveTowards(this.gameObject.transform.position, pos, speed * Time.deltaTime);
	}
}
