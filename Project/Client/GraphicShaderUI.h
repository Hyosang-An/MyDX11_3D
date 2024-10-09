#pragma once
#include "AssetUI.h"
class GraphicShaderUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    GraphicShaderUI();
    ~GraphicShaderUI();
};

