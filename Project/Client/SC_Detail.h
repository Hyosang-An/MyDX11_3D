#pragma once
#include "SC_Sub.h"

enum class SpriteSlectMode
{
    ClickAndDrag,        // Ŭ�� �巡�� ���
    AutoSelectOnClick,   // Ŭ�� �� �ڵ� ����
    ManualSpecification  // ���� �� ����
};

class SE_Detail :
    public SE_Sub
{
public:
    SE_Detail();
    ~SE_Detail();

    Ptr<CTexture>   m_AtlasTex;
    SpriteSlectMode m_SelectMode = SpriteSlectMode::AutoSelectOnClick;

    Vec2            m_SpriteLT;
    Vec2            m_SpriteSize;
    Vec2            m_BackgroundSize = { 200, 200 };

    Vec2            m_StartLT; // ManualSpecification ��忡�� ���� Left Top
    int             m_Count = 1; // ManualSpecification ��忡�� Count
    wstring         m_SpriteName; // ManualSpecification ��忡�� SpriteName

    wstring         m_lastFileName;
    wstring         m_lastSaveDirectory;

private:
    void Atlas();
    void AtlasInfo();
    void SpriteInfo();

    HRESULT SelectSpriteSaveFolderByDialog();

public:
    void SetAtlasTex(Ptr<CTexture> _Tex);

    void SetSpriteSelectMode(SpriteSlectMode _Mode) { m_SelectMode = _Mode; }
    SpriteSlectMode GetSpriteSelectMode() { return m_SelectMode; }

    std::pair<Vec2, Vec2> GetStartLTandSize() { return { m_StartLT, m_SpriteSize }; }
    int GetCount() { return m_Count; }

private:
    void SelectTexture(DWORD_PTR _ListUI);
    
    void SaveSprite();
    void SaveAllSprites(); // ManualSpecification ��忡�� ���


public:
    virtual void Init() override;
    virtual void Update() override;

};

