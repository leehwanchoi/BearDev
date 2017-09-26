#pragma once
class cRay
{
private:
	enum eRaySpace
	{
		E_NONE,
		E_VIEW,
		E_WORLD
	};
	
	eRaySpace m_eRaySpace;

public:
	D3DXVECTOR3			m_vOrg;
	D3DXVECTOR3			m_vDir;

public:
	cRay();
	~cRay();

	static cRay RayAtViewSpace(int x, int y);
	static cRay RayAtWorldSpace(int x, int y);
	bool IsPicked(ST_SPHERE* pSphere);
};

/*
반직선: P(t) = Q + Vt	// Q 반직선의 출발점, V 반직선의 방향
P(x) = Qx + Vxt, P(y) = Qy + Vyt, P(z) = Qz + Vzt

구의 방정식: x^2 + y^2 + z^2 = r^2  <= (x + a)^2 + (y + b)^2 + (z +c)^2 = r^2

구와 반직선을 연립 방정식으로 풀어낸다. (x + 1)^2 = x^2 + 2x + 1
(Qx + Vxt)^2 + (Qy + Vyt)^2 + (Qz + Vzt)^2 = r^2
(Qx)^2 + 2QxVxt + (Vxt)^2 + ... + ... - r^2 = 0
(Vx + Vy + Vz)^2 * t^2 + 2(QxVx + QyVy + QzVz)t + (Qx)^2 + (Qy)^2 + (Qz)^2 - r^2 = 0
(VV)t^2 + 2(QV)t + (Qx)^2 + (Qy)^2 + (Qz)^2 - r^2 = 0
(VV)t^2 + 2(QV)t + Q^2 - r^2 = 0

a = VV
b = 2QV
c = Q^2 - r^2

ax^2 + bx + c = 0;
( -b +- sqrt(b^2 - 4ac) ) / 2a
판별식: b^2 - 4ac

(2QV)^2 - 4(VV)(Q^2 - r^2) >= 0 => 근이 존재한다.(교점이 존재한다.)
4 * (QV)^2 - 4(VV)(Q^2 - r^2) >= 0
(QV)(QV) - (VV)(QQ - rr) >= 0, 벡터x벡터 = dot 프로덕트로 계산하면 된다.
*/