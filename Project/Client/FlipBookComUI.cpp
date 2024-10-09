#include "pch.h"
#include "FlipBookComUI.h"

#include "TreeUI.h"

#include "Engine/CPathMgr.h"
#include "Engine/CAssetMgr.h"
#include "Engine/CFlipBookComponent.h"

FlipBookComUI::FlipBookComUI()
	: ComponentUI(COMPONENT_TYPE::FLIPBOOK_COMPONENT)
{
}

FlipBookComUI::~FlipBookComUI()
{
}


void FlipBookComUI::Update()
{
    ImVec2 initial_content_pos = ImGui::GetCursorPos();

	Title();

	CFlipBookComponent* pFlipBookComponent = GetTargetObject()->FlipBookComponent();

	if (pFlipBookComponent == nullptr)
		return;

    // Add FlipBook Button
    if (ImGui::Button("Add FlipBook"))
	{
		AddFlipBookByDialog();
	}

    vector<wstring> flipBookNames = pFlipBookComponent->GetFlipBookNames();

    static int item_current_idx = 0; // Here we store our selection data as an index.

    // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
    string curFlipBookStringName = string(flipBookNames[item_current_idx].begin(), flipBookNames[item_current_idx].end());

    ImGui::Text("FlipBook");
    ImGui::SameLine(100);
    if (ImGui::BeginCombo("##Selected FlipBook", curFlipBookStringName.c_str(), ImGuiComboFlags_None)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < flipBookNames.size(); n++)
        {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(string(flipBookNames[n].begin(), flipBookNames[n].end()).c_str(), is_selected))
                item_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
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
			if (pAsset != nullptr && ASSET_TYPE::FLIPBOOK == pAsset->GetAssetType())
			{
				pFlipBookComponent->AddFlipBook((CFlipBook*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}


    //ImGuiInputTextFlags_
    static float flipBookFPS = 0.f;
    static bool repeat = true;
    ImGui::Text("FPS");
    ImGui::SameLine(100); // 100 is the x position of the next element (in this case the input field
    ImGui::InputFloat("##FPS", &flipBookFPS, 1);
    
    ImGui::Text("Repeat");
    ImGui::SameLine(100); 
    ImGui::Checkbox("Repeat", &repeat);

    ImGui::BeginDisabled(flipBookFPS <= 0.f);
    if (ImGui::Button("Play##FlipBook"))
    {
		pFlipBookComponent->Play(flipBookNames[item_current_idx], flipBookFPS, repeat);
    }
    ImGui::EndDisabled();



    ImVec2 last_content_pos = ImGui::GetCursorPos();
    ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

    SetChildSize(content_size);
}

void FlipBookComUI::AddFlipBookByDialog()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			COMDLG_FILTERSPEC rgSpec[] = {
				{ L"Flip Files", L"*.flip" },
				{ L"All Files", L"*.*" }
			};
			pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
			pFileOpen->SetFileTypeIndex(1);
			pFileOpen->SetTitle(L"Load Flip File");

			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_lastFlipBookDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentPath() + L"animation";
			else
				defaultDirectory = m_lastFlipBookDirectory;
			hr = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFileOpen->SetFolder(psiFolder);
				psiFolder->Release();
			}

			hr = pFileOpen->Show(NULL);

			if (SUCCEEDED(hr)) {
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr)) {

						// .flip 파일을 처리하는 로직 추가
						CFlipBookComponent* pFlipBookComponent = GetTargetObject()->FlipBookComponent();
						wstring relativePath = CPathMgr::GetInst()->GetRelativePath(pszFilePath);
						if (path(relativePath).extension().wstring() != L".flip")
						{
							MessageBox(NULL, L"선택한 파일이 FlipBook 파일이 아닙니다.", L"Error", MB_OK);
							return;
						}

						Ptr<CFlipBook> pFlipBook = CAssetMgr::GetInst()->FindAsset<CFlipBook>(relativePath);

						if (pFlipBook == nullptr)
						{
							MessageBox(NULL, L"선택한 FlipBook이 AssetMgr에 존재하지 않습니다.", L"Error", MB_OK);
						}

						pFlipBookComponent->AddFlipBook(pFlipBook);

						path filePath = pszFilePath;
						m_lastFlipBookDirectory = filePath.parent_path().wstring();

						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
}
