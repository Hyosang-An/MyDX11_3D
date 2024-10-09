#pragma once
#include "CComponent.h"


#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CAssetMgr.h"
#include "components.h"

enum class SCRIPT_PARAM_TYPE
{
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    TEXTURE,
    PREFAB,
};

struct tScriptParam
{
    SCRIPT_PARAM_TYPE Type;
    void* pData;
    string       Desc;
    DWORD_PTR    Param_0;
    DWORD_PTR    Param_1;
};

class CScript :
    public CComponent
{
private:
    UINT                    m_ScriptType;
    vector<tScriptParam>    m_vecScriptParam;

public:
    CRenderComponent* GetRenderComponent() { return GetOwner()->GetRenderComponent(); }
    UINT GetScriptType() { return m_ScriptType; }
    const vector<tScriptParam>& GetScriptParam() { return   m_vecScriptParam; }

protected:
    void AddScriptParam(SCRIPT_PARAM_TYPE _Type, const string& _Desc, void* _pData, DWORD_PTR _Param0 = 0, DWORD_PTR _Param1 = 0)
    {
        m_vecScriptParam.push_back(tScriptParam{ _Type,_pData, _Desc, _Param0, _Param1 });
    }

    void Instantiate(Ptr<CPrefab> _Pref, int _LayerIdx, Vec3 _WorldPos, const wstring& _Name = L"");
public:
    //virtual void Begin() override {}
    virtual void Tick() = 0;
    virtual void FinalTick() final override {}

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}
    virtual void Overlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}
    virtual void EndOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider) {}

    virtual void SaveToFile(FILE* _File) = 0;
    virtual void LoadFromFile(FILE* _File) = 0;

public:
    CScript(UINT _Type);
    ~CScript();
};