#pragma once
#include "CComponent.h"

struct tMtrlSet
{
    Ptr<CMaterial>  pSharedMtrl;    // 공유 메테리얼(마스터)
    Ptr<CMaterial>  pDynamicMtrl;   // 공유 메테리얼의 복사본    
    Ptr<CMaterial>  pCurMtrl;       // 현재 사용 중인 메테리얼
};

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
    vector<tMtrlSet>    m_vecMtrls; // 재질 

    bool                m_FrustumCheck = true; // 절두체 체크를 받을것인지 말것인지

public:
    void SetMesh(Ptr<CMesh> _Mesh);
    Ptr<CMesh> GetMesh() { return m_Mesh; }

    void SetMaterial(Ptr<CMaterial> _Mtrl, UINT _idx = 0);
    Ptr<CMaterial> GetMaterial(UINT _idx = 0);
    Ptr<CMaterial> GetSharedMaterial(UINT _idx = 0);

    // 동적재질 생성 및 반환
    Ptr<CMaterial> GetDynamicMaterial(UINT _idx = 0);

    UINT GetMaterialCount() { return (UINT)m_vecMtrls.size(); }

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

