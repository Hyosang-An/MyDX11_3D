#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{

public:
    CLayer(int _LayerIdx);
    CLayer(const CLayer& _Origin);
    ~CLayer();
    virtual CLayer* Clone() { return new CLayer(*this); }

private:
    vector<CGameObject*>    m_vecParents; // �ش� ���̾� �Ҽ� ������Ʈ�߿��� �ֻ��� �θ������Ʈ�� ����
    vector<CGameObject*>    m_vecObjects; // �ش� ���̾� �Ҽ� ������Ʈ �߿��� �θ� �ڽ� ������� ��� ������Ʈ
    const int               m_LayerIdx;

public:
    void Begin();
    void Tick();
    void FinalTick();

public:
    int GetLayerIdx() { return m_LayerIdx; }
    void AddObject(CGameObject* _Object, bool _bMoveChild);
    const vector<CGameObject*>& GetParentObjects() { return m_vecParents; }
    const vector<CGameObject*>& GetObjects() { return m_vecObjects; }
    void ClearObject() { m_vecObjects.clear(); }
    void RegisterGameObject(CGameObject* _Object) { m_vecObjects.push_back(_Object); }
    void DisconnectWithObject(CGameObject* _Object);

    void RegisterAsParent(CGameObject* _Object);
    void DeregisterAsParent(CGameObject* _Object);
    void DeregisterObject(CGameObject* _Object);




};

