#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"
#include "CPathMgr.h"

OBJECT_SAVE  CPrefab::g_ObjectSaveFunc = nullptr;
OBJECT_LOAD  CPrefab::g_ObjectLoadFunc = nullptr;

CPrefab::CPrefab()
    : CAsset(ASSET_TYPE::PREFAB)
{
}

CPrefab::~CPrefab()
{
    if (nullptr != m_ProtoObject)
        delete m_ProtoObject;
}

CGameObject* CPrefab::Instantiate()
{
    return m_ProtoObject->Clone();
}

int CPrefab::Save(const wstring& _FilePath)
{
    // 어디에 저장해뒀는지 알고 있는게 좋음
    wstring relativePath = CPathMgr::GetInst()->GetRelativePath(_FilePath);
    SetRelativePath(relativePath);

    std::filesystem::path dir_path = _FilePath;
    if (!std::filesystem::exists(dir_path.parent_path()))
        std::filesystem::create_directories(dir_path.parent_path()); // 중간 디렉토리 재귀적으로 생성

    FILE* File = nullptr;
    _wfopen_s(&File, _FilePath.c_str(), L"wb");

    if (nullptr == File)
    {
        return E_FAIL;
    }

    g_ObjectSaveFunc(File, m_ProtoObject);

    fclose(File);

    return S_OK;
}

int CPrefab::Load(const wstring& _FilePath)
{
    FILE* File = nullptr;
    _wfopen_s(&File, _FilePath.c_str(), L"rb");

    if (nullptr == File)
    {
        wstring msg = _FilePath + L" 경로를 열 수 없습니다.";
        MessageBox(nullptr, msg.c_str(), L"Load에러", MB_OK);
        return E_FAIL;
    }

    m_ProtoObject = g_ObjectLoadFunc(File);

    fclose(File);

    return S_OK;
}