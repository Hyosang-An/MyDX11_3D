#pragma once
#include "CComputeShader.h"
class CSetColorCS :
    public CComputeShader
{
private:
    Ptr<CTexture>       m_TargetTex;
    Vec4                m_ClearColor;

public:
    void SetTargetTexture(Ptr<CTexture> _Tex) { m_TargetTex = _Tex; }
    void SetClearColor(Vec4 _Color) { m_ClearColor = _Color; }

public:
    virtual int Binding() override;
    virtual void CalcGroupNum() override;
    virtual void Clear() override;

public:
    CSetColorCS();
    ~CSetColorCS();
};

