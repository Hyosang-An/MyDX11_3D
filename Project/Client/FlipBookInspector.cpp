#include "pch.h"
#include "FlipBookInspector.h"
#include <Engine/CAssetMgr.h>
#include <Engine/CTimeMgr.h>

#include "TreeUI.h"

FlipBookInspector::FlipBookInspector()
{
}

FlipBookInspector::~FlipBookInspector()
{
}


void FlipBookInspector::Init()
{

}

void FlipBookInspector::Update()
{
	ImGui::Text("This is the FlipBookInspector.");

	// New FlipBook Button
	if (ImGui::Button("New FlipBook"))
	{
		// Create a new FlipBook
		Ptr<CFlipBook> newFlipBook = new CFlipBook();
		m_owner->SetFlipBook(newFlipBook);
		m_selectedSpriteIndex = -1;
	}

	// ���� ���õ� FlipBook
	ImGui::BeginChild("Selected Sprite", ImVec2(0, 400), ImGuiChildFlags_None, ImGuiWindowFlags_None);
	{
		// FlipBook ����
		SelectFlipBook();

		// FlipBook�� Sprite ����Ʈ
		FlipBookSpriteList();

		// ���õ� Sprite ����
		SetSpriteOffsetAndBackgroundSize();

		// ���õ� FlipBook ����
		if (m_selectedFlipBook.Get())
		{
			// ���� ��ư
			if (ImGui::Button("Save FlipBook"))
			{
				SaveFlipBook(m_selectedFlipBook);
			}
		}
	}
	ImGui::EndChild();


	ImGui::Separator();
	//================================================================================================================================================================================
	//================================================================================================================================================================================


	// ��������Ʈ ���� ���� �� ��������Ʈ ����Ʈ ���
	SelectSpreteFolderAndShowSprites();
}


void FlipBookInspector::SelectFlipBook()
{
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.8f));

	ImGui::Button("Selected FlipBook");

	ImGui::PopStyleColor(3);

	m_selectedFlipBook = m_owner->GetFlipBook();
	string flipBookName;

	if (m_selectedFlipBook.Get())
	{
		if (m_selectedFlipBook->GetKey().empty())
			flipBookName = "New FlipBook";
		else
		{
			flipBookName = string(m_selectedFlipBook->GetKey().begin(), m_selectedFlipBook->GetKey().end());
			flipBookName = path(flipBookName).stem().string();
		}
	}

	ImGui::Text("FlipBook");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##FlipBookKey", (char*)flipBookName.c_str(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		// �����͸� ��� ���� Ʈ�� �̸� : ContentTree
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
		if (payload)
		{
			// payload�� Data�� TreeNode*�� ����Ű�� ������
			TreeNode** ppNode = (TreeNode**)payload->Data;
			TreeNode* pNode = *ppNode;

			Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();
			if (pAsset != nullptr && ASSET_TYPE::FLIPBOOK == pAsset->GetAssetType())
			{
				m_owner->SetFlipBook((CFlipBook*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	if (ImGui::Button("...##FlipBook"))
	{
		SelectFlipBookByDialog();
	}
}

void FlipBookInspector::FlipBookSpriteList()
{
	if (!m_selectedFlipBook.Get())
	{
		ImGui::Text("FlipBook is not selected.");
		return;
	}

	ImGui::BeginChild("Sprite List", ImVec2(0, 200), ImGuiChildFlags_Border, ImGuiWindowFlags_None);

	isSpriteListFocused = ImGui::IsWindowFocused();

	vector<Ptr<CSprite>>& spriteVecInFlipBook = m_selectedFlipBook->GetSpriteVec();
	static float maxWidth = 0;
	static bool firstLoop = true;

	for (int i = 0; i < spriteVecInFlipBook.size(); i++)
	{
		Ptr<CSprite>& pSprite = spriteVecInFlipBook[i];
		wstring wstrKey = pSprite->GetKey();
		string strKey = string(wstrKey.begin(), wstrKey.end());

		string spriteName = path(strKey).stem().string();

		float selectableWidth = 0;
		if (firstLoop == false)
			selectableWidth = maxWidth;

		if (ImGui::Selectable(spriteName.c_str(), (m_selectedSpriteIndex == i), ImGuiSelectableFlags_None, { selectableWidth, 0 }))
		{
			m_selectedSpriteIndex = i;
		}

		if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) // �������� Ȱ��ȭ�Ǿ� ������ ȣ������ ���� ���
		{
			// �巡�� ���⿡ ���� ������ �̵�
			int i_next = i + (ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y < 0.f ? -1 : 1);
			if (i_next >= 0 && i_next < spriteVecInFlipBook.size())
			{
				// ���� �ε����� ���� �ε����� �ٸ��� ������ ��ġ ��ȯ
				if (i != i_next) {
					std::swap(spriteVecInFlipBook[i], spriteVecInFlipBook[i_next]);
					ImGui::ResetMouseDragDelta(); // �巡�� ��ȭ�� ����
				}
			}
		}

		ImGui::SameLine();

		if (firstLoop)
		{
			auto pos = ImGui::GetCursorPos();

			maxWidth = max(pos.x, maxWidth);
		}

		if (ImGui::SmallButton(("-##" + std::to_string(i)).c_str()))
		{
			m_selectedFlipBook->RemoveSprite(i);

			m_selectedSpriteIndex = -1;
		}

		if (i == spriteVecInFlipBook.size() - 1)
			firstLoop = false;
	}

	// ����Ű�� ���� ���õ� Sprite �ٲٱ�
	if (ImGui::IsWindowFocused())
	{
		if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
		{
			if (m_selectedSpriteIndex > 0)
			{
				m_selectedSpriteIndex--;
			}
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
		{
			if (m_selectedSpriteIndex < spriteVecInFlipBook.size() - 1)
			{
				m_selectedSpriteIndex++;
			}
		}
	}

	ImGui::EndChild();


	
}

void FlipBookInspector::SetSpriteOffsetAndBackgroundSize()
{
	if (!m_selectedFlipBook.Get())
		return;

	if (m_selectedSpriteIndex == -1)
		return;

	// ������ flipBook�� offset ����
	ImGui::Text("Offset");
	ImGui::SameLine();
	Vec2 offsetPixel = m_selectedFlipBook->GetSprite((int)m_selectedSpriteIndex)->GetOffsetPixel();
	Vec2 prevOffsetPixel = offsetPixel;
	ImGui::DragFloat2("##Sprite Offset FlipBookInspector", offsetPixel, 1, 0, 0, "%.0f");
	m_selectedFlipBook->GetSprite(m_selectedSpriteIndex)->SetOffsetPixel(offsetPixel);
	
	// WASD�� ���õ� Sprite�� offset ���� 
	if (isSpriteListFocused)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_W))
		{
			//Vec2 offsetPixel = m_selectedFlipBook->GetSprite((int)m_selectedSpriteIndex)->GetOffsetPixel();
			offsetPixel.y -= 1;
			m_selectedFlipBook->GetSprite(m_selectedSpriteIndex)->SetOffsetPixel(offsetPixel);
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_S))
		{
			//Vec2 offsetPixel = m_selectedFlipBook->GetSprite((int)m_selectedSpriteIndex)->GetOffsetPixel();
			offsetPixel.y += 1;
			m_selectedFlipBook->GetSprite(m_selectedSpriteIndex)->SetOffsetPixel(offsetPixel);
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_A))
		{
			//Vec2 offsetPixel = m_selectedFlipBook->GetSprite((int)m_selectedSpriteIndex)->GetOffsetPixel();
			offsetPixel.x -= 1;
			m_selectedFlipBook->GetSprite(m_selectedSpriteIndex)->SetOffsetPixel(offsetPixel);
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_D))
		{
			//Vec2 offsetPixel = m_selectedFlipBook->GetSprite((int)m_selectedSpriteIndex)->GetOffsetPixel();
			offsetPixel.x += 1;
			m_selectedFlipBook->GetSprite(m_selectedSpriteIndex)->SetOffsetPixel(offsetPixel);
		}
	}

	if (prevOffsetPixel != offsetPixel)
	{
		SetSpriteChanged();
	}


	// ������ flipBook�� ��� ũ�� ����
	Vec2 backgroundPixelSize = m_selectedFlipBook->GetSprite((int)m_selectedSpriteIndex)->GetBackgroundSizeInAtlasUV() * Vec2(m_selectedFlipBook->GetSprite((int)m_selectedSpriteIndex)->GetAtlasTexture()->Width(), m_selectedFlipBook->GetSprite((int)m_selectedSpriteIndex)->GetAtlasTexture()->Height());
	Vec2 prevBackgroundPixelSize = backgroundPixelSize;
	ImGui::Text("Set All Sprites Background Size");
	//ImGui::SameLine();
	ImGui::InputFloat2("##Sprite Background Size", (float*)&backgroundPixelSize, "%.0f");
	m_selectedFlipBook->SetAllSpritesBackgroundPixelSize(backgroundPixelSize);

	if (prevBackgroundPixelSize != backgroundPixelSize)
	{
		SetSpriteChanged();
	}

	// ���� Sprite�� ������� 3�ʰ� ������ ����
	if (m_isSpriteChanged)
	{
		m_accTimeSinceSpriteChanged += CTimeMgr::GetInst()->GetEngineDeltaTime();
		if (m_accTimeSinceSpriteChanged > 3.0f)
		{
			vector<Ptr<CSprite>>& spriteVecInFlipBook = m_selectedFlipBook->GetSpriteVec();
			for(Ptr<CSprite>& pSprite : spriteVecInFlipBook)
				pSprite->Save(CPathMgr::GetInst()->GetContentPath() + pSprite->GetRelativePath());

			m_isSpriteChanged = false;
			m_accTimeSinceSpriteChanged = 0.0f;
		}
	}
}

void FlipBookInspector::SetSpriteChanged()
{
	m_isSpriteChanged = true;
	m_accTimeSinceSpriteChanged = 0;
}

void FlipBookInspector::SaveFlipBook(Ptr<CFlipBook> FlipBookToSave)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileSaveDialog* pFileSave;

		// CLSID_FileSaveDialog Ŭ������ �ν��Ͻ��� �����ϰ� IFileSaveDialog �������̽� �����͸� �����ɴϴ�.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr)) {
			// ���� ���� ���͸� �����մϴ�.
			COMDLG_FILTERSPEC rgSpec[] = {
				{ L"FlipBook Files", L"*.flip" },
				{ L"All Files", L"*.*" }
			};
			pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec); // ���͸� ��ȭ���ڿ� �����մϴ�.
			pFileSave->SetFileTypeIndex(1); // �⺻ ���� ������ �����մϴ�. rgSpec�� � ���� �⺻������ ����. Index�� 1����.
			pFileSave->SetDefaultExtension(L"flip"); // �⺻ Ȯ���ڸ� �����մϴ�.
			pFileSave->SetTitle(L"Save Flip File"); // ��ȭ������ ������ �����մϴ�.

			// ������ ��θ� �ʱ� ������ ����
			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_lastFlipBookDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentPath() + L"animation";
			else
				defaultDirectory = m_lastFlipBookDirectory;
			hr = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFileSave->SetFolder(psiFolder);
				psiFolder->Release();
			}

			// ���� ���� ��ȭ���ڸ� ǥ���մϴ�.
			hr = pFileSave->Show(NULL);

			if (SUCCEEDED(hr)) {
				IShellItem* pItem;

				// ����ڰ� ������ ������ IShellItem�� �����ɴϴ�.
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;

					// IShellItem���� ���� ��θ� �����ɴϴ�.
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr)) {
						// ���⿡�� pszFilePath ��θ� ����Ͽ� ������ �����մϴ�.
						// Save Selected FlipBook
						FlipBookToSave->Save(pszFilePath);

						// AssetMgr�� FlipBook �߰�
						CAssetMgr::GetInst()->AddAsset(CPathMgr::GetInst()->GetRelativePath(pszFilePath), FlipBookToSave);

						// ������ ���丮 ������Ʈ
						path filePath = pszFilePath;
						m_lastFlipBookDirectory = filePath.parent_path().wstring();

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

void FlipBookInspector::SelectSpreteFolderAndShowSprites()
{
	static float maxSelectableWidth = 0;

	if (ImGui::Button("Select Sprite Folder"))
	{
		if (SUCCEEDED(SelectSpriteFolderPathByDialog()))
		{
			GetSpritesFromSelectedFolder(m_selectedSpriteDirectory);

			// ���ο� ���� ���� �� maxSelectableWidth �ʱ�ȭ
			maxSelectableWidth = 0;
		}
	}

	ImGui::Text("Selected Folder");
	ImGui::SameLine();

	wstring relativePath = CPathMgr::GetInst()->GetRelativePath(m_selectedSpriteDirectory);
	auto strPath = string(relativePath.begin(), relativePath.end());

	// �ؽ�Ʈ ũ�� ���
	ImVec2 textSize = ImGui::CalcTextSize(strPath.c_str());

	// �е� �� ���� ���� �߰�
	float padding = 20.0f; // �ؽ�Ʈ �¿��� ���� ����
	float inputTextWidth = textSize.x + padding;

	// ���� ũ�⸦ ����Ͽ� ���� �׸��� �ʺ� ����
	ImGui::SetNextItemWidth(inputTextWidth);
	ImGui::InputText("##Selected Sprite Folder Path", (char*)strPath.c_str(), strPath.length(), ImGuiInputTextFlags_ReadOnly);


	// ���� �� sprite���� ����Ʈ�� �����ְ� �߰� ��ư �����
	ImGui::BeginChild("Sprite List2", ImVec2(0, 200), ImGuiChildFlags_Border, ImGuiWindowFlags_None);
	{
		if (m_selectedFlipBook.Get())
		{
			vector<Ptr<CSprite>>& spriteVecInFlipBook = m_selectedFlipBook->GetSpriteVec();

			for (int i = 0; i < m_vecSpriteInFolder.size(); i++)
			{
				Ptr<CSprite>& pSprite = m_vecSpriteInFolder[i];
				wstring wstrKey = pSprite->GetKey();
				string strKey = string(wstrKey.begin(), wstrKey.end());

				string spriteName = path(strKey).stem().string();

				bool bSelected = spriteVecInFlipBook.end() != std::find(spriteVecInFlipBook.begin(), spriteVecInFlipBook.end(), pSprite);
				
				maxSelectableWidth = max(ImGui::CalcTextSize(spriteName.c_str()).x, maxSelectableWidth);
				ImGui::Selectable(spriteName.c_str(), &bSelected, ImGuiSelectableFlags_None, { maxSelectableWidth, 0 });

				ImGui::SameLine();
				if (ImGui::SmallButton(("+##" + std::to_string(i)).c_str()))
				{
					m_selectedFlipBook->AddSprite(pSprite);
				}
			}
		}
		else
		{
			for (int i = 0; i < m_vecSpriteInFolder.size(); i++)
			{
				Ptr<CSprite>& pSprite = m_vecSpriteInFolder[i];
				wstring wstrKey = pSprite->GetKey();
				string strKey = string(wstrKey.begin(), wstrKey.end());

				string spriteName = path(strKey).stem().string();

				bool bSelected = false;
				
				maxSelectableWidth = max(ImGui::CalcTextSize(spriteName.c_str()).x, maxSelectableWidth);
				ImGui::Selectable(spriteName.c_str(), &bSelected, ImGuiSelectableFlags_None, { maxSelectableWidth, 0 });


				ImGui::SameLine();
				if (ImGui::SmallButton("+"))
				{
					
				}
			}
		}

		
	}
	ImGui::EndChild();





	static float maxSelectableWidth_test = 0;
	vector<string> m_vecString = { "a", "ab", "abc" };
	for (string& str : m_vecString)
	{
		bool b = false;
		ImGui::Selectable(str.c_str(), &b, ImGuiSelectableFlags_None, { maxSelectableWidth_test, 0 });
		maxSelectableWidth_test = max(ImGui::CalcTextSize(str.c_str()).x, maxSelectableWidth_test);


		

		ImGui::SameLine();
		if (ImGui::SmallButton("+"))
		{

		}
	}
}

HRESULT FlipBookInspector::SelectSpriteFolderPathByDialog()
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
			if (m_selectedSpriteDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentPath() + L"animation";
			else
				defaultDirectory = m_selectedSpriteDirectory;
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

						m_selectedSpriteDirectory = pszFolderPath;

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

void FlipBookInspector::SelectFlipBookByDialog()
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

						// .flip ������ ó���ϴ� ���� �߰�
						wstring relativePath = CPathMgr::GetInst()->GetRelativePath(pszFilePath);
						if (path(relativePath).extension().wstring() != L".flip")
						{
							MessageBox(NULL, L"������ ������ FlipBook ������ �ƴմϴ�.", L"Error", MB_OK);
							return;
						}

						m_owner->SetFlipBook(CAssetMgr::GetInst()->FindAsset<CFlipBook>(relativePath));

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


void FlipBookInspector::GetSpritesFromSelectedFolder(wstring _folderPath)
{
	// sprite ���� �ʱ�ȭ
	m_vecSpriteInFolder.clear();

	WIN32_FIND_DATA tFindData = {};

	// ��� ���� �齽���ð� ������ �߰�
	if (_folderPath.back() != L'\\' && _folderPath.back() != L'/')
	{
		_folderPath += L"\\";
	}

	// ��ο� �´� ������ Ž���� �� �ִ� Ŀ�� ������Ʈ ����
	wstring strFindPath = _folderPath + L"*.sprite";
	HANDLE hFinder = FindFirstFile(strFindPath.c_str(), &tFindData);
	if (hFinder == INVALID_HANDLE_VALUE)
	{
		// ��θ� ã�� �� ���� ��� ���� �޽��� ��� �� ��ȯ
		wprintf(L"Error: ��θ� ã�� �� �����ϴ�. ���: %s\n", _folderPath.c_str());
		return;
	}

	do
	{
		wstring strFindName = tFindData.cFileName;

		// sprite �����̸� �߰�
		wstring fullPath = _folderPath + strFindName;
		wstring relativePath = CPathMgr::GetInst()->GetRelativePath(fullPath);
		Ptr<CSprite> pSprite = CAssetMgr::GetInst()->FindAsset<CSprite>(relativePath);

		if (pSprite != nullptr) {
			m_vecSpriteInFolder.push_back(pSprite);
		}
		else {
			// ã�� �� ���� ��� ��� �޽���
			wprintf(L"Warning: CSprite ��ü�� ã�� �� �����ϴ�. ���: %s\n", fullPath.c_str());
		}

	} while (FindNextFile(hFinder, &tFindData) != 0);

	// �ڵ� �ݱ�
	FindClose(hFinder);
}












// ================================================================================================================================================================================
// �׽�Ʈ �ڵ�
// ================================================================================================================================================================================
// 
// 

	//static bool firstForLoop = true;
	//static float maxSelectableWidth = 0;
	//vector<string> m_vecString = { "a", "ab", "abc" };
	//for (string& str : m_vecString)
	//{
	//	bool b = false;
	//	ImGui::Selectable(str.c_str(), &b, ImGuiSelectableFlags_None, { maxSelectableWidth, 0 });
	//	ImGui::SameLine();
	//	if (firstForLoop)
	//	{
	//		auto pos = ImGui::GetCursorPos();

	//		maxSelectableWidth = fmax(pos.x, maxSelectableWidth);
	//	}

	//	if (ImGui::SmallButton("+"))
	//	{

	//	}
	//}

	//firstForLoop = false;




//ImGui::TextDisabled("(?)");
//if (ImGui::BeginItemTooltip())
//{
//	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
//	ImGui::TextUnformatted("We don't use the drag and drop api at all here!\n"
//		"Instead we query when the item is held but not hovered, and order items accordingly.");
//	ImGui::PopTextWrapPos();
//	ImGui::EndTooltip();
//}
//
//
// // ����Ʈ ���� �ٲٱ�
//static std::vector<std::string> item_names = {
//	"Item One", "Item Two", "Item Three", "Item Four", "Item Five"
//};
//
////int selected_index = 2; // ������ �ٸ��� ������ �׸��� �ε���
//
//for (int n = 0; n < item_names.size(); n++)
//{
//	const std::string& item = item_names[n];
//
//	static int selected_index = -1;
//
//	if (ImGui::Selectable(item.c_str(), selected_index == n)) // �׸� ǥ��, ���õ� �׸��� ImGuiCol_Header ������ ǥ����.
//	{
//		selected_index = n;
//	}
//
//	if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) // �������� Ȱ��ȭ�Ǿ� ������ ȣ������ ���� ���
//	{
//		// �巡�� ���⿡ ���� ������ �̵�
//		int n_next = n + (ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y < 0.f ? -1 : 1);
//		if (n_next >= 0 && n_next < item_names.size())
//		{
//			// ���� �ε����� ���� �ε����� �ٸ��� ������ ��ġ ��ȯ
//			if (n != n_next) {
//				std::swap(item_names[n], item_names[n_next]);
//				ImGui::ResetMouseDragDelta(); // �巡�� ��ȭ�� ����
//			}
//		}
//	}
//}