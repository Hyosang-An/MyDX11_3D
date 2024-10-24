#pragma once
#include "CEntity.h"

#include "CGameObject.h"

class CComponent :
    public CEntity
{
    friend class CGameObject; friend class CLevelSaveLoad;
public:
    CComponent(COMPONENT_TYPE _Type);
    CComponent(const CComponent& _Origin);
    ~CComponent();
    virtual CComponent* Clone() = 0;

private:
    const COMPONENT_TYPE    m_Type;
    CGameObject*            m_Owner;

public:
    COMPONENT_TYPE GetComponentType() { return m_Type; }
    CGameObject* GetOwner() { return m_Owner; }

    virtual void Init() {}
    virtual void Begin();
    virtual void Tick();
    virtual void FinalTick() = 0;

    virtual void SaveToFile(FILE* _File) = 0;   // ���Ͽ� ������Ʈ�� ������ ����
    virtual void LoadFromFile(FILE* _File) = 0; // ���Ͽ� ������Ʈ�� ������ �ε�

    class CTransform* Transform() { return m_Owner->Transform(); }
    class CMeshRender* MeshRender() { return m_Owner->MeshRender(); }
    class CCamera* Camera() { return m_Owner->Camera(); }
    class CCollider2D* Collider2D() { return m_Owner->Collider2D(); }
    class CFlipBookComponent* FlipBookComponent() { return m_Owner->FlipBookComponent(); }
    class CLight2D* Light2D() { return m_Owner->Light2D(); }
	class CLight3D* Light3D() { return m_Owner->Light3D(); }
    class CParticleSystem* ParticleSystem() { return m_Owner->ParticleSystem(); }
	class CSkyBox* SkyBox() { return m_Owner->SkyBox(); }
	class CDecal* Decal() { return m_Owner->Decal(); }
    


private:
    void SetOwner(CGameObject* _obj) { m_Owner = _obj; }
};

