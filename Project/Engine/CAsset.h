#pragma once
#include "CEntity.h"
class CAsset :
    public CEntity
{
    template<typename T>
    friend class Ptr; // 스마트 포인터가 내부 모든 변수 및 메서드에 접근 가능하도록
    friend class CAssetMgr;

public:
    CAsset(ASSET_TYPE _type);
    CAsset(const CAsset& _other);
    ~CAsset();
    virtual CAsset* Clone() { return nullptr; }

private:
    wstring             m_Key;
    wstring             m_RelativePath;
    const ASSET_TYPE    m_Type;
    int                 m_RefCount;

    bool                m_Engine = false;

public:
    const wstring& GetKey() { return m_Key; }
    const wstring& GetRelativePath() { return m_RelativePath; }
    ASSET_TYPE GetAssetType() { return m_Type; }
    UINT GetRefCount() { return m_RefCount; }
    bool IsEngineAsset() { return m_Engine; }

protected:
    void SetKey(const wstring& _Key) { m_Key = _Key; }
    void SetRelativePath(const wstring& _path) { m_RelativePath = _path; }
    void SetEngineAsset() { m_Engine = true; }

private:
    void AddRef() { ++m_RefCount; }
    void Release()
    {
        --m_RefCount;
        if (m_RefCount <= 0)
        {
            delete this;
        }
    }

    virtual int Load(const wstring& _FilePath) = 0;
    virtual int Save(const wstring& _FilePath) = 0;
};

