#include "pch.h"
#include "MenuUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/CLevelMgr.h>
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

				// CLSID_FileSaveDialog 클래스의 인스턴스를 생성하고 IFileSaveDialog 인터페이스 포인터를 가져옵니다.
				hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

				if (SUCCEEDED(hr)) {
					// 파일 형식 필터를 설정합니다.
					COMDLG_FILTERSPEC rgSpec[] = {
						{ L"Level Files", L"*.level" },
						{ L"All Files", L"*.*" }
					};
					pFileSave->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec); // 필터를 대화상자에 설정합니다.
					pFileSave->SetFileTypeIndex(1); // 기본 파일 형식을 설정합니다. rgSpec중 어떤 것을 기본값으로 할지. Index는 1부터.
					pFileSave->SetDefaultExtension(L"level"); // 기본 확장자를 설정합니다.
					pFileSave->SetTitle(L"Save Level File"); // 대화상자의 제목을 설정합니다.

					// 마지막 경로를 초기 폴더로 설정
					IShellItem* psiFolder = nullptr;
					hr = SHCreateItemFromParsingName(m_lastSaveLoadDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
					if (SUCCEEDED(hr) && psiFolder) {
						pFileSave->SetFolder(psiFolder);
						psiFolder->Release();
					}

					// 파일 저장 대화상자를 표시합니다.
					hr = pFileSave->Show(NULL);

					if (SUCCEEDED(hr)) {
						IShellItem* pItem;

						// 사용자가 선택한 파일의 IShellItem을 가져옵니다.
						hr = pFileSave->GetResult(&pItem);
						if (SUCCEEDED(hr)) {
							PWSTR pszFilePath;

							// IShellItem에서 파일 경로를 가져옵니다.
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
							if (SUCCEEDED(hr)) {
								// 여기에서 pszFilePath 경로를 사용하여 파일을 저장합니다.
								CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
								CLevelSaveLoad::SaveLevel(pszFilePath, pCurLevel);

								// 마지막 디렉토리 업데이트
								path filePath = pszFilePath;
								m_lastSaveLoadDirectory = filePath.parent_path().wstring();

								// 파일 경로 사용 후 메모리를 해제합니다.
								CoTaskMemFree(pszFilePath);
							}
							pItem->Release(); // IShellItem 포인터를 해제합니다.
						}
					}
					pFileSave->Release(); // IFileSaveDialog 포인터를 해제합니다.
				}
				CoUninitialize(); // COM 라이브러리를 종료합니다.
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

									// Inspector Clear 하기 (이전 오브젝트 정보를 보여주고 있을 수가 있기 때문에)				
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

			// Inspector Clear 하기 (이전 오브젝트 정보를 보여주고 있을 수가 있기 때문에)				
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
			// 현재 레벨에 있는 오브젝트들 확인하면서 중복되는 이름이 있는지 확인
			CLevel* pCurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();
			for (UINT i = 0; i < MAX_LAYER; ++i)
			{
				CLayer* pLayer = pCurrentLevel->GetLayer(i);
				const vector<CGameObject*>& vecObjects = pLayer->GetObjects();

				//for (size_t j = 0; j < vecObjects.size(); ++j)
				//{
				//	// 중복된 이름이 이미 있으면 숫자를 늘려가면서 이름을 만듦
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

				// 중복된 이름이 이미 있으면 숫자를 늘려가면서 이름을 만듦
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

			// transform 추가
			pObject->AddComponent(new CTransform);

			// 에디터 카메라 좌표에 생성
			Vec3 vPos = CEditorMgr::GetInst()->GetEditorCamera()->Transform()->GetWorldPos();
			pObject->Transform()->SetRelativePos(vPos);

			// 현재 레벨에 추가
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
				// 인스펙터
				Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");

				// 타겟 오브젝트 알아냄
				CGameObject* pObject = pInspector->GetTargetObject();

				// 오브젝트에, 선택한 스크립트를 추가해줌
				if (nullptr != pObject)
				{
					// 오브젝트에, 선택한 스크립트를 추가해줌
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

			// Key는 저장 상대경로
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

void MenuUI::LevelPlayPauseStopButton()
{
	// 중앙에 버튼 배치하기 위한 계산
	float window_width = ImGui::GetWindowWidth(); // 현재 창(메인 메뉴 바)의 너비를 가져옵니다.
	float button_width = 100.0f; // 버튼 그룹의 전체 너비 (버튼 간 간격 포함)
	float center_position_for_buttons = (window_width - button_width) / 2.0f; // 중앙 위치 계산

	// 버튼을 중앙에 배치하기 위해 커서 위치 조정
	ImGui::SetCursorPosX(center_position_for_buttons);

	// 플레이 버튼
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	LEVEL_STATE curState = pCurLevel ? pCurLevel->GetState() : LEVEL_STATE::STOP; // 현재 레벨의 상태를 가져옵니다.

	ImGui::BeginDisabled(!pCurLevel || LEVEL_STATE::PLAY == curState);
	if (ImGui::Button(ICON_FA_PLAY " Play")) {
		// Play 버튼 동작
		if (LEVEL_STATE::STOP == curState)
		{
			wstring strLevelPath = CPathMgr::GetInst()->GetContentPath();
			strLevelPath += L"level\\Temp.level";
			CLevelSaveLoad::SaveLevel(strLevelPath, pCurLevel);
		}

		ChangeLevelState(LEVEL_STATE::PLAY);
	}
	ImGui::EndDisabled();


	ImGui::SameLine(); // 다음 버튼을 같은 줄에 배치

	// 일시정지 버튼
	ImGui::BeginDisabled(!pCurLevel || LEVEL_STATE::PLAY != curState);
	if (ImGui::Button(ICON_FA_PAUSE " Pause")) {
		// Pause 버튼 동작
		ChangeLevelState(LEVEL_STATE::PAUSE);
	}
	ImGui::EndDisabled();

	//ImGui::SameLine(); // 다음 버튼을 같은 줄에 배치

	// 정지 버튼
	ImGui::BeginDisabled(!pCurLevel || LEVEL_STATE::STOP == curState);
	if (ImGui::Button(ICON_FA_STOP " Stop")) {
		// Stop 버튼 동작
		wstring StrLevelLoadPath = CPathMgr::GetInst()->GetContentPath();
		StrLevelLoadPath += L"level\\Temp.level";
		CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(StrLevelLoadPath);
		ChangeLevel(pLoadedLevel, LEVEL_STATE::STOP);

		// Inspector Clear 하기 (이전 오브젝트 정보를 보여주고 있을 수가 있기 때문에)				
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
			ImGui::Text(u8"Font %d: %s, Size: %.2f 한글", i, font->GetDebugName(), font->FontSize);
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
