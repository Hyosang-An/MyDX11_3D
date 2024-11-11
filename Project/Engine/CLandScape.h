#pragma once
#include "CRenderComponent.h"
class CLandScape :
    public CRenderComponent
{
private:
    UINT        m_FaceX;
    UINT        m_FaceZ;


public:
    void SetFace(UINT _X, UINT _Z);

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

private:
    void CreateMesh();

public:
    CLONE(CLandScape);
    CLandScape();
    ~CLandScape();
};

