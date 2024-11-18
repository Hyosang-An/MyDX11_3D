#pragma once
#include "CRenderComponent.h"
#include "CHeightMapCS.h"
#include "CRaycastCS.h"
#include "CWeightMapCS.h"

struct tRaycastOut
{
    Vec2    Location;
    UINT    Distance;
    int     Success;
};

enum LANDSCAPE_MODE
{
    NONE,
    HEIGHTMAP,
    SPLATING,
};

struct tWeight8
{
    float arrWeight[8];
};

class CLandScape :
    public CRenderComponent
{
private:
    int        m_FaceX;
    int        m_FaceZ;

    // Tessellation 
    float                   m_MinLevel = 0;
    float                   m_MaxLevel = 4;
    float                   m_MaxLevelRange = 2000.f;
    float                   m_MinLevelRange = 6000.f;

    // Brush
    Vec2                    m_BrushScale = Vec2(0.2f, 0.2f);
    vector<Ptr<CTexture>>   m_vecBrush;
    UINT                    m_BrushIdx = -1;

    // HeightMap
    Ptr<CTexture>           m_HeightMap;
    bool                    m_IsHeightMapCreated = false;
    Ptr<CHeightMapCS>       m_HeightMapCS;

    // Raycasting
    Ptr<CRaycastCS>         m_RaycastCS;
    CStructuredBuffer*      m_RaycastOut = nullptr;
    tRaycastOut             m_Out;

    // WeightMap
    Ptr<CTexture>           m_ColorTex;
    Ptr<CTexture>           m_NormalTex;
    CStructuredBuffer*      m_WeightMap = nullptr;
    UINT                    m_WeightWidth= 0;
    UINT                    m_WeightHeight= 0;
    Ptr<CWeightMapCS>       m_WeightMapCS;
    int                     m_WeightIdx= 0;

    // LandScape ¸ðµå
    LANDSCAPE_MODE          m_Mode = LANDSCAPE_MODE::SPLATING;

public:
    void SetFace(int _X, int _Z);
    void AddBrushTexture(Ptr<CTexture> _BrushTex) { m_vecBrush.push_back(_BrushTex); }
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap;  m_IsHeightMapCreated = false; }
    void CreateHeightMap(UINT _Width, UINT _Height);

public:
    virtual void Init() override;
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

private:
    void CreateMesh();
    void CreateComputeShader();
    void CreateTextureAndStructuredBuffer();
    int Raycasting();

public:
    CLONE(CLandScape);
    CLandScape();
    ~CLandScape();
};

