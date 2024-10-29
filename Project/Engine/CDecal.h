#pragma once

#include "CRenderComponent.h"

class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_DecalTex;
    Ptr<CTexture>   m_EmissiveTex;


public:
    void SetDecalTexture(Ptr<CTexture> _Tex) { m_DecalTex = _Tex; }
    void SetEmissiveTexture(Ptr<CTexture> _Tex) { m_EmissiveTex = _Tex; }

public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual void SaveToFile(FILE* _File) override;   // 파일에 컴포넌트의 정보를 저장
    virtual void LoadFromFile(FILE* _File) override; // 파일에 컴포넌트의 정보를 로드

    CLONE(CDecal);

public:
    CDecal();
    ~CDecal();
};