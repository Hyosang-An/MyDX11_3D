#pragma once
#include "ComponentUI.h"
class ScriptUI :
    public ComponentUI
{
private:
    class CScript* m_Script;
    Ptr<CPrefab>* m_SelectedPrefabPtr = nullptr;

public:
    void SetTargetScript(CScript* _Script);
    CScript* GetTargetScript() { return m_Script; }

private:
    void SelectPrefab(DWORD_PTR _ListUI);

public:
    virtual void Update() override;

public:
    ScriptUI();
    ~ScriptUI();
};

