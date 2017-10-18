using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DisableBuidings : MonoBehaviour
{
    [SerializeField] Transform PlayerTransform;

    private Ray ray;
    private RaycastHit hit;
    private Vector3 addRayPos; // 레이 위치에서 좀더 높은 곳에서 쏴야 오브젝트를 감지할 수 있다. (오브젝트 위치보다 높아야함)

    private MeshRenderer buildingRenderer;
    private Shader restoreShader;
    private bool isAlreadyBuildHit; // 이미 빌딩을 히트한 상태인지 체크

    [SerializeField] Shader transparentShader; // 투명 쉐이더

    private void Start()
    {
        addRayPos = new Vector3(0.0f, 10.0f, -10.0f);
        isAlreadyBuildHit = false;
    }

    private void Update()
    {
        //카메라 위치보다 y축 10.0f, z축 -10.0f 위에서 쏘는 레이
        ray.origin = this.transform.position + addRayPos;
        // 카메라에서 플레이어를 바라보는 방향
        ray.direction = PlayerTransform.position - this.transform.position;

        if (Physics.Raycast(ray.origin, ray.direction, out hit))
        {
            /*==================================================================*/
            /*                 hit 체크용, 릴리즈 할때 지우자.                   */
            //Debug.Log(hit.transform.name + " Hit !");
            /*==================================================================*/

            // hit가 건물이면
            if (hit.transform.tag == "Building")
            {
                if (isAlreadyBuildHit) // 이미 Hit 상태면 기존 건물은 복구
                {
                    // 원본 쉐이더로 복구
                    buildingRenderer.material.shader = restoreShader;

                    isAlreadyBuildHit = false; // Hit 처리 끄기
                }

                buildingRenderer = hit.transform.GetComponent<MeshRenderer>();

                // 현재 빌딩 쉐이더가 투명쉐이더랑 다를경우에만 (즉, 원본값일때만)
                if (buildingRenderer.material.shader != transparentShader)
                    restoreShader = buildingRenderer.material.shader; // 복구할 원본값을 따로 저장한다.

                buildingRenderer.material.shader = transparentShader; // 현재 빌딩 쉐이더에 투명 쉐이더를 적용한다.

                isAlreadyBuildHit = true; // Hit 처리
            }
        }

        // hit가 건물이 아니면
        if (hit.transform.tag != "Building")
        {
            // hit된 빌딩이 있으면
            if (buildingRenderer != null)
            {
                // 원본 쉐이더로 복구
                buildingRenderer.material.shader = restoreShader;

                // Hit 처리 끄기
                isAlreadyBuildHit = false;
            }
        }
    }
}