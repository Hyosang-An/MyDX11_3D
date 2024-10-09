#include "pch.h"
#include "EditorUI.h"

#include "ImGui/imgui.h"

UINT EditorUI::m_GlobalID = 0;

EditorUI::EditorUI()
	: m_ID(m_GlobalID++)
{

}

EditorUI::~EditorUI()
{
	Delete_Vec(m_vecChildUI);
}

void EditorUI::Tick()
{
	if (!m_Active)
		return;

	bool bActive = m_Active;

	int flags = 0;
	if (m_HorizontalScrollbar)
		flags |= ImGuiWindowFlags_HorizontalScrollbar;
	if (m_UseMenuBar)
		flags |= ImGuiWindowFlags_MenuBar;
	if (!m_Move)
		flags |= ImGuiWindowFlags_NoMove;


	// â�� �ּ� ũ�� �� �ִ� ũ�⸦ ����
	if (m_isSizeConstrained)
		ImGui::SetNextWindowSizeConstraints(m_minSize, m_maxSize);
	

	// �ֻ��� �θ� UI �� ���
	if (nullptr == m_Parent)
	{
		// Modaless
		if (false == m_Modal)
		{			
			ImGui::Begin(m_FullName.c_str(), &bActive, flags);

			if (m_Active != bActive)
			{
				SetActive(bActive);
			}

			Update();

			for (size_t i = 0; i < m_vecChildUI.size(); ++i)
			{
				m_vecChildUI[i]->Tick();

				if (m_vecChildUI[i]->m_ChildBorder && i == m_vecChildUI.size() - 1)
					ImGui::Separator();
			}

			ImGui::End();
		}


		// Modal
		else
		{
			
			ImGui::OpenPopup(m_FullName.c_str());

			if (ImGui::BeginPopupModal(m_FullName.c_str(), &bActive))
			{
				Update();

				for (size_t i = 0; i < m_vecChildUI.size(); ++i)
				{
					m_vecChildUI[i]->Tick();

					if (m_vecChildUI[i]->m_ChildBorder && i == m_vecChildUI.size() - 1)
						ImGui::Separator();
				}

				ImGui::EndPopup();
			}
			else
			{
				if (m_Active != bActive)
				{
					SetActive(bActive);
				}
			}
		}
	}

	// �ڽ� Ÿ�� UI �� ���
	else
	{
		if (m_ChildBorder)
		{
			ImGui::Separator();
		}

		ImGui::BeginChild(m_FullName.c_str(), m_ChildSize);

		Update();

		for (size_t i = 0; i < m_vecChildUI.size(); ++i)
		{
			//if (m_vecChildUI[i]->m_ChildBorder)
			//	ImGui::Separator();

			m_vecChildUI[i]->Tick();

			// ������ �ڽ� UI ���м� �ݾ��ֱ�
			if (m_vecChildUI[i]->m_ChildBorder && i == m_vecChildUI.size() - 1)
				ImGui::Separator();
		}

		ImGui::EndChild();
	}
}

void EditorUI::AddChild(EditorUI* _UI)
{
	_UI->m_Parent = this;
	m_vecChildUI.push_back(_UI);
}

void EditorUI::SetName(const string& _Name)
{
	m_Name = _Name;

	//// ID ���ڿ� ����
	//m_FullName = m_Name;
	//char szNum[50] = {};

	//// ���� ����
	//_itoa_s(m_ID, szNum, 10);

	// ## �ڷδ� UIâ�� ǥ�� �ȵ�
	m_FullName = m_Name + "##" + std::to_string(m_ID);
}

void EditorUI::SetActive(bool _Active)
{
	if (m_Active == _Active)
		return;

	m_Active = _Active;

	if (m_Active)
		Activate();
	else
		Deactivate();
}

void EditorUI::SetFocus()
{
	ImGui::SetWindowFocus(m_FullName.c_str());
}
