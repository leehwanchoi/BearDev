using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
using UnityEngine.AI;

public class Zombie_Health : NetworkBehaviour {

    private int health = 50;
    [HideInInspector] public bool isDie = false;

    private void Start()
    {
        if (!isLocalPlayer)
            return;

    }

	public void DeductHealth (int dmg)
	{
        health -= dmg;
		CheckHealth();
	}

    private void CheckHealth()
	{
        if (health <= 0)
        {
            isDie = true;
        }
    }

    public int GetHP()
    {
        return health;
    }
}
