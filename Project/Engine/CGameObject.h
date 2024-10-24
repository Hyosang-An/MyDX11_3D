#pragma once
#include "CEntity.h"

class CComponent;
class CRenderComponent;
class CScript;

class CGameObject :
    public CEntity
{
    friend class CLevel;
    friend class CLayer;
    friend class CTaskMgr;

public:
    CGameObject();
    CGameObject(const CGameObject& _Origin);
    ~CGameObject();
    virtual CGameObject* Clone() { return new CGameObject(*this); }

private:
    CComponent*             m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*       m_RenderCom;
    vector<CScript*>        m_vecScript;

    CGameObject*            m_Parent;
    vector<CGameObject*>    m_vecChildren;

    int                     m_LayerIdx = -1; // �Ҽ� ���̾��� �ε��� ��ȣ

    bool                    m_Dead = false;

public:
    void AddComponent(CComponent* _Comopnent);
    void AddChild(CGameObject* _ChildObject);

    CComponent* GetComponent(COMPONENT_TYPE _Type) { return m_arrCom[(UINT)_Type]; }
    CRenderComponent* GetRenderComponent() { return m_RenderCom; }
    CGameObject* GetParent() { return m_Parent; }
    const vector<CGameObject*>& GetChildren() { return m_vecChildren; }
    int GetLayerIdx() { return m_LayerIdx; }
    const vector<CScript*> GetScripts() { return m_vecScript; }

    bool IsAncestor(CGameObject* _Object);
    bool IsDead() { return m_Dead; }
    void DisconnectWithLayer();

    // Unregister : ����� ����ϴ�, ����ϴ� ������ ����ϴ�
    // Deregister : ��ϵȰ��� ����ϴ�
    void DeregisterThisChildFromParent();

    class CTransform* Transform() { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }
    class CMeshRender* MeshRender() { return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER); }
    class CCamera* Camera() { return (CCamera*)GetComponent(COMPONENT_TYPE::CAMERA); }
    class CCollider2D* Collider2D() { return (CCollider2D*)GetComponent(COMPONENT_TYPE::COLLIDER2D); }
    class CFlipBookComponent* FlipBookComponent() { return (CFlipBookComponent*)GetComponent(COMPONENT_TYPE::FLIPBOOK_COMPONENT); }
    class CTileMap* TileMap() { return (CTileMap*)GetComponent(COMPONENT_TYPE::TILEMAP); }
    class CLight2D* Light2D() { return (CLight2D*)GetComponent(COMPONENT_TYPE::LIGHT2D); }
	class CLight3D* Light3D() { return (CLight3D*)GetComponent(COMPONENT_TYPE::LIGHT3D); }
    class CParticleSystem* ParticleSystem() { return (CParticleSystem*)GetComponent(COMPONENT_TYPE::PARTICLE_SYSTEM); }
	class CSkyBox* SkyBox() { return (CSkyBox*)GetComponent(COMPONENT_TYPE::SKYBOX); }
	class CDecal* Decal() { return (CDecal*)GetComponent(COMPONENT_TYPE::DECAL); }


public:
    void Begin();
    void Tick();
    virtual void FinalTick();
    void Render();



};

