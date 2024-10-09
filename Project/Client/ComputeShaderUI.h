#pragma once
#include "AssetUI.h"
class ComputeShaderUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    ComputeShaderUI();
    ~ComputeShaderUI();
};

