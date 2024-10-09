#pragma once
#include "AssetUI.h"
class MeshUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    MeshUI();
    ~MeshUI();
};

