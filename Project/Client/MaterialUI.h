#pragma once
#include "AssetUI.h"
class MaterialUI :
    public AssetUI
{
private:
    TEX_PARAM   m_SelectTexParam;

public:
    virtual void Update() override;

private:
    void ShaderParameter();
    void SelectShader(DWORD_PTR _ListUI);

    void ChangeTexture(DWORD_PTR Param);

public:
    MaterialUI();
    ~MaterialUI();
};

