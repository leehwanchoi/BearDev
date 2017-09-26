using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class loadingbar : MonoBehaviour
{
    AsyncOperation ao;

    LoadingButton button;
    public GameObject loadingScreenBG;
    public Slider progBar;
    public Text loadingText;

    public bool isFakeLoadingBar = false;
    public float fakeIncrement = 0f;
    public float fakeTiming = 0f;
    public string sceneName;

	// Use this for initialization
	void Start ()
    {
        LoadLevel(); // 자동실행. (수동으로 할때는 버튼에다가 넣어주면 됨.)
        button = GameObject.Find("Button").GetComponent<LoadingButton>(); //버튼 누르면 씬넘어가게 추가
    }
	
	// Update is called once per frame
	void Update ()
    {

    }

    public void LoadLevel()
    {
        loadingScreenBG.SetActive(true);
        progBar.gameObject.SetActive(true);
        loadingText.gameObject.SetActive(true);
        loadingText.text = "Loading...";

        if (!isFakeLoadingBar)
        {
            StartCoroutine(LoadLevelWithRealProgress());
        }
        else
        {
            StartCoroutine(LoadLevelWithFakeProgress());
        }
    }

    IEnumerator LoadLevelWithRealProgress()
    {
        yield return new WaitForSeconds(1);

        ao = SceneManager.LoadSceneAsync(sceneName);
        ao.allowSceneActivation = false;

        while(!ao.isDone)
        {
            progBar.value = ao.progress;
            GameObject.Find("LoadingBar").GetComponent<Image>().fillAmount += ao.progress;

            if (ao.progress == 0.9f)
            {
                progBar.value = 1f;
                loadingText.text = "Touch To Continue !";

                if (Input.GetKeyDown(KeyCode.F) || button.isDown)
                {
                    ao.allowSceneActivation = true;
                }
            }

            Debug.Log(ao.progress);
            yield return null;
        }
    }

    IEnumerator LoadLevelWithFakeProgress()
    {
        yield return new WaitForSeconds(1);

        while(progBar.value != 1f)
        {
            progBar.value += fakeIncrement;
            GameObject.Find("LoadingBar").GetComponent<Image>().fillAmount += fakeIncrement;
            yield return new WaitForSeconds(fakeTiming);
        }

        while(progBar.value == 1f)
        {
            loadingText.text = "Touch To Continue !";
            
            if (Input.GetKeyDown(KeyCode.F) || button.isDown)
            {
                SceneManager.LoadScene(sceneName);
            }

            yield return null;
        }
    }
}
