#pragma once
#include "SC_Sub.h"


class SE_AtlasView :
    public SE_Sub
{
public:
    SE_AtlasView();
    ~SE_AtlasView();

private:
    Ptr<CTexture>   m_AtlasTex;

    ImVec2          m_imageRectMin; // Image 위젯 좌상단 실제 위치

    ImVec2          m_MousePos; // 실제 마우스 위치

    ImVec2          m_stripeBoxLTonOriginalTex;  // 마우스 위치에 해당하는 Texture 의 픽셀 좌표
    ImVec2          m_stripeBoxRBonOriginalTex; // 마우스 위치에 해당하는 Texture 의 픽셀 좌표

    float           m_WheelScale = 1;
    float           m_Ratio = 1;        // 실제 해상도 대비 출력 이미지의 비율



public:
    void SetAtlasTex(Ptr<CTexture> _Tex);
    std::pair<Vec2, Vec2> GetSpriteBoxPosOnOriginalTex();



private:
    void WheelCheck();
    void SelectSpriteArea();
    void DrawSelectRect();
    void CalcSpriteSize(Vec2 _PixelPos);

public:
    virtual void Init() override;
    virtual void Update() override;

};

