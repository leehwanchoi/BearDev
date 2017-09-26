#pragma once
#include "cMtlTex.h"

struct ST_ASE_SCENE
{
	int nFirstFrame;
	int nLastFrame;
	int nFrameSpeed;
	int nTicksPerFrame;
};

struct ST_POS_SAMPLE
{
	int			n;	// ������ ��
	D3DXVECTOR3 v;	// �ش� �����ӿ����� ��ġ ��
};

struct ST_ROT_SAMPLE
{
	int				n;	// ������ ��
	D3DXQUATERNION	q;	// �ش� �����ӿ����� ���ʹϿ� ��
};

class cFrame : public cObject
{
private:
	SYNTHESIZE_PASS_BY_REF(D3DXMATRIXA16, m_matWorldTM, WorldTM);
	SYNTHESIZE_PASS_BY_REF(vector<ST_PNT_VERTEX>, m_vecVertex, Vertex);
	SYNTHESIZE_ADD_REF(cMtlTex*, m_pMtlTex, MtlTex);

	vector<cFrame*>			m_vecChild;

	D3DXMATRIXA16			m_matLocalTM;
	vector<ST_POS_SAMPLE>	m_vecPosTrack;
	vector<ST_ROT_SAMPLE>	m_vecRotTrack;

public:
	cFrame();
	~cFrame();

	void AddChild(cFrame* pChild);
	void CalcLocalTM(cFrame* pParent);

	void Update(int nKeyFrame, D3DXMATRIXA16* pParent);
	void CalcLocalTranslationMatrix(IN int nKeyFrame, OUT D3DXMATRIXA16& mat);
	void CalcLocalRotationMatrix(IN int nKeyFrame, OUT D3DXMATRIXA16& mat);

	void Render();
};

