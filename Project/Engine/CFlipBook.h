#pragma once
#include "CAsset.h"
class CFlipBook :
    public CAsset
{
private:
    vector<Ptr<CSprite>>    m_vecSprite;

public:
    void AddSprite(Ptr<CSprite> _Sprite) { m_vecSprite.push_back(_Sprite); }
    void RemoveSprite(int _Idx) { m_vecSprite.erase(m_vecSprite.begin() + _Idx); }

    vector<Ptr<CSprite>>& GetSpriteVec() { return m_vecSprite; }
    Ptr<CSprite> GetSprite(int _Idx) { return m_vecSprite[_Idx]; }
    int GetMaxFrameCount() { return (int)m_vecSprite.size(); }
    void SetAllSpritesBackgroundPixelSize(Vec2 _Size);


public:
    void FinalTick();

    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override;

public:
    CFlipBook();
    ~CFlipBook();
};

