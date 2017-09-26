using UnityEngine;
using CnControls;

// Just in case so no "duplicate definition" stuff shows up
namespace UnityStandardAssets.Copy._2D
{
    [RequireComponent(typeof (ThidPersonExampleController1))]
    public class PlatformerUserControl1 : MonoBehaviour
    {
        private ThidPersonExampleController1 player;
        private bool m_Fire1;

        private void Awake()
        {
            player = GetComponent<ThidPersonExampleController1>();
        }
        
        private void Update()
        {
            if (!m_Fire1)
            {
                // Read the jump input in Update so button presses aren't missed.
                //m_Fire1 = CnInputManager.GetButtonDown("Fire1");
            }
        }
        
        private void FixedUpdate()
        {
            float h = CnInputManager.GetAxis("Horizontal");
            // Pass all parameters to the character control script.
            if (m_Fire1)
                player.MovementSpeed += 1;

            m_Fire1 = false;
        }
    }
}
