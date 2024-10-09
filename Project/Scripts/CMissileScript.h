#pragma once
#include "Engine/CScript.h"
class CMissileScript :
    public CScript
{
private:
    float   m_Speed;


public:
    virtual void Begin() override;
    virtual void Tick() override;

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) override;

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

public:
    virtual CMissileScript* Clone() { return new CMissileScript(*this); }
    CMissileScript();
    ~CMissileScript();
};

