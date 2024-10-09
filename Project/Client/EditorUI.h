#pragma once

#include "ImGui/imgui.h"
class EditorUI
{
protected:
	static UINT			m_GlobalID;

	string				m_Name;			// 창에 표시될 이름
	const UINT			m_ID;			// UI 고유 아이디
	string				m_FullName;		// UI 고유 아이디를 이용한 키 name
	bool				m_Active = true;

	EditorUI*			m_Parent = nullptr;
	vector<EditorUI*>	m_vecChildUI;

	ImVec2				m_ChildSize;	// 자식 창의 크기

	bool				m_Modal = false;
	bool				m_ChildBorder = false;	// 자식 구분선 표기 유무 

	bool				m_HorizontalScrollbar = false;
	bool				m_UseMenuBar = false;
	bool				m_Move = true;


	bool				m_isSizeConstrained = false;
	ImVec2				m_minSize;
	ImVec2				m_maxSize;


public:
	void AddChild(EditorUI* _UI);

	EditorUI* GetParent() { return m_Parent; }
	const vector<EditorUI*>& GetChildren() { return m_vecChildUI; }

	void SetName(const string& _Name);

	const string& GetName() { return m_Name; }

	UINT GetID() { return m_ID; }
	const string& GetFullName() { return m_FullName; }

	bool IsActive() { return m_Active; }

	void UseMenuBar(bool _Use) { m_UseMenuBar = _Use; }
	void SetMove(bool _Move) { m_Move = _Move; }

	void SetChildBorder(bool _Set) { m_ChildBorder = _Set; }
	void SetChildSize(ImVec2 _Size) { m_ChildSize = _Size; }

	void SetActive(bool _Active);

	void SetModal(bool _Modal) { m_Modal = _Modal; }
	bool IsModal() { return m_Modal; }

	void SetFocus();

private:
	virtual void Activate() {}
	virtual void Deactivate() {}

public:
	virtual void Init() {}
	virtual void Tick();
	virtual void Update() = 0;

public:
	EditorUI();
	virtual ~EditorUI();
};

typedef void(EditorUI::* DELEGATE_0)(void);
typedef void(EditorUI::* DELEGATE_1)(DWORD_PTR);
typedef void(EditorUI::* DELEGATE_2)(DWORD_PTR, DWORD_PTR);
typedef void(EditorUI::* DELEGATE_3)(DWORD_PTR, DWORD_PTR, DWORD_PTR);

