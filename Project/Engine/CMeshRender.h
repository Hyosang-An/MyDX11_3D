#pragma once

#include "CRenderComponent.h"

class CMeshRender :
    public CRenderComponent
{

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual CMeshRender* Clone() { return new CMeshRender(*this); }

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

public:
    CMeshRender();
    ~CMeshRender();
};