#pragma once
#include "AssetUI.h"
class PrefabUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    PrefabUI();
    ~PrefabUI();
};

