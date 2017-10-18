using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BloodParticleLife : MonoBehaviour
{
    private ParticleSystem ps;

	// Use this for initialization
	void Start () {
        ps = this.GetComponent<ParticleSystem>();
	}
	
	// Update is called once per frame
	void FixedUpdate() {
        if (!ps.IsAlive())
            Destroy(this.gameObject);
	}
}