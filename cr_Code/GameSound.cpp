#include "stdafx.h"
#include "GameSound.h"

GameSound::GameSound() {}
GameSound::~GameSound() {}

HRESULT GameSound::init()
{
	/*Sound �߰�*/
	SOUNDMANAGER->addSound("MainGameMusic", "sound/The Witcher 3- Wild Hunt OST - Silver For Monsters.mp3", true, true);

	/*���������� ���� ȿ���� �߰�*/
	SOUNDMANAGER->addSound("Boom", "sound/������_����ź_1.wav", false, false);
	SOUNDMANAGER->addSound("button_BGM1", "sound/ui-sound-19.ogg", false, false);
	SOUNDMANAGER->addSound("button_BGM2", "sound/ui-sound-20.ogg", false, false);
	SOUNDMANAGER->addSound("button_BGM3", "sound/ui-sound4.ogg", false, false);

	/*���ӳ����� ���� ȿ���� �߰�*/
	//Į���� ���ݽ�
	SOUNDMANAGER->addSound("ATT_1", "sound/att-sound2-3.ogg", false, false);
	//Ȱ ���ݽ�
	SOUNDMANAGER->addSound("ATT_2", "sound/weapon-sound8.ogg", false, false);
	//�ǹ� ���ݽ�
	SOUNDMANAGER->addSound("ATT_3", "sound/att-sound2-7.ogg", false, false);
	//����
	SOUNDMANAGER->addSound("Death", "sound/Death.mp3", false, false);

	/*���п��� ȿ���� �߰�*/
	SOUNDMANAGER->addSound("WIN", "sound/You - Win.mp3", false, false);
	SOUNDMANAGER->addSound("LOSE", "sound/You - Lose.mp3", false, false);

	return S_OK;
}

void GameSound::release()
{
}

void GameSound::update()
{
}

void GameSound::render()
{
}

void GameSound::BackGroundSound()
{
	//if (SOUNDMANAGER->isPlaySound("MainGameMusic"))
	//{
	//	SOUNDMANAGER->stop("MainGameMusic");
	//	SOUNDMANAGER->play("MainGameMusic", 1.f);
	//}
	//else
		SOUNDMANAGER->play("MainGameMusic", 1.f);
}

void GameSound::MeleeHitSound()
{
	if (SOUNDMANAGER->isPlaySound("ATT_1"))
	{
		SOUNDMANAGER->stop("ATT_1");
		SOUNDMANAGER->play("ATT_1", 1.f);
	}
	else
		SOUNDMANAGER->play("ATT_1", 1.f);
}

void GameSound::RangeHitSound()
{
	if (SOUNDMANAGER->isPlaySound("ATT_2"))
	{
		SOUNDMANAGER->stop("ATT_2");
		SOUNDMANAGER->play("ATT_2", 1.f);
	}
	else
		SOUNDMANAGER->play("ATT_2", 1.f);
}

void GameSound::BuildHitSound()
{
	if (SOUNDMANAGER->isPlaySound("ATT_3"))
	{
		SOUNDMANAGER->stop("ATT_3");
		SOUNDMANAGER->play("ATT_3", 1.f);
	}
	else
		SOUNDMANAGER->play("ATT_3", 1.f);
}

void GameSound::ClickSound()
{
	if (SOUNDMANAGER->isPlaySound("button_BGM1"))
	{
		SOUNDMANAGER->stop("button_BGM1");
		SOUNDMANAGER->play("button_BGM1", 1.f);
	}
	else
		SOUNDMANAGER->play("button_BGM1", 1.f);
}

void GameSound::DestroyedSound()
{
	if (SOUNDMANAGER->isPlaySound("Boom"))
	{
		SOUNDMANAGER->stop("Boom");
		SOUNDMANAGER->play("Boom", 0.3f);
	}
	else
		SOUNDMANAGER->play("Boom", 0.3f);
}

void GameSound::WinSound()
{
	if (SOUNDMANAGER->isPlaySound("WIN"))
	{
		SOUNDMANAGER->stop("WIN");
		SOUNDMANAGER->play("WIN", 1.f);
	}
	else
		SOUNDMANAGER->play("WIN", 1.f);
}

void GameSound::LoseSound()
{
	if (SOUNDMANAGER->isPlaySound("LOSE"))
	{
		SOUNDMANAGER->stop("LOSE");
		SOUNDMANAGER->play("LOSE", 1.f);
	}
	else
		SOUNDMANAGER->play("LOSE", 1.f);
}