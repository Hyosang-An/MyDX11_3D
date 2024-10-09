#pragma once

typedef void(*OBJECT_SAVE)(FILE*, class CGameObject*);
typedef CGameObject* (*OBJECT_LOAD)(FILE*);

class CEngine :
    public CSingleton<CEngine>
{
private:
    friend class CSingleton<CEngine>;
    CEngine();
    ~CEngine();

private:
    HWND			m_hWnd;
    POINT			m_ptResolution;
    FMOD::System*   m_FMODSystem;  // FMOD 관리자 클래스

public:
    HWND GetMainWnd() { return m_hWnd; }

    int Init(HWND _wnd, POINT _ptResolution, OBJECT_SAVE _SaveFunc, OBJECT_LOAD _LoadFunc);
    void Progress();
    void ChangeWindowScale(UINT _width, UINT _height);
    FMOD::System* GetFMODSystem() { return m_FMODSystem; }
};

