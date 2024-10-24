#pragma once
#include "CRenderComponent.h"

enum SKYBOX_TYPE
{
    SPHERE,
    CUBE,
};

class CSkyBox :
    public CRenderComponent
{
private:
    SKYBOX_TYPE     m_Type;
    Ptr<CTexture>   m_SkyBoxTex;

public:
    void SetSkyBoxType(SKYBOX_TYPE _Type);
    void SetSkyBoxTexture(Ptr<CTexture> _Tex) { m_SkyBoxTex = _Tex; }

public:
    virtual void FinalTick() override;
    virtual void Render() override;

    virtual void SaveToFile(FILE* _File) override;   // 파일에 컴포넌트의 정보를 저장
    virtual void LoadFromFile(FILE* _File) override; // 파일에 컴포넌트의 정보를 로드
    CLONE(CSkyBox);
public:
    CSkyBox();
    CSkyBox(const CSkyBox& _Origin);
    ~CSkyBox();
};

