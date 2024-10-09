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

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetMaterial(Ptr<CMaterial> _mtrl);

    Ptr<CMesh> GetMesh() { return m_Mesh; }
    Ptr<CMaterial> GetMaterial() { return m_Mtrl; }
    Ptr<CMaterial> GetSharedMtrl();

    // 동적재질 생성 및 반환
    Ptr<CMaterial> GetDynamicMaterial();


public:
    virtual void FinalTick() = 0;
    virtual void Render() = 0;

protected:
    void SaveDataToFile(FILE* _File);
    void LoadDataFromFile(FILE* _File);
};

