using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;

public class Latency : NetworkBehaviour
{
    private NetworkManager networkManger;
    private NetworkClient nClient;
    private int latency;
    private Text latencyText;

    private void Start()
    {
        if (!isLocalPlayer)
            return;

        networkManger = GameObject.Find("NetworkManager").GetComponent<NetworkManager>();
        nClient = networkManger.client;
        latencyText = GameObject.Find("Latency Text").GetComponent<Text>();
    }

    // Update is called once per frame
    private void FixedUpdate()
    {
        if (!isLocalPlayer)
            return;

        ShowLatency();
    }

    private void ShowLatency()
    {
        latency = nClient.GetRTT();
        latencyText.text = "Ping: " + latency.ToString();
    }
}