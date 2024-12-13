#pragma once
#include "CComponent.h"

struct tMtrlSet
{
    Ptr<CMaterial>  pSharedMtrl;    // ���� ���׸���(������)
    Ptr<CMaterial>  pDynamicMtrl;   // ���� ���׸����� ���纻    
    Ptr<CMaterial>  pCurMtrl;       // ���� ��� ���� ���׸���
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
    vector<tMtrlSet>    m_vecMtrls; // ���� 

    bool                m_FrustumCheck = true; // ����ü üũ�� ���������� ��������

public:
    void SetMesh(Ptr<CMesh> _Mesh);
    Ptr<CMesh> GetMesh() { return m_Mesh; }

    void SetMaterial(Ptr<CMaterial> _Mtrl, UINT _idx = 0);
    Ptr<CMaterial> GetMaterial(UINT _idx = 0);
    Ptr<CMaterial> GetSharedMaterial(UINT _idx = 0);

    // �������� ���� �� ��ȯ
    Ptr<CMaterial> GetDynamicMaterial(UINT _idx = 0);

    UINT GetMaterialCount() { return (UINT)m_vecMtrls.size(); }

    void SetFrustumCheck(bool _Check) { m_FrustumCheck = _Check; }
    bool IsFrustumCheck() { return m_FrustumCheck; }

public:
    virtual void FinalTick() = 0;
    virtual void Render() = 0;
    virtual void render_shadowmap();

    virtual void SaveToFile(FILE* _File) = 0;   // ���Ͽ� ������Ʈ�� ������ ����
    virtual void LoadFromFile(FILE* _File) = 0; // ���Ͽ� ������Ʈ�� ������ �ε�

protected:
    void SaveAssetDataToFile(FILE* _File);
    void LoadAssetDataFromFile(FILE* _File);
};

