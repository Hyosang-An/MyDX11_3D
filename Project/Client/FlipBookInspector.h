#pragma once
#include "FlipBookEditor.h"


class FlipBookInspector :
    public FlipBookEditor
{
    friend class FlipBookEditor;
public:
    FlipBookInspector();
    ~FlipBookInspector();

private:
    FlipBookEditor*         m_owner = nullptr;
    FlipBookViewer*         m_viewer = nullptr; 

    Ptr<CFlipBook>          m_selectedFlipBook;

    bool isSpriteListFocused;

    bool m_isSpriteChanged = false;
    float m_accTimeSinceSpriteChanged = 0.0f;

    wstring                 m_lastFlipBookDirectory;
    wstring                 m_selectedSpriteDirectory;
    vector<Ptr<CSprite>>    m_vecSpriteInFolder;
    int                     m_selectedSpriteIndex = -1;

private:
    void    SelectFlipBook();
    void    FlipBookSpriteList();
    void    SetSpriteOffsetAndBackgroundSize();
    void    SetSpriteChanged();
    void    SaveFlipBook(Ptr<CFlipBook> FlipBookToSave);

    void    SelectSpreteFolderAndShowSprites();



    HRESULT SelectSpriteFolderPathByDialog();
    void    SelectFlipBookByDialog();
    void    GetSpritesFromSelectedFolder(wstring _folderPath);

public:
    int     GetSelectedSpriteIndex() { return m_selectedSpriteIndex; }

public:
    virtual void Init() override;
    virtual void Update() override;
};

