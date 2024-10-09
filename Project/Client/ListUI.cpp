#include "pch.h"
#include "ListUI.h"

ListUI::ListUI()
	: m_SelectedIdx(-1)
	, m_UI(nullptr)
	, m_MemFunc(nullptr)
{
}

ListUI::~ListUI()
{
}

void ListUI::Update()
{
	for (size_t i = 0; i < m_vecList.size(); ++i)
	{
		char szID[255] = {};
		sprintf_s(szID, 255, "%s##%d", m_vecList[i].c_str(), (int)i); // 중복된 이름이 있는경우 구분을 위해 ##숫자 추가

		UINT Flag = ImGuiTreeNodeFlags_Leaf;

		if (i == m_SelectedIdx)
		{
			Flag |= ImGuiTreeNodeFlags_Selected;
		}

		if (ImGui::TreeNodeEx(szID, Flag)) // ImGuiTreeNodeFlags_Leaf 플래그면 무조건 반환값이 true
		{
			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_SelectedIdx = (int)i;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			m_SelectedName = m_vecList[i];

			if (m_UI && m_MemFunc)
			{
				(m_UI->*m_MemFunc)((DWORD_PTR)this);
			}

			SetActive(false);
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			SetActive(false);

		/*if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_SelectedIdx = i;
		}*/
	}
}

void ListUI::Activate()
{
	m_vecList.insert(m_vecList.begin(), 1, "None");
}

void ListUI::AddItem(const string& _item)
{
	m_vecList.push_back(_item);
}

void ListUI::AddList(const vector<string>& _list)
{
	m_vecList.insert(m_vecList.end(), _list.begin(), _list.end());
}

void ListUI::Deactivate()
{
	// Clear
	m_vecList.clear();
	m_SelectedIdx = -1;
	m_UI = nullptr;
	m_MemFunc = nullptr;
}
