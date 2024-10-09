#include "pch.h"
#include "CKeyMgr.h"

#include "CEngine.h"

CKeyMgr::CKeyMgr()
{
}

CKeyMgr::~CKeyMgr()
{
}

void CKeyMgr::Init()
{
	for (int i = 0; i < (UINT)KEY::END; i++)
	{
		tKeyInfo KeyInfo{};
		KeyInfo.eKey = (KEY)i;
		KeyInfo.eKeyState = KEY_STATE::NONE;
		KeyInfo.bPressed = false;

		m_vecKeyInfo.push_back(KeyInfo);
	}
}

void CKeyMgr::Tick()
{
	// MainWindow가 포커싱 상태일때만 키 입력 적용
	if (CEngine::GetInst()->GetMainWnd() == GetFocus())
	{
		for (UINT i = 0; i < (UINT)KEY::END; ++i)
		{
			auto& KeyInfo = m_vecKeyInfo[i];
			// 현재 키가 눌렸는지 체크
			if (0x8001 & GetAsyncKeyState(m_RealKey[i]))
			{
				//이전에도 눌려 있었는지
				if (KeyInfo.bPressed)
				{
					KeyInfo.eKeyState = KEY_STATE::PRESSED;
				}
				else
				{
					KeyInfo.eKeyState = KEY_STATE::JUST_PRESSED;
					KeyInfo.bPressed = true;
				}
			}

			else
			{
				//이전에도 눌려 있었는지
				if (KeyInfo.bPressed)
				{
					KeyInfo.eKeyState = KEY_STATE::RELEASED;
					KeyInfo.bPressed = false;
				}
				else
				{
					KeyInfo.eKeyState = KEY_STATE::NONE;
				}
			}
		}

		// 마우스 좌표 계산
		if (m_MouseCapture)
		{
			POINT ptMousePos = { };
			GetCursorPos(&ptMousePos);
			ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
			m_MousePosInClient = Vec2((float)ptMousePos.x, (float)ptMousePos.y);

			m_MouseDragDelta = m_MousePosInClient - m_CapturePos;

			POINT ptCapturePos = { (int)m_CapturePos.x, (int)m_CapturePos.y };
			ClientToScreen(CEngine::GetInst()->GetMainWnd(), &ptCapturePos);
			SetCursorPos(ptCapturePos.x, ptCapturePos.y);
		}
		else
		{
			m_PrevMousePos = m_MousePosInClient;
			POINT ptMousePos = { };
			GetCursorPos(&ptMousePos);
			ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
			m_MousePosInClient = Vec2((float)ptMousePos.x, (float)ptMousePos.y);
			m_MouseDragDelta = m_MousePosInClient - m_PrevMousePos;
		}
	}

	// 윈도우 포커싱이 해제되면 눌려있던 키 해제(Release)
	else
	{
		for (UINT i = 0; i < (UINT)KEY::END; i++)
		{
			auto& KeyInfo = m_vecKeyInfo[i];

			if (KeyInfo.eKeyState == KEY_STATE::NONE)
				continue;

			if (KeyInfo.eKeyState == KEY_STATE::JUST_PRESSED || KeyInfo.eKeyState == KEY_STATE::PRESSED)
				KeyInfo.eKeyState = KEY_STATE::RELEASED;

			else
				KeyInfo.eKeyState = KEY_STATE::NONE;

			KeyInfo.bPressed = false;
		}
	}

}
