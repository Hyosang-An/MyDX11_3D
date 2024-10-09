#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CMeshRender.h>
#include <Engine/CMesh.h>
#include <Engine/CMaterial.h>

#include "CEditorMgr.h"
#include "ListUI.h"
#include "TreeUI.h"

MeshRenderUI::MeshRenderUI()
	: ComponentUI(COMPONENT_TYPE::MESHRENDER)
{
}

MeshRenderUI::~MeshRenderUI()
{
}

void MeshRenderUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();


	Title();

	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();

	// 메쉬 정보
	Ptr<CMesh> pMesh = pMeshRender->GetMesh();

	string MeshName;

	if (pMesh.Get())
		MeshName = string(pMesh->GetKey().begin(), pMesh->GetKey().end());

	ImGui::Text("Mesh");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##MeshKey", (char*)MeshName.c_str(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		// 데이터를 드랍 받을 트리 이름 : ContentTree
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
		if (payload)
		{
			// payload의 Data는 TreeNode*를 가리키는 포인터
			TreeNode** ppNode = (TreeNode**)payload->Data;
			TreeNode* pNode = *ppNode;

			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset != nullptr && ASSET_TYPE::MESH == pAsset->GetAssetType())
			{
				pMeshRender->SetMesh((CMesh*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}
	
	ImGui::SameLine();
	if (ImGui::Button("##MeshBtn", ImVec2(18.f, 18.f)))
	{
		ListUI* pListUI = (ListUI*)CEditorMgr::GetInst()->FindEditorUI("List");
		pListUI->SetName("Mesh");
		vector<string> vecMeshNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::MESH, vecMeshNames);
		pListUI->AddList(vecMeshNames);
		pListUI->AddDelegate(this, (DELEGATE_1)&MeshRenderUI::SelectMesh);
		pListUI->SetActive(true);
	}


	// 머티리얼 정보
	Ptr<CMaterial> pMtrl = pMeshRender->GetMaterial();

	string MtrlName;
	if (pMtrl.Get())
		MtrlName = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());

	ImGui::Text("Material");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##MaterialKey", (char*)MtrlName.c_str(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
		if (payload)
		{
			TreeNode** ppNode = (TreeNode**)payload->Data;
			TreeNode* pNode = *ppNode;

			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset != nullptr && ASSET_TYPE::MATERIAL == pAsset->GetAssetType())
			{
				pMeshRender->SetMaterial((CMaterial*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##MtrlBtn", ImVec2(18.f, 18.f)))
	{
		ListUI* pListUI = (ListUI*)CEditorMgr::GetInst()->FindEditorUI("List");
		pListUI->SetName("Material");
		vector<string> vecMtrlNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::MATERIAL, vecMtrlNames);
		pListUI->AddList(vecMtrlNames);
		pListUI->AddDelegate(this, (DELEGATE_1)&MeshRenderUI::SelectMaterial);
		pListUI->SetActive(true);
	}


	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(content_size);
}

void MeshRenderUI::SelectMesh(DWORD_PTR _ListUI)
{
	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();

	ListUI* pListUI = (ListUI*)_ListUI;
	string strName = pListUI->GetSelectName();

	if (strName == "None")
	{
		pMeshRender->SetMesh(nullptr);
		return;
	}

	wstring strAssetName = wstring(strName.begin(), strName.end());

	Ptr<CMesh> pMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(strAssetName);

	assert(pMesh.Get());

	
	pMeshRender->SetMesh(pMesh);
}

void MeshRenderUI::SelectMaterial(DWORD_PTR _ListUI)
{
	CMeshRender* pMeshRender = GetTargetObject()->MeshRender();

	ListUI* pListUI = (ListUI*)_ListUI;
	string strName = pListUI->GetSelectName();

	if ("None" == strName)
	{
		pMeshRender->SetMaterial(nullptr);
		return;
	}

	wstring strAssetName = wstring(strName.begin(), strName.end());

	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(strAssetName);

	assert(pMtrl.Get());

	pMeshRender->SetMaterial(pMtrl);
}