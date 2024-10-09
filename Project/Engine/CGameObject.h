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

    int                     m_LayerIdx = -1; // 소속 레이어의 인덱스 번호

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

    // Unregister : 등록을 취소하다, 등록하는 행위를 취소하다
    // Deregister : 등록된것을 취소하다
    void DeregisterThisChildFromParent();

    class CTransform* Transform() { return (CTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM); }
    class CMeshRender* MeshRender() { return (CMeshRender*)GetComponent(COMPONENT_TYPE::MESHRENDER); }
    class CCamera* Camera() { return (CCamera*)GetComponent(COMPONENT_TYPE::CAMERA); }
    class CCollider2D* Collider2D() { return (CCollider2D*)GetComponent(COMPONENT_TYPE::COLLIDER2D); }
    class CFlipBookComponent* FlipBookComponent() { return (CFlipBookComponent*)GetComponent(COMPONENT_TYPE::FLIPBOOK_COMPONENT); }
    class CTileMap* TileMap() { return (CTileMap*)GetComponent(COMPONENT_TYPE::TILEMAP); }
    class CLight2D* Light2D() { return (CLight2D*)GetComponent(COMPONENT_TYPE::LIGHT2D); }
    class CParticleSystem* ParticleSystem() { return (CParticleSystem*)GetComponent(COMPONENT_TYPE::PARTICLE_SYSTEM); }


public:
    void Begin();
    void Tick();
    virtual void FinalTick();
    void Render();



};

