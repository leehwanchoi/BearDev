using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class spearParticleLife : MonoBehaviour
{
    private ParticleSystem ps;
    [SerializeField] Transform psParent;

    // Use this for initialization
    void Start()
    {
        ps = this.GetComponent<ParticleSystem>();
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (!ps.IsAlive())
            psParent.gameObject.SetActive(false);
    }
}
