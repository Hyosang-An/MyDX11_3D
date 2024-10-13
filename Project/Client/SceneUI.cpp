#include "pch.h"
#include "SceneUI.h"

#include "Engine/CRenderMgr.h"
#include "Engine/CAssetMgr.h"
#include "Engine/CDevice.h"

SceneUI::SceneUI()
{
	Vec2 Resolution = CDevice::GetInst()->GetResolution();
	wstring texName = L"RTTex_For_SceneUI";
	CAssetMgr::GetInst()->CreateTexture(texName, (UINT)Resolution.x, (UINT)Resolution.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);
}

SceneUI::~SceneUI()
{
}

void SceneUI::Update()
{
	Ptr<CTexture> pSceneTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RTTex_For_SceneUI");

	ImGui::Image(pSceneTex->GetSRV().Get(), ImVec2(100, 100));
}
