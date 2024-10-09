#pragma once
#include "EditorUI.h"

#include "SpriteCreator.h"
#include <Engine/CAssetMgr.h>

class SE_Sub :
    public EditorUI
{
public:
    SE_Sub();
    ~SE_Sub();

private:
    SpriteCreator* m_Owner;

public:
    SpriteCreator* GetOwner() { return m_Owner; }
    class SE_Detail* GetDetail() { return m_Owner->GetDetail(); }
    class SE_AtlasView* GetAtlasView() { return m_Owner->GetAtlasView(); }

    friend class SpriteCreator;
};

