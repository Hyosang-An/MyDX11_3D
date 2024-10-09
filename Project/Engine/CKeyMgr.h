#pragma once

enum class KEY
{
	_0, _1, _2, _3, _4, _5, _6, _7, _8, _9,

	Q, W, E, R, T, Y, U, I, O, P,
	A, S, D, F, G, H, J, K, L,
	Z, X, C, V, B, N, M,

	LEFT, RIGHT, UP, DOWN,

	ENTER, SPACE, ALT, CTRL, LSHIFT, ESC,

	LBTN,
	RBTN,

	END
};

enum class KEY_STATE
{
	JUST_PRESSED, // 키가 막 눌린 상태
	PRESSED,      // 키가 눌려있는 상태
	RELEASED,     // 키가 해제된 상태
	NONE
};

struct tKeyInfo
{
	KEY         eKey;       // 키의 종류
	KEY_STATE   eKeyState;  // 키의 현재 상태
	bool        bPressed;   // 키가 지금 눌려있는 상태인지
};

class CKeyMgr :
	public CSingleton<CKeyMgr>
{
private:
	friend class CSingleton<CKeyMgr>;
	CKeyMgr();
	~CKeyMgr();

private:
	UINT    m_RealKey[(UINT)KEY::END] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M',

		VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,

		VK_RETURN,
		VK_SPACE,
		VK_MENU,
		VK_CONTROL,
		VK_LSHIFT,
		VK_ESCAPE,

		VK_LBUTTON,
		VK_RBUTTON
	};

	vector<tKeyInfo>    m_vecKeyInfo;

	Vec2	m_MousePosInClient;
	Vec2	m_PrevMousePos;
	Vec2	m_MouseDragDelta;

	Vec2				m_CapturePos;
	bool				m_MouseCapture;

public:
	void Init();
	void Tick();

	KEY_STATE GetKeyState(KEY _key) { return m_vecKeyInfo[(UINT)_key].eKeyState; }
	Vec2 GetMousePosInClient() { return m_MousePosInClient; }
	Vec2 GetMouseDragDelta() { return m_MouseDragDelta; }

	void MouseCapture(bool _bCapture)
	{
		m_MouseCapture = _bCapture;		

		if (_bCapture == true)
			m_CapturePos = m_MousePosInClient;
	}
};

// 키 상태 체크 관련 매크로 함수를 인라인 함수로 변환
inline bool KEY_CHECK(KEY _key, KEY_STATE _state) {
	return CKeyMgr::GetInst()->GetKeyState(_key) == _state;
}

inline bool KEY_JUST_PRESSED(KEY _key) {
	return KEY_CHECK(_key, KEY_STATE::JUST_PRESSED);
}

inline bool KEY_PRESSED(KEY _key) {
	return KEY_CHECK(_key, KEY_STATE::PRESSED);
}

inline bool KEY_RELEASED(KEY _key) {
	return KEY_CHECK(_key, KEY_STATE::RELEASED);
}

inline bool KEY_NONE(KEY _key) {
	return KEY_CHECK(_key, KEY_STATE::NONE);
}