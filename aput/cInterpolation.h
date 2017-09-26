#pragma once
class cInterpolation : public cObject
{
private:
	vector<ST_PC_VERTEX>	m_vecPCVertex;
	vector<D3DXVECTOR3>		m_vecTarget;

	D3DXVECTOR3				m_vFrom;
	D3DXVECTOR3				m_vMid;
	D3DXVECTOR3				m_vTo;

	float					m_fActionTime;
	float					m_fPassedTime;
	D3DXMATRIXA16			m_matWorld;
	int						m_nNextIndex;

public:
	cInterpolation();
	~cInterpolation();

	void Setup();
	void Update();
	void Render();

	void LinearInterpolation();
	void BezierInterpolation();

	void SetFrom(D3DXVECTOR3 v) { m_vFrom = v; }
	void SetMid(D3DXVECTOR3 v) { m_vMid = v; }
	void SetTo(D3DXVECTOR3 v) { m_vTo = v; }
	void SetActionTime(float t) { m_fActionTime = t; }
	D3DXMATRIXA16& GetWorldTransform() { return m_matWorld; }
};

