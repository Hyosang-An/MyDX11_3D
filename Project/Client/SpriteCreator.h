#pragma once
#include "EditorUI.h"

class SE_AtlasView;
class SE_Detail;

class SpriteCreator :
    public EditorUI
{
public:
    SpriteCreator();
    ~SpriteCreator();

private:
    SE_AtlasView* m_AtlasView = nullptr;
    SE_Detail* m_Detail = nullptr;

public:
    SE_AtlasView* GetAtlasView() { return m_AtlasView; }
    SE_Detail* GetDetail() { return m_Detail; }

public:
    virtual void Init() override;
    virtual void Update() override;

    virtual void Activate() override;
    virtual void Deactivate() override;

};

