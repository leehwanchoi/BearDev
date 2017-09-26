#include "stdafx.h"
#include "cCreditScene.h"
#include "cUIObject.h"
#include "cUIImageView.h"
#include "cUITextView.h"

cCreditScene::cCreditScene()
: m_pUIRoot(NULL)
, m_pSprite(NULL)
, m_vCreditPos(0, 0, 0)
{
}


cCreditScene::~cCreditScene()
{
}

HRESULT cCreditScene::init()
{
	D3DXCreateSprite(g_pD3DDevice, &m_pSprite);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO	   stImageInfo;

	char* str = "./UI/creditBackground.jpg";
	g_pTextureManager->AddTexture(str, str, &pTexture, &stImageInfo);

	cUIImageView* pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(0, 0, 0));
	pImageView->Render(m_pSprite);
	m_pUIRoot = pImageView;

	cUIButton* pButton = NULL;
	pButton = new cUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./UI/back.png", "./UI/back - up.png", "./UI/back - down.png");
	pButton->SetLocalPos(D3DXVECTOR3(25, WINSIZEY - (pButton->GetSize().fHeight + 25), 0));
	pButton->SetSize(ST_SIZE(120, 30));
	pButton->SetTag(E_CREDITBUTTON1);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//Credit Text Setting
	m_sCredit = \
		"\
				CREDIT\n\
						\nDirectX 9.0c & FMOD\
								\n\
										\n\
												\nDirector\
														\nChang-in Choi\n\
																\n\
																		\nGamePlay & AI & Animation & SFX Programmer\
																				\nDo-Yeon Kim\n\
																						\n\
																								\nMapTool & FX & UI & FrameWork & BGM Programmer\
																										\nChang-in Choi\n\
																												\n\
																														\nUI Programmer & Resource Researcher\
																																\nSu-ho Choi\
																																		\n\
																																				\n\
																																						\n\
																																								\n\
																																										\n\
																																												\n\
																																														\nSpecial Thanks\
																																																\n\
																																																		\nResource By opengameart.org\
																																																				\n\
																																																						\nGraphic Resource\n\
																																																								\n\
																																																										\nPINKCANNON\n\
																																																												\nCTatz\n\
																																																														\ngfx0\n\
																																																																\nabetusk\n\
																																																																		\nThe Chayed - KIIRA\n\
																																																																				\nSkorpio\n\
																																																																						\ntxturs\n\
																																																																								\n\
																																																																										\nSound Resource\
																																																																												\n\
																																																																														\npheonton\n\
																																																																																\nSketchyLogic\n\
																																																																																		\nbart\n\
																																																																																				\n\
																																																																																						\n\
																																																																																								\n\
																																																																																										\n\
																																																																																												\n\
																																																																																														\n\
																																																																																																\n\
																																																																																																		\n\
																																																																																																				\n\
																																																																																																						\n\
																																																																																																								\n\
																																																																																																										\n\
																																																																																																												\n\
																																																																																																														\n\
																																																																																																																\n\
																																																																																																																		\n\
																																																																																																																				\nThanks for Playing Our First Game.\
																																																																																																																						";

	cUITextView* pTextView = new cUITextView;
	pTextView->AutoRelease();
	pTextView->SetText(m_sCredit);
	pTextView->SetSize(ST_SIZE(1366, 1800));
	m_vCreditPos = D3DXVECTOR3(WINSIZEX / 2 - pTextView->GetSize().fWidth / 2, WINSIZEY, 0);	//Credit Position Setting
	pTextView->SetLocalPos(m_vCreditPos);
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(128, 128, 128));
	pTextView->SetTag(E_CREDITTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	//사운드
	//타이틀 BGM 정지
	g_pSoundManager->stop("./Sound/Title/one_0.mp3");

	//크레딧 BGM 재생
	g_pSoundManager->addSound("./Sound/Credit/SwingingLevel.wav", "./Sound/Credit/SwingingLevel.wav", true, true);
	g_pSoundManager->play("./Sound/Credit/SwingingLevel.wav", 0.6f);

	return S_OK;
}

void cCreditScene::update()
{
	UpCredit();

	if (m_pUIRoot)
		m_pUIRoot->Update();
}

void cCreditScene::Release()
{
	SAFE_RELEASE(m_pSprite);
}

void cCreditScene::render()
{
	if (m_pUIRoot)
		m_pUIRoot->Render(m_pSprite);
}

void cCreditScene::OnClick(cUIButton* pSender)
{
	if (pSender->GetTag() == E_CREDITBUTTON1)
		g_pSceneManager->changeScene("TitleScene");
}

void cCreditScene::UpCredit()
{
	cUITextView* pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_CREDITTEXTVIEW);

	//위치 이동
	m_vCreditPos.x = 0.0f;
	m_vCreditPos.y -= 30.0f * g_pTimeManager->getElapsedTime();
	m_vCreditPos.z = 0.0f;

	pTextView->SetLocalPos(m_vCreditPos);

	//재생 끝나면 다시 초기화
	if (m_vCreditPos.y + pTextView->GetSize().fHeight < 0)
		m_vCreditPos.y = WINSIZEY;
}