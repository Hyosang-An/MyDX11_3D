#pragma once
#include "AssetUI.h"
class MeshDataUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    MeshDataUI();
    ~MeshDataUI();
};

