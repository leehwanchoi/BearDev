#pragma once
class cMtlTex : public cObject
{
private:
	SYNTHESIZE_PASS_BY_REF(D3DMATERIAL9, m_stMtl, Mtl);
	SYNTHESIZE_ADD_REF(LPDIRECT3DTEXTURE9, m_pTexture, Texture);
	SYNTHESIZE(DWORD, m_nMtlRef, MtlRef);

	//D3DMATERIAL9 m_stMtl;
	//LPDIRECT3DTEXTURE9 m_pTexture;

public:
	cMtlTex();
	~cMtlTex();

	//D3DMATERIAL9& GetMtl() { return m_stMtl; }
	//void SetMtl(D3DMATERIAL9& mtl) { m_stMtl = mtl; }

	//LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; }
	//void SetTexture(LPDIRECT3DTEXTURE9 pTexture)
	//{
	//	if (m_pTexture != pTexture)
	//	{
	//		SAFE_RELEASE(m_pTexture);
	//		SAFE_ADDREF(pTexture);
	//		m_pTexture = pTexture;
	//	}
	//}
};

