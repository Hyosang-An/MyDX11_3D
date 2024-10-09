#pragma once
#include "AssetUI.h"
class SpriteUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    SpriteUI();
    ~SpriteUI();
};

