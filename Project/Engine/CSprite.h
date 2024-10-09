#pragma once
#include "CAsset.h"
class CSprite :
    public CAsset
{
private:
    Ptr<CTexture>   m_Atlas;
    Vec2            m_LeftTopInAtlasUV;      // UV ÁÂÇ¥°è
    Vec2            m_SliceSizeInAtlasUV;        // UV ÁÂÇ¥°è
    Vec2            m_BackgroundSizeInAtlasUV;
    Vec2            m_OffsetUV;


public:
    Ptr<CTexture> GetAtlasTexture() { return m_Atlas; }

    void SetLeftTop(Vec2 _LeftTop);
    void SetSlice(Vec2 _Slice);
    void SetBackgroundPixelSize(Vec2 _BackgroundPixelSize);
    void SetOffsetPixel(Vec2 _Offset);

    Vec2 GetLeftTopInAtlasUV() { return m_LeftTopInAtlasUV; }
    Vec2 GetSliceSizeInAtlasUV() { return m_SliceSizeInAtlasUV; }
    Vec2 GetBackgroundSizeInAtlasUV() { return m_BackgroundSizeInAtlasUV; }
    Vec2 GetOffsetUV() { return m_OffsetUV; }
    Vec2 GetOffsetPixel();


public:
    void Create(Ptr<CTexture> _Atlas, Vec2 _LeftTopPixel, Vec2 _SlicePixel);

    virtual int Load(const wstring& _FilePath) override;
    virtual int Save(const wstring& _FilePath) override;


public:
    CSprite();
    ~CSprite();
};

