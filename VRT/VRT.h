#pragma once
using UInt = unsigned int;
class VRT
{
public:
	VRT() = default;
	~VRT();
	void Init();
	void Render();
	void RenderToTarget(float* _pData, UInt _x, UInt _y);
	void Finish();
	void GetTargetSize(UInt& x, UInt& y);
private:
	void InitShape();
	void InitShader();
	void InitTexture();
	void InitRenderTarget();

	UInt m_iVBO, m_iEBO, m_iVAO;
	
	UInt m_iShader;
	UInt m_iVolume;
	
	UInt m_iFBO;
	UInt m_iRBOs[2];
	UInt m_iTargetSize[2];
};

