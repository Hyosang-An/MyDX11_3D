#include "pch.h"
#include "SC_Detail.h"

#include "CEditorMgr.h"

#include "ListUI.h"
#include "TreeUI.h"

#include "SC_AtlasView.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CSprite.h>
#include <Engine/func.h>

SE_Detail::SE_Detail()
{
}

SE_Detail::~SE_Detail()
{
}


void SE_Detail::Init()
{

}

void SE_Detail::Update()
{
	Atlas();

	AtlasInfo();

	SpriteInfo();
}

void SE_Detail::Atlas()
{
	string TexName;

	if (nullptr != m_AtlasTex)
		TexName = string(m_AtlasTex->GetKey().begin(), m_AtlasTex->GetKey().end());

	ImGui::Text("Atlas Texture");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##AtlasTex", (char*)TexName.c_str(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
		if (payload)
		{
			TreeNode** ppNode = (TreeNode**)payload->Data;
			TreeNode* pNode = *ppNode;

			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset != nullptr && ASSET_TYPE::TEXTURE == pAsset->GetAssetType())
			{
				SetAtlasTex((CTexture*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##AtlasTexBtn", ImVec2(18.f, 18.f)))
	{
		ListUI* pListUI = (ListUI*)CEditorMgr::GetInst()->FindEditorUI("List");
		pListUI->SetName("Texture");
		vector<string> vecTexNames;
		CAssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::TEXTURE, vecTexNames);
		pListUI->AddList(vecTexNames);
		pListUI->AddDelegate(this, (DELEGATE_1)&SE_Detail::SelectTexture);
		pListUI->SetActive(true);
	}
}

void SE_Detail::AtlasInfo()
{
	// �ػ�
	UINT width = 0;
	UINT height = 0;

	if (nullptr != m_AtlasTex)
	{
		width = m_AtlasTex->Width();
		height = m_AtlasTex->Height();
	}

	char buff[50] = {};
	sprintf_s(buff, "%d", width);

	ImGui::Text("Width");
	ImGui::SameLine(100);
	ImGui::InputText("##TextureWidth", buff, 50, ImGuiInputTextFlags_ReadOnly);

	sprintf_s(buff, "%d", height);
	ImGui::Text("Height");
	ImGui::SameLine(100);
	ImGui::InputText("##TextureHeight", (char*)std::to_string(height).c_str(), 50, ImGuiInputTextFlags_ReadOnly);
}

void SE_Detail::SpriteInfo()
{
	ImGui::Separator();

	// Sprite SelectMod Combo Box
	ImGui::Text("Sprite Select Mode");
	ImGui::SameLine(140);
	ImGui::SetNextItemWidth(180.f);
	const char* SpriteSelectModes[] = { "Click And Drag", "Auto Select On Click", "Manual Specification"};
	if (ImGui::BeginCombo("##SpriteSelectMode", SpriteSelectModes[(int)m_SelectMode]))
	{
		if (ImGui::Selectable("ClickAndDrag"))
			m_SelectMode = SpriteSlectMode::ClickAndDrag; 
		if (ImGui::Selectable("AutoSelectOnClick"))
			m_SelectMode = SpriteSlectMode::AutoSelectOnClick;
		if (ImGui::Selectable("ManualSpecification"))
			m_SelectMode = SpriteSlectMode::ManualSpecification;
		ImGui::EndCombo();
	}

	if (m_SelectMode == SpriteSlectMode::ClickAndDrag || m_SelectMode == SpriteSlectMode::AutoSelectOnClick)
	{
		auto spriteBoxPos = GetAtlasView()->GetSpriteBoxPosOnOriginalTex();
		if (spriteBoxPos.first == spriteBoxPos.second)
		{
			spriteBoxPos.first = Vec2();
			spriteBoxPos.second = Vec2();
		}

		m_SpriteLT = spriteBoxPos.first;
		m_SpriteSize = spriteBoxPos.second - spriteBoxPos.first;

		ImGui::PushID("Sprite Info");
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));

		ImGui::Button("Sprite Info");

		ImGui::PopStyleColor(3);
		ImGui::PopID();



		ImGui::Text("Left Top");
		ImGui::SameLine(100);
		ImGui::InputFloat2("##Left Top", m_SpriteLT, "%.0f", ImGuiInputTextFlags_ReadOnly);

		ImGui::Text("Right Bottom");
		ImGui::SameLine(100);
		ImGui::InputFloat2("##Right Bottom", spriteBoxPos.second, "%.0f", ImGuiInputTextFlags_ReadOnly);

		ImGui::Text("Size");
		ImGui::SameLine(100);
		ImGui::InputFloat2("##Right Bottom", m_SpriteSize, "%.0f", ImGuiInputTextFlags_ReadOnly);

		ImGui::NewLine();

		ImGui::Text("Set Background Size");
		ImGui::InputFloat2("##Set Background Size", m_BackgroundSize, "%.0f");

		// "Save Sprite" ��ư
		ImGui::BeginDisabled(m_SpriteSize.x * m_SpriteSize.y == 0 || m_AtlasTex == nullptr);
		if (ImGui::Button("Save Sprite"))
			SaveSprite();
		ImGui::EndDisabled();
	}

	else if (m_SelectMode == SpriteSlectMode::ManualSpecification)
	{
		ImGui::PushID("Set Specification");
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));

		ImGui::Button("Set Specification");

		ImGui::PopStyleColor(3);
		ImGui::PopID();

		// ���� Left Top
		int startLT[] = { (int)m_StartLT.x, (int)m_StartLT.y };
		ImGui::Text("Start Left Top");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(150.f);
		ImGui::InputInt2("##StartLeftTop", startLT);
		m_StartLT.x = (float)startLT[0];
		m_StartLT.y = (float)startLT[1];
		m_StartLT.x = max(0, m_StartLT.x);
		m_StartLT.y = max(0, m_StartLT.y);

		// size
		int size[] = { (int)m_SpriteSize.x, (int)m_SpriteSize.y };
		ImGui::Text("Size");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(150.f);
		ImGui::InputInt2("##Size", size);
		m_SpriteSize.x = (float)size[0];
		m_SpriteSize.y = (float)size[1];
		m_SpriteSize.x = max(0, m_SpriteSize.x);
		m_SpriteSize.y = max(0, m_SpriteSize.y);

		// Count
		ImGui::Text("Count");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(150.f);
		ImGui::InputInt("##Count", &m_Count);
		m_Count = max(1, m_Count);

		// Background Size
		int backgroundSize[] = { (int)m_BackgroundSize.x, (int)m_BackgroundSize.y };
		ImGui::Text("Set Background Size");
		ImGui::InputInt2("##Set Background Size", backgroundSize);
		m_BackgroundSize.x = (float)backgroundSize[0];
		m_BackgroundSize.y = (float)backgroundSize[1];
		m_BackgroundSize.x = max(0, m_BackgroundSize.x);
		m_BackgroundSize.y = max(0, m_BackgroundSize.y);

		// ���� ��� ����
		if (ImGui::Button("Select Sprite Save Folder"))
		{
			if (SUCCEEDED(SelectSpriteSaveFolderByDialog()))
			{
				m_SpriteName = path(m_lastSaveDirectory).stem().wstring();
			}
		}
		ImGui::Text("Save Directory");
		ImGui::SameLine(120);
		wstring WsaveRelativeDirectory = CPathMgr::GetInst()->GetRelativePath(m_lastSaveDirectory);
		string saveRelativeDirectory = string(WsaveRelativeDirectory.begin(), WsaveRelativeDirectory.end());
		ImGui::InputText("##SaveDirectory", (char*)saveRelativeDirectory.c_str(), saveRelativeDirectory.size(), ImGuiInputTextFlags_ReadOnly);


		// ���� �̸� ����
		ImGui::Text("File Name");
		ImGui::SameLine(120);
		string spriteName = string(m_SpriteName.begin(), m_SpriteName.end());
		char buff[255] = {};
		sprintf_s(buff, "%s", spriteName.c_str());
		ImGui::InputText("##FileName", buff, 255);
		spriteName = buff;
		m_SpriteName = wstring(spriteName.begin(), spriteName.end());


		// "Save All Sprites" ��ư
		ImGui::BeginDisabled(m_SpriteSize.x * m_SpriteSize.y == 0 || m_AtlasTex == nullptr);
		if (ImGui::Button("Save All Sprites"))
			SaveAllSprites();
		ImGui::EndDisabled();
	}


}

void SE_Detail::SetAtlasTex(Ptr<CTexture> _Tex)
{
	m_AtlasTex = _Tex;
	GetAtlasView()->SetAtlasTex(m_AtlasTex);
}

void SE_Detail::SelectTexture(DWORD_PTR _ListUI)
{
	ListUI* pListUI = (ListUI*)_ListUI;
	string strName = pListUI->GetSelectName();

	if (strName == "None")
	{
		m_AtlasTex = nullptr;
		return;
	}

	wstring strAssetName = wstring(strName.begin(), strName.end());

	Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(strAssetName);

	assert(pTex.Get());

	SetAtlasTex(pTex);
}


void SE_Detail::SaveSprite()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileSaveDialog* pFileSave;

		// CLSID_FileSaveDialog Ŭ������ �ν��Ͻ��� �����ϰ� IFileSaveDialog �������̽� �����͸� �����ɴϴ�.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr)) {
			// ���� ���� ���͸� �����մϴ�.
			COMDLG_FILTERSPEC rgSpec[] = {
				{ L"Sprite Files", L"*.sprite" },
				{ L"All Files", L"*.*" }
			};
			pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec); // ���͸� ��ȭ���ڿ� �����մϴ�.
			pFileSave->SetFileTypeIndex(1); // �⺻ ���� ������ �����մϴ�. rgSpec�� � ���� �⺻������ ����. Index�� 1����.
			pFileSave->SetDefaultExtension(L"sprite"); // �⺻ Ȯ���ڸ� �����մϴ�.
			pFileSave->SetTitle(L"Save sprite File"); // ��ȭ������ ������ �����մϴ�.

			// ���⼭ �⺻ ���� �̸��� �����մϴ�.
			wstring possibleNextFileName;
			if (m_lastFileName != L"")
			{
				possibleNextFileName = incrementTrailingNumber(m_lastFileName);
			}
			pFileSave->SetFileName(possibleNextFileName.c_str()); // �⺻ ���� �̸� ����

			// ������ ��θ� �ʱ� ������ ����
			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_lastSaveDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentPath() + L"animation";
			else
				defaultDirectory = m_lastSaveDirectory;
			hr = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFileSave->SetFolder(psiFolder);
				psiFolder->Release();
			}

			// ���� ���� ��ȭ���ڸ� ǥ���մϴ�.
			hr = pFileSave->Show(NULL);

			if (SUCCEEDED(hr)) 
			{
				IShellItem* pItem;

				// ����ڰ� ������ ������ IShellItem�� �����ɴϴ�.
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr)) 
				{
					PWSTR pszFilePath;

					// IShellItem���� ���� ��θ� �����ɴϴ�.
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr)) 
					{
						// ���⿡�� pszFilePath ��θ� ����Ͽ� ������ �����մϴ�.

						Ptr<CSprite> pSprite = new CSprite;
						pSprite->Create(m_AtlasTex, m_SpriteLT, m_SpriteSize);
						pSprite->SetBackgroundPixelSize(m_BackgroundSize);

						pSprite->Save(pszFilePath);
						

						// ������ ���ϸ� �� ���丮 ������Ʈ
						path filePath = pszFilePath;
						m_lastFileName = filePath.stem().wstring();
						m_lastSaveDirectory = filePath.parent_path().wstring();
						// ���� ��� ��� �� �޸𸮸� �����մϴ�.
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release(); // IShellItem �����͸� �����մϴ�.
				}
			}
			pFileSave->Release(); // IFileSaveDialog �����͸� �����մϴ�.
		}
		CoUninitialize(); // COM ���̺귯���� �����մϴ�.
	}

}

void SE_Detail::SaveAllSprites()
{
	for (int i = 0; i < m_Count; i++)
	{
		wstring fileName = m_SpriteName + L"_" + std::to_wstring(i) + L".sprite";

		Ptr<CSprite> pSprite = new CSprite;
		pSprite->Create(m_AtlasTex, m_StartLT + Vec2(m_SpriteSize.x * i, 0.f), m_SpriteSize);
		pSprite->SetBackgroundPixelSize(m_BackgroundSize);

		wstring savePath = path(m_lastSaveDirectory) / path(fileName);
		pSprite->Save(savePath);
	}
}

HRESULT SE_Detail::SelectSpriteSaveFolderByDialog()
{
	// COM ���̺귯�� �ʱ�ȭ
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		// IFileOpenDialog �������̽� ������ ����
		IFileOpenDialog* pFolderOpen = nullptr;

		// IFileOpenDialog �ν��Ͻ� ����
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFolderOpen));
		if (SUCCEEDED(hr)) {
			// ���� ���� �ɼ� ����
			DWORD dwOptions;
			pFolderOpen->GetOptions(&dwOptions);
			pFolderOpen->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM); // ���� ���� �ɼ� �߰�

			// ��ȭ���� Ÿ��Ʋ ����
			pFolderOpen->SetTitle(L"Select Folder");

			// ������ ��θ� �ʱ� ������ ����
			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_lastSaveDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentPath() + L"animation";
			else
				defaultDirectory = m_lastSaveDirectory;
			hr = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFolderOpen->SetFolder(psiFolder);
				psiFolder->Release();
			}

			// ��ȭ���� ǥ��
			hr = pFolderOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				// ����ڰ� ������ ������ ��Ÿ���� IShellItem ������
				IShellItem* pItem = nullptr;
				hr = pFolderOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					// ������ ������ ��� ���
					PWSTR pszFolderPath = nullptr;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
					if (SUCCEEDED(hr))
					{

						m_lastSaveDirectory = pszFolderPath;

						// ��� ��� �� �޸� ����
						CoTaskMemFree(pszFolderPath);
					}
					pItem->Release();
				}
			}

			pFolderOpen->Release();
		}
		CoUninitialize();
	}

	return hr;
}