using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Movement : MonoBehaviour
{
    public float moveSpeed = 5;
    public float moveSpeedSmooth = 0.3f;
    public float rotateSpeed = 50;
    public float rotationSpeedSmooth = 0.3f;
    public float jumpSpeed = 5;
    public float gravity = 9.8f;

    float currentForwardSpeed;
    float currentSideSpeed;
    float forwardSpeedV;
    float sideSpeedV;

    float targetRotation;
    float currentRotation;
    float rotationV;

    CharacterController controller;
    Vector3 currentMovement;

    public float drag = 0.5f;
    public float terminalRotationSpeed = 25.0f;
    public Vector3 MoveVector { set; get; }
    public VirtualJoyStick joystick;

    private Rigidbody thisRigidbody;

	// Use this for initialization
	private void Start ()
    {
        thisRigidbody = this.GetComponent<Rigidbody>();
        thisRigidbody.maxAngularVelocity = terminalRotationSpeed;
        thisRigidbody.drag = drag;

        controller = GetComponent<CharacterController>();
    }
	
	// Update is called once per frame
	private void Update ()
    {
        //MoveVector = PoolInput();

        Move();
	}

    private void Move()
    {
        targetRotation += Input.GetAxisRaw("Vertical") * rotateSpeed * Time.deltaTime;

        if (targetRotation > 360)
            targetRotation -= 360;
        if (targetRotation < 0)
            targetRotation += 360;
        currentRotation = Mathf.SmoothDampAngle(currentRotation, targetRotation, ref rotationV, rotationSpeedSmooth);
        transform.eulerAngles = new Vector3(0, currentRotation, 0);

        currentForwardSpeed = Mathf.SmoothDamp(currentForwardSpeed, joystick.Vertical() * moveSpeed, ref forwardSpeedV, moveSpeedSmooth);
        currentSideSpeed = Mathf.SmoothDamp(currentSideSpeed, joystick.Horizontal() * moveSpeed, ref sideSpeedV, moveSpeedSmooth);

        currentMovement = new Vector3(currentSideSpeed, currentMovement.y, currentForwardSpeed);
        currentMovement = transform.rotation * currentMovement;

        if (!controller.isGrounded)
            currentMovement -= new Vector3(0, gravity * Time.deltaTime);
        else
            currentMovement.y = 0;

        if (controller.isGrounded && Input.GetButtonDown("Jump"))
            currentMovement.y = jumpSpeed;

        controller.Move(currentMovement * Time.deltaTime);
    }

    private Vector3 PoolInput()
    {
        Vector3 dir = Vector3.zero;

        //dir.x = Input.GetAxis("Horizontal");
        //dir.z = Input.GetAxis("Vertical");

        dir.x = joystick.Horizontal();
        dir.z = joystick.Vertical();

        if (dir.magnitude > 1)
            dir.Normalize();

        return dir;
    }
}
