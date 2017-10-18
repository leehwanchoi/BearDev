using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(AudioSource))]
public class SoundControl : MonoBehaviour
{
    //private AudioSource audioSource;

    public AudioClip[] musicClips;              // 배경음
    public AudioClip[] playerHitEfxClips;       // 플레이어 타격음 & 발걸음
    public AudioClip[] playerDamagedEfxClips;   // 플레이어 피격음
    public AudioClip[] enemyHitEfxClips;        // 적 타격음 & 발걸음
    public AudioClip[] enemyDamagedEfxClips;    // 적 피격음
    public AudioClip[] UI_ETC;                  // UI 효과음

    private static SoundControl sInstance;
    public static SoundControl Instance
    {
        get
        {
            if (sInstance == null)
            {
                // 없으면 찾아서 넣는다.
                sInstance = GameObject.Find("SoundControl").GetComponent<SoundControl>();

                // 찾았는데도 없으면 만들어서 넣는다.
                if (sInstance == null)
                {
                    GameObject gObject = new GameObject("SoundControl");
                    sInstance = gObject.AddComponent<SoundControl>();
                }
            }
            return sInstance;
        }
    }

    private void Awake()
    {
        //audioSource = this.GetComponent<AudioSource>();

        SoundManager.Instance.SetMusicVolume(0.65f);
        // 배경음악 재생
        SoundManager.Instance.PlayMusic(musicClips[0]);

        // 사운드 매니저에 자식으로 넣는다.
        // 사운드 매니저는 삭제 되지 않기때문에 자식으로 넣으면 SoundControl도 삭제 되지 않는다.
        this.transform.SetParent(SoundManager.Instance.transform);
    }

    private void OnEnable()
    {
        if (!SoundManager.Instance.IsMusicPlaying())
            SoundManager.Instance.UnPauseMusic();
    }
}
