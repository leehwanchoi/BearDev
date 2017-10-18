using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// Music
// 0: BGM

// SFX
// 1: Player  (타격음, 발걸음)
// 2: Player  (피격음)
// 3: Enemy   (타격음, 발걸음)
// 4: Enemy   (피격음)
// 5: UI & ETC

public enum ASN // Audio Source Number
{
    BGM             = 0, // 0: BGM
    Player_Hit_Walk = 1, // 1: Player  (타격음, 발걸음)
    Player_Damaged  = 2, // 2: Player  (피격음)
    Enemy_Hit_Walk  = 3, // 3: Enemy   (타격음, 발걸음)
    Enemy_Damaged   = 4, // 4: Enemy   (피격음)
    Enemy_Hit_Walk2 = 5,
    UI_ETC          = 6  // 5: UI & ETC
};

[RequireComponent(typeof(AudioSource))]
public class SoundManager : MonoBehaviour
{
    private AudioSource[] audioSources;
    private int audioSourceNum = 6;

    // Music
    // 0: BGM

    // SFX
    // 1: Player  (타격음, 발걸음)
    // 2: Player  (피격음)
    // 3: Enemy   (타격음, 발걸음)
    // 4: Enemy   (피격음)
    // 5: UI & ETC

    private static SoundManager sInstance;
    public static SoundManager Instance
    {
        get
        {
            if (sInstance == null)
            {
                // 없으면 찾아서 넣는다. (SoundControl에서 만들기 때문에 이부분은 현재 필요없음)
                //sInstance = GameObject.Find("_SoundManager").GetComponent<SoundManager>();

                // 찾았는데도 없으면 만들어서 넣는다.
                if (sInstance == null)
                {
                    GameObject gObject = new GameObject("_SoundManager");
                    sInstance = gObject.AddComponent<SoundManager>();
                }
            }
            return sInstance;
        }
    }

    private void Awake()
    {
        //audioSourceNum 만큼 생성
        for (int i = 0; i < audioSourceNum; i++)
        {
            this.gameObject.AddComponent<AudioSource>();
        }

        audioSources = this.GetComponents<AudioSource>();

        audioSources[0].loop = true; // 0번은 BGM이라 loop

        DontDestroyOnLoad(this.gameObject);
    }

    public void PlayMusic(AudioClip clip)
    {
        audioSources[0].clip = clip;
        audioSources[0].Play();
    }

    public void PlayRandomMusic(AudioClip[] clips)
    {
        PlayMusic(clips[Random.Range(0, clips.Length)]);
    }

    public void StopMusic()
    {
        audioSources[0].Stop();
    }

    public void PauseMusic()
    {
        audioSources[0].Pause();
    }

    public void UnPauseMusic()
    {
        audioSources[0].UnPause();
    }

    public bool IsMusicPlaying()
    {
        return audioSources[0].isPlaying;
    }

    public bool IsSameMusic(AudioClip clip)
    {
        if (audioSources[0].clip == clip)
            return true;
        else
            return false;
    }

    public void MusicVolumeUp()
    {
        if (audioSources[0].volume < 1.0f)
            audioSources[0].volume += 0.1f;
    }

    public void MusicVolumeDown()
    {
        if (audioSources[0].volume > 0.0f)
            audioSources[0].volume -= 0.1f;
    }

    public void SetMusicVolume(float volume)
    {
        if (volume >= 0.0f && volume <= 1.0f)
            audioSources[0].volume = volume;
    }

    public float lowPitchRange = 0.1f;
    public float highPitchRange = 3.0f;

    public void PlayEfx(int index, AudioClip clip)
    {
        //audioSources[1].pitch = Random.Range(lowPitchRange, highPitchRange);

        audioSources[index].clip = clip;
        audioSources[index].Play();
    }

    //public void PlayRandomEfx(AudioClip[] clips)
    //{
    //    PlayEfx(clips[Random.Range(0, clips.Length)]);
    //}

    public void StopEfx(int index)
    {
        audioSources[index].Stop();
    }

    public void PauseEfx(int index)
    {
        audioSources[index].Pause();
    }

    public void UnPauseEfx(int index)
    {
        audioSources[index].UnPause();
    }

    public bool IsEfxPlaying(int index)
    {
        return audioSources[index].isPlaying;
    }

    public bool IsSameEfx(int index, AudioClip clip)
    {
        if (audioSources[index].clip == clip)
            return true;
        else
            return false;
    }

    public void SetEfxVolume(int index, float volume)
    {
        if (volume >= 0.0f && volume <= 1.0f)
            audioSources[index].volume = volume;
    }

    public float GetEfxCurClipTime(int index)
    {
        return audioSources[index].time;
    }

    public void SetEfxPitch(int index, float pitch)
    {
        audioSources[index].pitch = pitch;
    }
}
