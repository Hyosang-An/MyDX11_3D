#pragma once
#include "EditorUI.h"
class MenuUI :
    public EditorUI
{
private:
    std::wstring m_lastSaveLoadDirectory; // 초기 경로 설정

    UINT m_newObjIdx = 0;

public:
    virtual void Init() override;
    virtual void Tick() override;
    virtual void Update() override;

private:
    void File();
    void Level();
    void GameObject();
    void Assets();

	void LevelPlayPauseStopButton();

private:
    void AddScript();

    void FontsCheck();

    wstring CreateRelativePathAssetKey(ASSET_TYPE _Type, const wstring& _Format);


public:
    MenuUI();
    ~MenuUI();
};

