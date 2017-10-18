using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using UnityEngine.Networking;

public class GameManager_References : NetworkBehaviour
{
    private static GameManager_References sInstance;
    public static GameManager_References Instance
    {
        get
        {
            if (sInstance == null)
            {
                // 없으면 찾아서 넣는다.
                sInstance = GameObject.Find("GameManager").GetComponent<GameManager_References>();

                // 찾았는데도 없으면 만들어서 넣는다.
                if (sInstance == null)
                {
                    GameObject gObject = new GameObject("GameManager");
                    sInstance = gObject.AddComponent<GameManager_References>();
                }
            }
            return sInstance;
        }
    }

    public GameObject respawnButton;
    [SerializeField] GameObject pauseMenu;
    public GameObject resultMenu;
    [SerializeField] Text TotalKillCounter;

    private int GoalKillScore = 20;
    [HideInInspector] public int ZombieDeadCounter = 0;
    [HideInInspector] public bool isSurvived = false;

    [SyncVar] private int networkTotalCounter = 0;

    private void FixedUpdate()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            ToggleMainMenu();
        }

        if (ZombieDeadCounter >= GoalKillScore)
        {
            isSurvived = true;
            resultMenu.SetActive(true);
        }

        // 갱신
        RpcTransmitTotalCounter();
        SyncTotalKillCounter();

        // 출력
        TotalKillCounter.text = "TOTAL KILL: " + ZombieDeadCounter.ToString() + " Kills";
    }

    public void ToggleMainMenu()
    {
        pauseMenu.SetActive(!pauseMenu.activeSelf);
    }

    void SyncTotalKillCounter()
    {
        if (!isLocalPlayer)
        {
            ZombieDeadCounter = networkTotalCounter;
        }
    }

    [Command]
    void CmdProvideTotalCounterToServer(int count)
    {
        networkTotalCounter = count;
    }

    [ClientRpc]
    void RpcTransmitTotalCounter()
    {
        if (isServer)
        {
            CmdProvideTotalCounterToServer(ZombieDeadCounter);
        }
    }
}
