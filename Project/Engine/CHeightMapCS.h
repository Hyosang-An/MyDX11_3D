#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CHeightMapCS :
    public CComputeShader
{
public:
	CHeightMapCS();
	~CHeightMapCS();

private:
	Ptr<CTexture>	m_HeightMapTex;
	Ptr<CTexture>	m_BrushTex;

	CStructuredBuffer* m_RaycastOut = nullptr;
	Vec2			m_BrushScale;


public:
	void SetBrushPos(CStructuredBuffer* _Buffer) { m_RaycastOut = _Buffer; }
	void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }

	void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMapTex = _HeightMap; }
	void SetBrushTex(Ptr<CTexture> _BrushTex) { m_BrushTex = _BrushTex; }

public:
	virtual int Binding() override;
	virtual void CalcGroupNum() override;
	virtual void Clear() override;
};

