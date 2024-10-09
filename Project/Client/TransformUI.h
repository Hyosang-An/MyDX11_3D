#pragma once
#include "ComponentUI.h"

class TransformUI :
    public ComponentUI
{
public:
    virtual void Update() override;

public:
    TransformUI();
    ~TransformUI();
};

