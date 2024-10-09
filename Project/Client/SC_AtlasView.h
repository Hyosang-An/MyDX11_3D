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

    ImVec2          m_imageRectMin; // Image ���� �»�� ���� ��ġ

    ImVec2          m_MousePos; // ���� ���콺 ��ġ

    ImVec2          m_stripeBoxLTonOriginalTex;  // ���콺 ��ġ�� �ش��ϴ� Texture �� �ȼ� ��ǥ
    ImVec2          m_stripeBoxRBonOriginalTex; // ���콺 ��ġ�� �ش��ϴ� Texture �� �ȼ� ��ǥ

    float           m_WheelScale = 1;
    float           m_Ratio = 1;        // ���� �ػ� ��� ��� �̹����� ����



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

