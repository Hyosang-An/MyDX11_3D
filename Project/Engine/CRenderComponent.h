#pragma once
#include "CComponent.h"


class CRenderComponent :
    public CComponent
{
public:
    CRenderComponent(COMPONENT_TYPE _Type);
    CRenderComponent(const CRenderComponent& _Origin);
    ~CRenderComponent();
    virtual CRenderComponent* Clone() = 0;

private:
    Ptr<CMesh>          m_Mesh;
    
    Ptr<CMaterial>      m_Mtrl;         // 현재 사용중인 재질
    Ptr<CMaterial>      m_SharedMtrl;   // 공유 재질(마스터)
    Ptr<CMaterial>      m_DynamicMtrl;  // 임시 재질

    bool                m_FrustumCheck = true; // 절두체 체크를 받을것인지 말것인지

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetMaterial(Ptr<CMaterial> _mtrl);

    Ptr<CMesh> GetMesh() { return m_Mesh; }
    Ptr<CMaterial> GetMaterial() { return m_Mtrl; }
    Ptr<CMaterial> GetSharedMtrl();

    // 동적재질 생성 및 반환
    Ptr<CMaterial> GetDynamicMaterial();

    void SetFrustumCheck(bool _Check) { m_FrustumCheck = _Check; }
    bool IsFrustumCheck() { return m_FrustumCheck; }

public:
    virtual void FinalTick() = 0;
    virtual void Render() = 0;
    virtual void render_shadowmap();

    virtual void SaveToFile(FILE* _File) = 0;   // 파일에 컴포넌트의 정보를 저장
    virtual void LoadFromFile(FILE* _File) = 0; // 파일에 컴포넌트의 정보를 로드

protected:
    void SaveAssetDataToFile(FILE* _File);
    void LoadAssetDataFromFile(FILE* _File);
};

