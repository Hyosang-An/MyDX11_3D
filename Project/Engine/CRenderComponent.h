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
    
    Ptr<CMaterial>      m_Mtrl;         // ���� ������� ����
    Ptr<CMaterial>      m_SharedMtrl;   // ���� ����(������)
    Ptr<CMaterial>      m_DynamicMtrl;  // �ӽ� ����

    bool                m_FrustumCheck = true; // ����ü üũ�� ���������� ��������

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetMaterial(Ptr<CMaterial> _mtrl);

    Ptr<CMesh> GetMesh() { return m_Mesh; }
    Ptr<CMaterial> GetMaterial() { return m_Mtrl; }
    Ptr<CMaterial> GetSharedMtrl();

    // �������� ���� �� ��ȯ
    Ptr<CMaterial> GetDynamicMaterial();

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

