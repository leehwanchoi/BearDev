#pragma once
#include <string>
class cUIObject;

//최소 2개 이상 있어야함.
enum
{
	None = 0,
	E_AchievementTEXT
};

class cAchievementBox : public cObject
{
private:
	LPD3DXSPRITE			m_pSprite2;
	D3DXIMAGE_INFO			m_stImageInfo;
	cUIObject*				m_pUIRoot;

	bool					m_isQuestSuccess;//퀘스트 성공했는지 확인
	string					m_sQuestName;	 //퀘스트 이름

	bool					m_isAnimationEnd;//업적 애니메이션 확인
	float					m_fCount;		 //시간 계산
	float					m_fTime;		 //업적 보여줄 시간
	int						m_nDir;			 //업적바 움직이는 방향 (y좌표)

	int						m_nPlayCnt;

	//초기 위치값 설정
	void	InitPosition();

public:
	cAchievementBox();
	~cAchievementBox();

	void Setup();
	void Update(D3DXVECTOR3* pos = NULL);
	void Render();

	//퀘스트 완료했는지 설정하는 함수 (이 함수를 true해서 업적을 실행시킨다.)
	void SetisisQuestSuccess(bool isQuestSuccess) { m_isQuestSuccess = isQuestSuccess; }
	//완료한 퀘스트 이름
	void SetQuestName(string QuestName);

	//퀘스트바 움직이게 하는 함수
	void AchievementBarUpdate();

	void SetPlayCnt(int cnt){ m_nPlayCnt = cnt; }
};

