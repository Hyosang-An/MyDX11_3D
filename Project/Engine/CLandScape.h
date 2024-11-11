#pragma once
#include "CRenderComponent.h"
class CLandScape :
    public CRenderComponent
{
private:
    int        m_FaceX;
    int        m_FaceZ;
    Ptr<CTexture>   m_HeightMap;

    float           m_TessLevel = 8.f;

public:
    void SetFace(int _X, int _Z);
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap; }

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

