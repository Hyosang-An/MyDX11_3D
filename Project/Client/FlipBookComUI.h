#pragma once
#include "ComponentUI.h"
class FlipBookComUI :
    public ComponentUI
{
private:
    wstring m_lastFlipBookDirectory;

private:
    virtual void Update() override;

    void AddFlipBookByDialog();

public:
    FlipBookComUI();
    ~FlipBookComUI();
};

