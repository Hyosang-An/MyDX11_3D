#pragma once
#include <Engine/CGameObject.h>

class CGameObjectEx :
    public CGameObject
{
public:
    virtual void FinalTick() override;

    virtual CGameObjectEx* Clone() { return new CGameObjectEx(*this); };
};

