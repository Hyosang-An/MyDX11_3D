#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

struct tRaycastOut
{
	Vec2    Location;
	UINT	Distance;
	int		Success;
};

class CHeightMapCS :
    public CComputeShader
{
public:
	CHeightMapCS();
	~CHeightMapCS();

private:
	Ptr<CTexture>	m_HeightMapTex;
	Ptr<CTexture>	m_BrushTex;

	Vec2			m_BrushPos;
	Vec2			m_BrushScale;


public:
	void SetBrushPos(Vec2 _BrushPos) { m_BrushPos = _BrushPos; }
	void SetBrushScale(Vec2 _Scale) { m_BrushScale = _Scale; }

	void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMapTex = _HeightMap; }
	void SetBrushTex(Ptr<CTexture> _BrushTex) { m_BrushTex = _BrushTex; }

public:
	virtual int Binding() override;
	virtual void CalcGroupNum() override;
	virtual void Clear() override;
};

