#include "pch.h"
#include "CEngine.h"

#include "CDevice.h"

#include "CPathMgr.h"
#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CAssetMgr.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CCollisionMgr.h"
#include "CTaskMgr.h"
#include "CPrefab.h"
#include "CFontMgr.h"

CEngine::CEngine() :
	m_hWnd{}
{
}

CEngine::~CEngine()
{
	if (nullptr != m_FMODSystem)
	{
		m_FMODSystem->release();
		m_FMODSystem = nullptr;
	}
}

int CEngine::Init(HWND _wnd, POINT _ptResolution, OBJECT_SAVE _SaveFunc, OBJECT_LOAD _LoadFunc)
{
	m_hWnd = _wnd;
	ChangeWindowScale(_ptResolution.x, _ptResolution.y);

	if (FAILED(CDevice::GetInst()->Init(m_hWnd, _ptResolution.x, _ptResolution.y)))
	{
		MessageBox(nullptr, L"장치 초기화 실패", L"CDevice 초기화 실패", MB_OK);
		return E_FAIL;
	}

	// FMOD 초기화		
	FMOD::System_Create(&m_FMODSystem);
	assert(m_FMODSystem);

	// 32개 채널 생성
	m_FMODSystem->init(32, FMOD_DEFAULT, nullptr);

	// Manager 초기화
	CPathMgr::GetInst()->Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();
	CAssetMgr::GetInst()->Init();
	CRenderMgr::GetInst()->Init();	// AssetMgr에서 렌더링에 필요한 Texture들을 생성한 뒤 RenderMgr 초기화
	
	CLevelMgr::GetInst()->Init();
	CFontMgr::GetInst()->Init();

	// Prefab Function 등록
	CPrefab::g_ObjectSaveFunc = _SaveFunc;
	CPrefab::g_ObjectLoadFunc = _LoadFunc;

	return S_OK;
}

void CEngine::Progress()
{
	// FMOD Tick
	m_FMODSystem->update();

	// Manager
	CKeyMgr::GetInst()->Tick();
	CTimeMgr::GetInst()->Tick();
	CAssetMgr::GetInst()->Tick();
	CLevelMgr::GetInst()->Progress();

	// Collision
	CCollisionMgr::GetInst()->Tick();

	// Render
	CRenderMgr::GetInst()->Tick();

	// TaskMgr
	CTaskMgr::GetInst()->Tick();
}

void CEngine::ChangeWindowScale(UINT _width, UINT _height)
{
	bool bMenu = false;
	if (GetMenu(m_hWnd))
		bMenu = true;

	RECT rt{ 0,0, (int)_width, (int)_height };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, bMenu);
	SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
}

void CEngine::ResizeWindowResolution(UINT _newWidth, UINT _newHeight)
{
	ChangeWindowScale(_newWidth, _newHeight);
	CDevice::GetInst()->ResizeResolution(_newWidth, _newHeight);
}


void CEngine::ToggleFullScreen()
{
	static RECT windowRect;  // 기존 창의 크기와 위치를 저장할 변수

	// 현재 윈도우 스타일 가져오기
	DWORD style = GetWindowLong(m_hWnd, GWL_STYLE);
	MONITORINFO mi = { sizeof(mi) };

	if (style & WS_OVERLAPPEDWINDOW)  // 윈도우가 전체화면이 아닐 때
	{
		// 현재의 해상도 저장
		m_prevWindowModeResolution = CDevice::GetInst()->GetResolution();

		// 현재 모니터의 정보 가져오기
		if (GetWindowRect(m_hWnd, &windowRect) && GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &mi))
		{
			// 윈도우 스타일을 전체화면용으로 변경 (타이틀 바, 테두리 제거)
			SetWindowLong(m_hWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);

			// 윈도우 크기를 전체 화면으로 설정
			SetWindowPos(m_hWnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			// 해상도는 어차피 전체화면이므로 모니터의 해상도로 설정
			CDevice::GetInst()->ResizeResolution(mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);
		}
	}
	else  // 윈도우가 이미 전체화면일 때
	{
		// 기존의 윈도우 스타일로 되돌림 (타이틀 바, 테두리 복구)
		SetWindowLong(m_hWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);

		// 기존의 윈도우 크기와 위치로 되돌림
		SetWindowPos(m_hWnd, NULL, windowRect.left, windowRect.top,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		CDevice::GetInst()->ResizeResolution(m_prevWindowModeResolution.x, m_prevWindowModeResolution.y);
	}
}