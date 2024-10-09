#pragma once
#include "AssetUI.h"
class TextureUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    TextureUI();
    ~TextureUI();
};

