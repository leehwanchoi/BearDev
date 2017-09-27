using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Timeline;

public class TimeLineAnimation : MonoBehaviour
{
    private PlayerMovement playerMovement;
    private PlayerCollision playerCollision;
    private Camera MainCamera;
    private Camera TimeLineCamera;

    public GameObject HPBar;
    public GameObject BulletCounter;
    public GameObject Joystick;
    public GameObject Reload;
    public GameObject Player;

    private GamePlayManager GPManager;

    private bool isOncePlay_Human;
    private bool isOncePlay_Car;
    public Animator TimelineAnimator_Human;
    public Animator TimelineAnimator_Car;

    public GameObject TimelineHuman;
    public GameObject TimelineCar;

    [HideInInspector] public bool isEnding;

    private void Awake()
    {
        playerMovement = GameObject.Find("Player").GetComponent<PlayerMovement>();
        playerCollision = GameObject.Find("PlayerCollider").GetComponent<PlayerCollision>();

        MainCamera = GameObject.Find("Main Camera").GetComponent<Camera>();
        TimeLineCamera = GameObject.Find("TimeLineCamera").GetComponent<Camera>();
        TimeLineCamera.GetComponent<AudioListener>().enabled = false;

        GPManager = GameObject.Find("GameManager").GetComponent<GamePlayManager>();

        isOncePlay_Human = false;
        isOncePlay_Car = false;

        TimelineAnimator_Car.enabled = false;

        isEnding = false;
    }

    private void FixedUpdate()
    {
        if (GPManager.stageState == StageState.Stage04)
        {
            playerMovement.enabled = false;
            playerCollision.enabled = false;
            MainCamera.enabled = false;
            MainCamera.GetComponent<AudioListener>().enabled = false;
            HPBar.SetActive(false);
            BulletCounter.SetActive(false);
            Joystick.SetActive(false);
            Reload.SetActive(false);
            Player.SetActive(false);

            TimeLineCamera.enabled = true;
            TimeLineCamera.GetComponent<AudioListener>().enabled = true;

            if (!isOncePlay_Human)
            {
                TimelineAnimator_Human.Play("Human");
                isOncePlay_Human = true;
            }

            if (isOncePlay_Human && TimelineHuman.gameObject.activeSelf && TimelineAnimator_Human.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1f)
            {
                TimelineHuman.SetActive(false);
                TimelineAnimator_Car.enabled = true;
                TimelineAnimator_Car.Play("Car");
                isOncePlay_Car = true;
            }

            if (isOncePlay_Car && TimelineCar.gameObject.activeSelf && TimelineAnimator_Car.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1f)
            {
                TimelineCar.SetActive(false);
                isEnding = true;
            }
        }
    }
}