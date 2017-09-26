using UnityEngine;
using UnityEngine.EventSystems;

namespace CnControls
{
    /// <summary>
    /// Simple button class
    /// Handles press, hold and release, just like a normal button
    /// </summary>
    public class SimpleButtonA1 : MonoBehaviour, IPointerUpHandler, IPointerDownHandler
    {
        /// <summary>
        /// The name of the button
        /// </summary>
        public string ButtonName = "Fire1";

        /// <summary>
        /// Utility object that is registered in the system
        /// </summary>
        private VirtualButton _virtualButton;

        private ThidPersonExampleController1 player;

        private void Awake()
        {
            player = GameObject.Find("Judy").GetComponent<ThidPersonExampleController1>();
        }

        /// <summary>
        /// It's pretty simple here
        /// When we enable, we register our button in the input system
        /// </summary>
        private void OnEnable()
        {
            _virtualButton = _virtualButton ?? new VirtualButton(ButtonName);
            CnInputManager.RegisterVirtualButton(_virtualButton);
        }

        /// <summary>
        /// When we disable, we unregister our button
        /// </summary>
        private void OnDisable()
        {
            CnInputManager.UnregisterVirtualButton(_virtualButton);
        }

        /// <summary>
        /// uGUI Event system stuff
        /// It's also utilised by the editor input helper
        /// </summary>
        /// <param name="eventData">Data of the passed event</param>
        public void OnPointerUp(PointerEventData eventData)
        {
            _virtualButton.Release();
        }

        /// <summary>
        /// uGUI Event system stuff
        /// It's also utilised by the editor input helper
        /// </summary>
        /// <param name="eventData">Data of the passed event</param>
        public void OnPointerDown(PointerEventData eventData)
        {
            _virtualButton.Press();

            Debug.Log("A Pressed");

            player.MovementSpeed += 1;
        }
    }
}