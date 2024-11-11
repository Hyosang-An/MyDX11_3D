#include "pch.h"
#include "MenuUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CRenderMgr.h>

#include <Engine/CLevel.h>
#include <Engine/assets.h>
#include <Engine/CGameObject.h>
#include <Engine/CScript.h>
#include <Engine/CLayer.h>
#include <Scripts/CScriptMgr.h>


#include "CLevelSaveLoad.h"
#include "Inspector.h"
#include "CEditorMgr.h"

#include "IconsFontAwesome6/IconsFontAwesome6.h"

MenuUI::MenuUI()
{
}

MenuUI::~MenuUI()
{
}

void MenuUI::Init()
{
	m_lastSaveLoadDirectory = CPathMgr::GetInst()->GetContentPath() + L"level\\";
}

void MenuUI::Tick()
{
	if (!IsActive())
		return;

	if (ImGui::BeginMainMenuBar())
	{

		Update();

		ImGui::EndMainMenuBar();
	}
}


void MenuUI::Update()
{
	File();

	Level();

	GameObject();

	Assets();

	RenderTarget();

	FontsCheck();

	LevelPlayPauseStopButton();

}

void MenuUI::File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save Level"))
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr)) {
				IFileSaveDialog* pFileSave;

				// CLSID_FileSaveDialog Ŭ������ �ν��Ͻ��� �����ϰ� IFileSaveDialog �������̽� �����͸� �����ɴϴ�.
				hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

				if (SUCCEEDED(hr)) {
					// ���� ���� ���͸� �����մϴ�.
					COMDLG_FILTERSPEC rgSpec[] = {
						{ L"Level Files", L"*.level" },
						{ L"All Files", L"*.*" }
					};
					pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec); // ���͸� ��ȭ���ڿ� �����մϴ�.
					pFileSave->SetFileTypeIndex(1); // �⺻ ���� ������ �����մϴ�. rgSpec�� � ���� �⺻������ ����. Index�� 1����.
					pFileSave->SetDefaultExtension(L"level"); // �⺻ Ȯ���ڸ� �����մϴ�.
					pFileSave->SetTitle(L"Save Level File"); // ��ȭ������ ������ �����մϴ�.

					// ������ ��θ� �ʱ� ������ ����
					IShellItem* psiFolder = nullptr;
					hr = SHCreateItemFromParsingName(m_lastSaveLoadDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
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
								CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
								CLevelSaveLoad::SaveLevel(pszFilePath, pCurLevel);

								// ������ ���丮 ������Ʈ
								path filePath = pszFilePath;
								m_lastSaveLoadDirectory = filePath.parent_path().wstring();

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

		if (ImGui::MenuItem("Load Level"))
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr)) {
				IFileOpenDialog* pFileOpen;

				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

				if (SUCCEEDED(hr)) {
					COMDLG_FILTERSPEC rgSpec[] = {
						{ L"Level Files", L"*.level" },
						{ L"All Files", L"*.*" }
					};
					pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
					pFileOpen->SetFileTypeIndex(1);
					pFileOpen->SetTitle(L"Load Level File");

					IShellItem* psiFolder = nullptr;
					hr = SHCreateItemFromParsingName(m_lastSaveLoadDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
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

								if (path(pszFilePath).extension().wstring() == L".level")
								{
									CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(pszFilePath);
									ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);

									// Inspector Clear �ϱ� (���� ������Ʈ ������ �����ְ� ���� ���� �ֱ� ������)				
									Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
									pInspector->SetTargetObject(nullptr);
									pInspector->SetTargetAsset(nullptr);

									path filePath = pszFilePath;
									m_lastSaveLoadDirectory = filePath.parent_path().wstring();
								}

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

		ImGui::EndMenu();
	}
}

void MenuUI::Level()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	ImGui::BeginDisabled(!pCurLevel);

	if (ImGui::BeginMenu("Level"))
	{
		auto curState = pCurLevel->GetState();

		ImGui::BeginDisabled(LEVEL_STATE::PLAY == curState);
		if (ImGui::MenuItem("Play"))
		{
			if (LEVEL_STATE::STOP == curState)
			{
				wstring strLevelPath = CPathMgr::GetInst()->GetContentPath();
				strLevelPath += L"level\\Temp.level";
				CLevelSaveLoad::SaveLevel(strLevelPath, pCurLevel);
			}

			ChangeLevelState(LEVEL_STATE::PLAY);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(LEVEL_STATE::PLAY != curState);
		if (ImGui::MenuItem("Pause"))
		{
			ChangeLevelState(LEVEL_STATE::PAUSE);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(LEVEL_STATE::STOP == curState);
		if (ImGui::MenuItem("Stop"))
		{
			wstring StrLevelLoadPath = CPathMgr::GetInst()->GetContentPath();
			StrLevelLoadPath += L"level\\Temp.level";
			CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(StrLevelLoadPath);
			ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);

			// Inspector Clear �ϱ� (���� ������Ʈ ������ �����ְ� ���� ���� �ֱ� ������)				
			Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
			pInspector->SetTargetObject(nullptr);
			pInspector->SetTargetAsset(nullptr);
		}
		ImGui::EndDisabled();

		ImGui::EndMenu();
	}
	ImGui::EndDisabled();
}

void MenuUI::GameObject()
{
	ImGui::BeginDisabled(nullptr == CLevelMgr::GetInst()->GetCurrentLevel() || CLevelMgr::GetInst()->GetCurrentLevel()->GetState() != STOP);
	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::MenuItem("Create Empty Object"))
		{
			CGameObject* pObject = new CGameObject;
			wstring strObjDefaultName = L"New GameObject";
			wstring strObjName = strObjDefaultName + L" " + to_wstring(m_newObjIdx);
			// ���� ������ �ִ� ������Ʈ�� Ȯ���ϸ鼭 �ߺ��Ǵ� �̸��� �ִ��� Ȯ��
			CLevel* pCurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();
			for (UINT i = 0; i < MAX_LAYER; ++i)
			{
				CLayer* pLayer = pCurrentLevel->GetLayer(i);
				const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

				//for (size_t j = 0; j < vecObjects.size(); ++j)
				//{
				//	// �ߺ��� �̸��� �̹� ������ ���ڸ� �÷����鼭 �̸��� ����
				//	if (vecObjects[j]->GetName() == strObjName)
				//	{
				//		for (; m_newObjIdx < 0xffffffff; ++m_newObjIdx)
				//		{
				//			strObjName = strObjDefaultName + L" " + to_wstring(++m_newObjIdx);
				//			bool bExist = false;
				//			for (size_t k = 0; k < vecObjects.size(); ++k)
				//			{
				//				if (vecObjects[k]->GetName() == strObjName)
				//				{
				//					bExist = true;
				//					break;
				//				}
				//			}
				//			if (false == bExist)
				//				break;
				//		}
				//	}
				//}

				// �ߺ��� �̸��� �̹� ������ ���ڸ� �÷����鼭 �̸��� ����
				while (true)
				{
					bool bExist = false;
					for (size_t j = 0; j < vecObjects.size(); ++j)
					{
						if (vecObjects[j]->GetName() == strObjName)
						{
							bExist = true;
							break;
						}
					}

					if (false == bExist)
						break;

					strObjName = strObjDefaultName + L" " + to_wstring(++m_newObjIdx);
				}
			}

			pObject->SetName(strObjName);

			// transform �߰�
			pObject->AddComponent(new CTransform);

			// ������ ī�޶� ��ǥ�� ����
			Vec3 vPos = CEditorMgr::GetInst()->GetEditorCamera()->Transform()->GetWorldPos();
			pObject->Transform()->SetRelativePos(vPos);

			// ���� ������ �߰�
			CLevelMgr::GetInst()->GetCurrentLevel()->AddObject(0, pObject);
		}

		if (ImGui::BeginMenu("Add Component"))
		{
			ImGui::MenuItem("MeshRender");
			ImGui::MenuItem("Collider2D");
			ImGui::MenuItem("Camera");



			ImGui::EndMenu();
		}

		AddScript();


		ImGui::EndMenu();
	}
	ImGui::EndDisabled();
}


void MenuUI::AddScript()
{
	if (ImGui::BeginMenu("Add Script"))
	{
		vector<wstring> vecScriptsName;
		CScriptMgr::GetScriptInfo(vecScriptsName);

		for (size_t i = 0; i < vecScriptsName.size(); ++i)
		{
			if (ImGui::MenuItem(string(vecScriptsName[i].begin(), vecScriptsName[i].end()).c_str()))
			{
				// �ν�����
				Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");

				// Ÿ�� ������Ʈ �˾Ƴ�
				CGameObject* pObject = pInspector->GetTargetObject();

				// ������Ʈ��, ������ ��ũ��Ʈ�� �߰�����
				if (nullptr != pObject)
				{
					// ������Ʈ��, ������ ��ũ��Ʈ�� �߰�����
					CScript* pScript = CScriptMgr::GetScript(vecScriptsName[i]);
					pObject->AddComponent(pScript);
				}
			}
		}

		ImGui::EndMenu();
	}

}

void MenuUI::Assets()
{
	if (ImGui::BeginMenu("Assets"))
	{
		// Create Empty Material
		if (ImGui::MenuItem("Create Empty Material"))
		{
			Ptr<CMaterial> pMtrl = new CMaterial;

			// Key�� ���� �����
			wstring relativePathKey = CreateRelativePathAssetKey(ASSET_TYPE::MATERIAL, L"Default Material");
			CAssetMgr::GetInst()->AddAsset<CMaterial>(relativePathKey, pMtrl);
			pMtrl->Save(CPathMgr::GetInst()->GetContentPath() + relativePathKey);
		}

		// SpriteCreator
		EditorUI* pSpriteEditor = CEditorMgr::GetInst()->FindEditorUI("SpriteCreator");
		bool IsActive = pSpriteEditor->IsActive();

		if (ImGui::MenuItem("Create Sprite", nullptr, &IsActive))
		{
			CEditorMgr::GetInst()->FindEditorUI("SpriteCreator")->SetActive(IsActive);
		}

		// FlipBook Editor
		EditorUI* pFlipBookEditor = CEditorMgr::GetInst()->FindEditorUI("FlipBookEditor");
		IsActive = pFlipBookEditor->IsActive();

		if (ImGui::MenuItem("Edit FlipBook", nullptr, &IsActive))
		{
			CEditorMgr::GetInst()->FindEditorUI("FlipBookEditor")->SetActive(IsActive);
		}

		ImGui::EndMenu();
	}
}

void MenuUI::RenderTarget()
{
	if (ImGui::BeginMenu("RenderTarget"))
	{
		Ptr<CTexture> pTarget = CRenderMgr::GetInst()->GetSpecifiedTarget();

		auto AlbedoTargetTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"AlbedoTargetTex");
		auto NormalTargetTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"NormalTargetTex");
		auto PositionTargetTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex");
		auto EmissiveTargetTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"EmissiveTargetTex");
		auto DiffuseTargetTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DiffuseTargetTex");
		auto SpecularTargetTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"SpecularTargetTex");

		if (ImGui::MenuItem("Albedo Target", nullptr, pTarget == AlbedoTargetTex))
		{
			if (pTarget == AlbedoTargetTex)
				CRenderMgr::GetInst()->SetSpecifiedTarget(nullptr);
			else
				CRenderMgr::GetInst()->SetSpecifiedTarget(AlbedoTargetTex);
		}

		if (ImGui::MenuItem("Normal Target", nullptr, pTarget == NormalTargetTex))
		{
			if (pTarget == NormalTargetTex)
				CRenderMgr::GetInst()->SetSpecifiedTarget(nullptr);
			else
				CRenderMgr::GetInst()->SetSpecifiedTarget(NormalTargetTex);
		}

		if (ImGui::MenuItem("Position Target", nullptr, pTarget == PositionTargetTex))
		{
			if (pTarget == PositionTargetTex)
				CRenderMgr::GetInst()->SetSpecifiedTarget(nullptr);
			else
				CRenderMgr::GetInst()->SetSpecifiedTarget(PositionTargetTex);
		}

		if (ImGui::MenuItem("Emissive Target", nullptr, pTarget == EmissiveTargetTex))
		{
			if (pTarget == EmissiveTargetTex)
				CRenderMgr::GetInst()->SetSpecifiedTarget(nullptr);
			else
				CRenderMgr::GetInst()->SetSpecifiedTarget(EmissiveTargetTex);
		}

		if (ImGui::MenuItem("Diffuse Target", nullptr, pTarget == DiffuseTargetTex))
		{
			if (pTarget == DiffuseTargetTex)
				CRenderMgr::GetInst()->SetSpecifiedTarget(nullptr);
			else
				CRenderMgr::GetInst()->SetSpecifiedTarget(DiffuseTargetTex);
		}

		if (ImGui::MenuItem("Specular Target", nullptr, pTarget == SpecularTargetTex))
		{
			if (pTarget == SpecularTargetTex)
				CRenderMgr::GetInst()->SetSpecifiedTarget(nullptr);
			else
				CRenderMgr::GetInst()->SetSpecifiedTarget(SpecularTargetTex);
		}

		ImGui::EndMenu();
	}
}

void MenuUI::LevelPlayPauseStopButton()
{
	// �߾ӿ� ��ư ��ġ�ϱ� ���� ���
	float window_width = ImGui::GetWindowWidth(); // ���� â(���� �޴� ��)�� �ʺ� �����ɴϴ�.
	float button_width = 100.0f; // ��ư �׷��� ��ü �ʺ� (��ư �� ���� ����)
	float center_position_for_buttons = (window_width - button_width) / 2.0f; // �߾� ��ġ ���

	// ��ư�� �߾ӿ� ��ġ�ϱ� ���� Ŀ�� ��ġ ����
	ImGui::SetCursorPosX(center_position_for_buttons);

	// �÷��� ��ư
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	LEVEL_STATE curState = pCurLevel ? pCurLevel->GetState() : LEVEL_STATE::STOP; // ���� ������ ���¸� �����ɴϴ�.

	ImGui::BeginDisabled(!pCurLevel || LEVEL_STATE::PLAY == curState);
	if (ImGui::Button(ICON_FA_PLAY " Play")) {
		// Play ��ư ����
		if (LEVEL_STATE::STOP == curState)
		{
			wstring strLevelPath = CPathMgr::GetInst()->GetContentPath();
			strLevelPath += L"level\\Temp.level";
			CLevelSaveLoad::SaveLevel(strLevelPath, pCurLevel);
		}

		ChangeLevelState(LEVEL_STATE::PLAY);
	}
	ImGui::EndDisabled();


	ImGui::SameLine(); // ���� ��ư�� ���� �ٿ� ��ġ

	// �Ͻ����� ��ư
	ImGui::BeginDisabled(!pCurLevel || LEVEL_STATE::PLAY != curState);
	if (ImGui::Button(ICON_FA_PAUSE " Pause")) {
		// Pause ��ư ����
		ChangeLevelState(LEVEL_STATE::PAUSE);
	}
	ImGui::EndDisabled();

	//ImGui::SameLine(); // ���� ��ư�� ���� �ٿ� ��ġ

	// ���� ��ư
	ImGui::BeginDisabled(!pCurLevel || LEVEL_STATE::STOP == curState);
	if (ImGui::Button(ICON_FA_STOP " Stop")) {
		// Stop ��ư ����
		wstring StrLevelLoadPath = CPathMgr::GetInst()->GetContentPath();
		StrLevelLoadPath += L"level\\Temp.level";
		CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(StrLevelLoadPath);
		ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);

		// Inspector Clear �ϱ� (���� ������Ʈ ������ �����ְ� ���� ���� �ֱ� ������)				
		Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
		pInspector->SetTargetObject(nullptr);
		pInspector->SetTargetAsset(nullptr);
	}
	ImGui::EndDisabled();
}

void MenuUI::FontsCheck()
{
	if (ImGui::BeginMenu("Fonts Check"))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFontAtlas* atlas = io.Fonts;
		ImGui::Text("Number of fonts loaded: %d", atlas->Fonts.Size);

		for (int i = 0; i < atlas->Fonts.Size; i++)
		{
			ImFont* font = atlas->Fonts[i];
			ImGui::PushFont(font);
			ImGui::Text(u8"Font %d: %s, Size: %.2f �ѱ�", i, font->GetDebugName(), font->FontSize);
			ImGui::PopFont();
		}

		ImGui::EndMenu();
	}
}

wstring MenuUI::CreateRelativePathAssetKey(ASSET_TYPE _Type, const wstring& _KeyFormat)
{
	wstring Key;

	switch (_Type)
	{
	case ASSET_TYPE::MATERIAL:
	{
		Key = wstring(L"material\\") + _KeyFormat + L" %d.mtrl";
	}
	break;
	case ASSET_TYPE::PREFAB:
	{
		Key = wstring(L"prefab\\") + _KeyFormat + L" %d.pref";
	}
	break;
	case ASSET_TYPE::SPRITE:
	{
		Key = wstring(L"sprite\\") + _KeyFormat + L" %d.sprite";
	}
	break;
	case ASSET_TYPE::FLIPBOOK:
	{
		Key = wstring(L"flipbook\\") + _KeyFormat + L" %d.flip";
	}
	break;
	default:
		assert(nullptr);
		break;
	}

	wchar_t szKey[255] = {};
	wstring FilePath = CPathMgr::GetInst()->GetContentPath();

	for (UINT i = 0; i < 0xffffffff; ++i)
	{
		swprintf_s(szKey, 255, Key.c_str(), i);

		if (false == std::filesystem::exists(FilePath + szKey))
		{
			break;
		}
	}

	return szKey;
}
