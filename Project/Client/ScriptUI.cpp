#include "pch.h"
#include "ScriptUI.h"

#include <Scripts/CScriptMgr.h>
#include <Engine/CScript.h>

#include "ParamUI.h"
#include "ListUI.h"

ScriptUI::ScriptUI()
	: ComponentUI(COMPONENT_TYPE::SCRIPT)
	, m_Script(nullptr)
{
}

ScriptUI::~ScriptUI()
{
}



void ScriptUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();

	ImGui::PushID((int)GetComponentUIType());
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));

	wstring strScriptName = CScriptMgr::GetScriptName(m_Script);
	ImGui::Button(string(strScriptName.begin(), strScriptName.end()).c_str());

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	// Script 에서 노출시킬 데이터를 보여준다.
	const vector<tScriptParam>& vecParam = m_Script->GetScriptParam();

	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		switch (vecParam[i].Type)
		{
			case SCRIPT_PARAM_TYPE::INT:
				ParamUI::InputInt((int*)vecParam[i].pData, vecParam[i].Desc);
				break;
			case SCRIPT_PARAM_TYPE::FLOAT:
				ParamUI::InputFloat((float*)vecParam[i].pData, vecParam[i].Desc);
				break;
			case SCRIPT_PARAM_TYPE::VEC2:
				ParamUI::InputVec2((Vec2*)vecParam[i].pData, vecParam[i].Desc);
				break;
			case SCRIPT_PARAM_TYPE::VEC3:

				break;
			case SCRIPT_PARAM_TYPE::VEC4:
				ParamUI::InputVec4((Vec4*)vecParam[i].pData, vecParam[i].Desc);
				break;
			case SCRIPT_PARAM_TYPE::TEXTURE:
			{
				Ptr<CTexture>& pTex = *((Ptr<CTexture>*)vecParam[i].pData);
				ParamUI::InputTexture(pTex, vecParam[i].Desc);
			break;
			}
			case SCRIPT_PARAM_TYPE::PREFAB:
			{
				Ptr<CPrefab>& pPrefab = *((Ptr<CPrefab>*)vecParam[i].pData);

				if (ParamUI::InputPrefab(pPrefab, vecParam[i].Desc, this, (DELEGATE_1)&ScriptUI::SelectPrefab))
				{
					m_SelectedPrefabPtr = &pPrefab;
				}
				break;
			}
		}
	}

	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(content_size);
}


void ScriptUI::SetTargetScript(CScript* _Script)
{
	m_Script = _Script;

	if (nullptr != m_Script)
		SetActive(true);
	else
		SetActive(false);
}

void ScriptUI::SelectPrefab(DWORD_PTR _ListUI)
{
	ListUI* pListUI = (ListUI*)_ListUI;
	string strName = pListUI->GetSelectName();

	if ("None" == strName)
	{
		*m_SelectedPrefabPtr = nullptr;
		return;
	}

	wstring strAssetName = wstring(strName.begin(), strName.end());
	Ptr<CPrefab> pPrefab = CAssetMgr::GetInst()->FindAsset<CPrefab>(strAssetName);

	assert(pPrefab.Get());

	*m_SelectedPrefabPtr = pPrefab;
}