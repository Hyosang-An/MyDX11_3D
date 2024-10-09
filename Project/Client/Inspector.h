#pragma once
#include "EditorUI.h"

#include <Engine/Ptr.h>
#include <Engine/assets.h>

class Inspector :
    public EditorUI
{
private:
    class CGameObject*      m_TargetObject;
    class ComponentUI*      m_arrComUI[(UINT)COMPONENT_TYPE::END];
    vector<class ScriptUI*> m_vecScriptUI;

    Ptr<CAsset>             m_TargetAsset;
    class AssetUI*          m_arrAssetUI[(UINT)ASSET_TYPE::END];

public:
    void SetTargetObject(CGameObject* _Object);
    void SetTargetAsset(Ptr<CAsset> _Asset);

    CGameObject* GetTargetObject() { return m_TargetObject; }
    Ptr<CAsset> GetTargetAsset() { return m_TargetAsset; }

private:
    void CreateComponentUI();
    void CreateAssetUI();
    void CreateScriptUI(UINT _Count);

public:
    virtual void Init() override;
    virtual void Update() override;

public:
    Inspector();
    ~Inspector();
};

