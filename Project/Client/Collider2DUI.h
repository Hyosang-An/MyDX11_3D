#pragma once

#include "ComponentUI.h"

class Collider2DUI :
	public ComponentUI
{
public:
    virtual void Update() override;

public:
    Collider2DUI();
    ~Collider2DUI();
};

