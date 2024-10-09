#include "pch.h"
#include "TextureUI.h"

TextureUI::TextureUI()
	: AssetUI(ASSET_TYPE::TEXTURE)
{
}

TextureUI::~TextureUI()
{
}

void TextureUI::Update()
{
	Title();

	Ptr<CTexture> pTexture = (CTexture*)GetAsset().Get();

	// 이미지	
	ImVec2 uv_min = ImVec2(0.0f, 0.0f);
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);

	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	ImGui::Image(pTexture->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);

	// 텍스쳐 이름
	OutputAssetName();

	// 해상도
	UINT width = pTexture->Width();
	UINT height = pTexture->Height();

	char buff[50] = {};

	sprintf_s(buff, "%d", width);
	ImGui::Text("Width");
	ImGui::SameLine(100);
	ImGui::InputText("##TextureWidth", buff, 50, ImGuiInputTextFlags_ReadOnly);

	sprintf_s(buff, "%d", height);
	ImGui::Text("Height");
	ImGui::SameLine(100);
	ImGui::InputText("##TextureHeight", buff, 50, ImGuiInputTextFlags_ReadOnly);


	// 배열 사이즈
}