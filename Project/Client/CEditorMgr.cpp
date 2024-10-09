#include "pch.h"
#include "CEditorMgr.h"

#include "CGameObjectEx.h"
#include "CEditorCameraScript.h"
#include "EditorUI.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CRenderMgr.h>

#include <Engine/components.h>
#include <Engine/CEngine.h>
#include <Engine/CDevice.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CPathMgr.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

CEditorMgr::CEditorMgr()
{

}

CEditorMgr::~CEditorMgr()
{
	Delete_Vec(m_vecEditorObject);
	Delete_Map(m_mapUI);

	// ImGui Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CEditorMgr::Init()
{
	CreateEditorObject();

	InitImGui();

	// Content 폴더를 감시하는 커널 오브젝트 생성
	wstring ContentsPath = CPathMgr::GetInst()->GetContentPath();
	m_hNotifyHandle = FindFirstChangeNotification(ContentsPath.c_str(), true
		, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
		| FILE_ACTION_ADDED | FILE_ACTION_REMOVED);
}


void CEditorMgr::Progress()
{
	ShortCut();

	EditorObjectProgress();

	ImGuiProgress();

	ObserveContent();
}


void CEditorMgr::ShortCut()
{
	if (KEY_JUST_PRESSED(KEY::I))
	{
		EditorUI* pUI = FindEditorUI("Inspector");

		if (pUI->IsActive())
			pUI->SetActive(false);
		else
			pUI->SetActive(true);
	}
}


void CEditorMgr::EditorObjectProgress()
{
	for (size_t i = 0; i < m_vecEditorObject.size(); ++i)
	{
		m_vecEditorObject[i]->Tick();
	}

	for (size_t i = 0; i < m_vecEditorObject.size(); ++i)
	{
		m_vecEditorObject[i]->FinalTick();
	}
}

void CEditorMgr::ImGuiTick()
{
	ImGui::ShowDemoWindow();

	for (const auto& pair : m_mapUI)
	{
		pair.second->Tick();
	}
}