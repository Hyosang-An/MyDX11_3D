#pragma once
#include "EditorUI.h"
class Content :
    public EditorUI
{
private:
    class TreeUI* m_Tree;
    vector<wstring> m_vecAssetRelativePath; // Content 폴더에 있는 에셋 상대 경로

private:
    void RenewContent();

    void AssetSelected(DWORD_PTR _Param);
    void PopupMenu(DWORD_PTR _Parma);

    void FindAssetName(const wstring& _FolderPath, const wstring& _Filter);
    void LoadAsset(const path& _Path);

public:
    // Content 폴더에 있는 리소스(에셋) 들을 확인 및 로딩
    void Reload();

public:
    virtual void Init() override;
    virtual void Update() override;

public:
    Content();
    ~Content();
};

