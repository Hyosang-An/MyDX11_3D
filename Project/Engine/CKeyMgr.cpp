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

	m_PrevMousePos = m_MousePosInClient;
	POINT ptMousePos = { };
	GetCursorPos(&ptMousePos);
	ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
	m_MousePosInClient = m_PrevMousePos = Vec2((float)ptMousePos.x, (float)ptMousePos.y);
}

void CKeyMgr::Tick()
{
	// MainWindow�� ��Ŀ�� �����϶��� Ű �Է� ����
	if (CEngine::GetInst()->GetMainWnd() == GetFocus())
	{
		for (UINT i = 0; i < (UINT)KEY::END; ++i)
		{
			auto& KeyInfo = m_vecKeyInfo[i];
			// ���� Ű�� ���ȴ��� üũ
			if (0x8001 & GetAsyncKeyState(m_RealKey[i]))
			{
				//�������� ���� �־�����
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
				//�������� ���� �־�����
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

		// ���콺 ��ǥ ���
		if (m_MouseCapture)
		{
			POINT ptMousePos = { };
			GetCursorPos(&ptMousePos);
			ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
			m_MousePosInClient = Vec2((float)ptMousePos.x, (float)ptMousePos.y);

			m_MousePosDelta = m_MousePosInClient - m_CapturePos;

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
			m_MousePosDelta = m_MousePosInClient - m_PrevMousePos;
		}
	}

	// ������ ��Ŀ���� �����Ǹ� �����ִ� Ű ����(Release)
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
