#pragma once

class CLevel;
class CGameObject;

class CLevelMgr :
	public CSingleton<CLevelMgr>
{
	friend CSingleton<CLevelMgr>;
	friend class CTaskMgr;

private:
	CLevelMgr();
	~CLevelMgr();

private:
	CLevel*		m_CurLevel;
	bool		m_LevelChanged = false;

public:
	CLevel* GetCurrentLevel() { return m_CurLevel; }
	CGameObject* FindObjectByName(const wstring& _Name);

	void LevelChanged();
	bool IsLevelChanged() { return m_LevelChanged; }

private:
	bool ChangeLevel(CLevel* _NextLevel);

public:
	void Init();
	void Progress();
};

