#pragma once
#include "AssetUI.h"
class SoundUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    SoundUI();
    ~SoundUI();
};

