#pragma once
#include "ComponentUI.h"
class MeshRenderUI :
    public ComponentUI
{
private:
    virtual void Update() override;

public:
    void SelectMesh(DWORD_PTR _AssetName);
    void SelectMaterial(DWORD_PTR _AssetName);

public:
    MeshRenderUI();
    ~MeshRenderUI();
};

