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
		MessageBox(nullptr, L"��ġ �ʱ�ȭ ����", L"CDevice �ʱ�ȭ ����", MB_OK);
		return E_FAIL;
	}

	// FMOD �ʱ�ȭ		
	FMOD::System_Create(&m_FMODSystem);
	assert(m_FMODSystem);

	// 32�� ä�� ����
	m_FMODSystem->init(32, FMOD_DEFAULT, nullptr);

	// Manager �ʱ�ȭ
	CPathMgr::GetInst()->Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();
	CAssetMgr::GetInst()->Init();
	CRenderMgr::GetInst()->Init();	// AssetMgr���� �������� �ʿ��� Texture���� ������ �� RenderMgr �ʱ�ȭ
	
	CLevelMgr::GetInst()->Init();
	CFontMgr::GetInst()->Init();

	// Prefab Function ���
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
	static RECT windowRect;  // ���� â�� ũ��� ��ġ�� ������ ����

	// ���� ������ ��Ÿ�� ��������
	DWORD style = GetWindowLong(m_hWnd, GWL_STYLE);
	MONITORINFO mi = { sizeof(mi) };

	if (style & WS_OVERLAPPEDWINDOW)  // �����찡 ��üȭ���� �ƴ� ��
	{
		// ������ �ػ� ����
		m_prevWindowModeResolution = CDevice::GetInst()->GetResolution();

		// ���� ������� ���� ��������
		if (GetWindowRect(m_hWnd, &windowRect) && GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &mi))
		{
			// ������ ��Ÿ���� ��üȭ������� ���� (Ÿ��Ʋ ��, �׵θ� ����)
			SetWindowLong(m_hWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);

			// ������ ũ�⸦ ��ü ȭ������ ����
			SetWindowPos(m_hWnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			// �ػ󵵴� ������ ��üȭ���̹Ƿ� ������� �ػ󵵷� ����
			CDevice::GetInst()->ResizeResolution(mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);
		}
	}
	else  // �����찡 �̹� ��üȭ���� ��
	{
		// ������ ������ ��Ÿ�Ϸ� �ǵ��� (Ÿ��Ʋ ��, �׵θ� ����)
		SetWindowLong(m_hWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);

		// ������ ������ ũ��� ��ġ�� �ǵ���
		SetWindowPos(m_hWnd, NULL, windowRect.left, windowRect.top,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		CDevice::GetInst()->ResizeResolution(m_prevWindowModeResolution.x, m_prevWindowModeResolution.y);
	}
}