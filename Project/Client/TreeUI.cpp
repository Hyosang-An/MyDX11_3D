#include "pch.h"
#include "TreeUI.h"

// ========
// TreeNode
// ========
TreeNode::TreeNode(UINT _ID)
	: m_OwnerTree(nullptr)
	, m_ParentNode(nullptr)
	, m_ID(_ID)
	, m_Data(0)
	, m_Frame(false)
	, m_Selected(false)
{
}

TreeNode::~TreeNode()
{
	Delete_Vec(m_vecChildNode);
}

void TreeNode::Update()
{
	UINT Flag = ImGuiTreeNodeFlags_OpenOnDoubleClick
			  | ImGuiTreeNodeFlags_OpenOnArrow
			  | ImGuiTreeNodeFlags_SpanAvailWidth;

	// openNodeNames�� �ش� ��尡 �ִ� ���
	if (m_OwnerTree->GetOpenNodeNames().find(m_Name) != m_OwnerTree->GetOpenNodeNames().end())
		Flag |= ImGuiTreeNodeFlags_DefaultOpen;

	if (m_Frame)
		Flag |= ImGuiTreeNodeFlags_Framed;

	if (m_Selected)
		Flag |= ImGuiTreeNodeFlags_Selected;

	//char Name[255] = {};
	string strName;

	if (m_vecChildNode.empty())
	{
		Flag |= ImGuiTreeNodeFlags_Leaf;
		//sprintf_s(Name, 255, "   %s##%d", m_Name.c_str(), m_ID);

		if (m_Frame)
			strName = "   " + m_Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
		else
			strName = m_Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
	}
	else
	{
		//sprintf_s(Name, 255, "%s##%d", m_Name.c_str(), m_ID);

		strName = m_Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
	}


	// NameOnly (���� ��� �� Ȯ���ڸ� �� ���� �̸��� ����)
	if (m_OwnerTree->IsShowNameOnly())
	{
		path Path = strName;
		strName = Path.stem().string();
	}

	bool bColorPushed = false;

	if (m_Folder)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0.4f, 1.0f)); // Gray color for text
		bColorPushed = true;
	}


	if (ImGui::TreeNodeEx(strName.c_str(), Flag))
	{
		// ���������鼭 �ڽ��� �ִ� ��� openNodeNames�� �߰�
		if (!m_vecChildNode.empty())
			m_OwnerTree->GetOpenNodeNames().insert(m_Name);

		// ��Ŭ�� �˾��޴�
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			m_OwnerTree->PopupMenu(this);
			ImGui::EndPopup();
		}

		// �ش� ��尡 Ŭ���� ��� (���� ��ư�� ���� ����)
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_OwnerTree->SetClickedNode(this);
		}

		// �ش� ��������� ���콺 ���� ��ư�� ���� ����
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			// �ش� ��尡 Ŭ�� �� ����� ���
			if (this == m_OwnerTree->GetClickedNode())
				m_OwnerTree->SetSelectedNode(this);
			else
				m_OwnerTree->SetClickedNode(nullptr);
		}

		// Drag, Drop üũ�� ������� Update ���� ó���ؾ� �Ѵ�!
		// Drag üũ	
		DragCheck();
		// Drop üũ
		DropCheck();

		if (bColorPushed)
		{
			ImGui::PopStyleColor(1);
			bColorPushed = false;
		}

		for (size_t i = 0; i < m_vecChildNode.size(); ++i)
		{
			m_vecChildNode[i]->Update();
		}

		ImGui::TreePop();
	}

	// Ʈ�� ��尡 �������� ��
	else
	{
		// �����ִ� ��� openNodeNames���� ����
		auto &openNodeNames = m_OwnerTree->GetOpenNodeNames();
		if (openNodeNames.find(m_Name) != openNodeNames.end())
			openNodeNames.erase(m_Name);

		// Drag üũ	
		DragCheck();
		// Drop üũ
		DropCheck();
	}

	if (bColorPushed)
		ImGui::PopStyleColor(1);
}

void TreeNode::DragCheck()
{
	if (m_OwnerTree->IsUsingDrag())
	{
		if (ImGui::BeginDragDropSource())
		{
			TreeNode* pThis = this;

			// �ش� Ʈ�� ����� �ּҰ��� ����
			// �������� �ּҸ� ���̷ε忡 ���� (���� ������ ����ؾ��� / �ش� �����Ͱ� ����Ű�� �޸𸮿��� sizeof(TreeNode*)��ŭ �����Ͽ� ���̷ε�� ����) 
			ImGui::SetDragDropPayload(m_OwnerTree->GetName().c_str(), &pThis, sizeof(TreeNode*));
			ImGui::Text(m_Name.c_str());
			m_OwnerTree->SetDragedNode(this);

			ImGui::EndDragDropSource();
		}
	}
}


void TreeNode::DropCheck()
{
	if (!m_OwnerTree->IsUsingDrop())
		return;

	if (ImGui::BeginDragDropTarget())
	{
		m_OwnerTree->SetDroppedNode(this);

		ImGui::EndDragDropTarget();
	}
}


// ======================================================================================================
// TreeUI
// ======================================================================================================
TreeUI::TreeUI()
{
}

TreeUI::~TreeUI()
{
	Clear();
}

void TreeUI::Update()
{
	if (nullptr == m_Root)
		return;

	if (m_ShowRoot)
		m_Root->Update();
	else
	{
		for (size_t i = 0; i < m_Root->m_vecChildNode.size(); ++i)
		{
			m_Root->m_vecChildNode[i]->Update();
		}
	}
	// ��� ��� ��ȸ �Ϸ�

	// ���콺 ���� ��ư�� ���� ��
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		// ���� â ������ � ��带 �巡���� �����ε� ���� â�� ��尡 �ƴ� �κп��� ���콺�� �� ��� SelfDragDrop
		if (ImGui::IsWindowHovered() && m_DragedNode && !m_DroppedNode)
		{
			if (m_OwnerUI && m_SelfDragDropFunc)
			{
				(m_OwnerUI->*m_SelfDragDropFunc)((DWORD_PTR)m_DragedNode, 0);
			}
		}

		//���õ� m_DroppedNode, m_DragedNode ����
		m_DroppedNode = m_DragedNode = nullptr;
	}
}

TreeNode* TreeUI::AddNode(TreeNode* _ParentNode, const string& _Name, DWORD_PTR _Data)
{
	// _Data �⺻�� 0
	
	// ��� ���� �� �̸�, ID ����
	TreeNode* pNode = new TreeNode(m_NodeID++);
	pNode->m_OwnerTree = this;
	pNode->SetName(_Name);
	pNode->m_Data = _Data;

	// �θ� �������� ������ ��带 ��Ʈ�� ��ڴ�.
	if (nullptr == _ParentNode)
	{
		assert(!m_Root);

		m_Root = pNode;
	}
	else
	{
		_ParentNode->AddChildNode(pNode);
	}

	return pNode;
}

void TreeUI::SetClickedNode(TreeNode* _node)
{
	m_ClickedNode = _node;
}

void TreeUI::SetSelectedNode(TreeNode* _Node)
{
	// ������ ���û��¿��� ��带 ���û��� �����ϰ�
	if (nullptr != m_SelectedNode)
		m_SelectedNode->m_Selected = false;

	// ���ο� ��带 ���õ� ���� ����
	m_SelectedNode = _Node;

	// ���ο� ��带 OwnerUI���� ���õ� ���·� �������
	if (nullptr != m_SelectedNode)
	{
		m_SelectedNode->m_Selected = true;

		if (m_OwnerUI && m_SelectFunc)
		{
			(m_OwnerUI->*m_SelectFunc)((DWORD_PTR)m_SelectedNode);
		}
	}
}

void TreeUI::SetDragedNode(TreeNode* _Node)
{
	m_DragedNode = _Node;
}

void TreeUI::SetDroppedNode(TreeNode* _Node)
{
	// ���� Ʈ������ Drag �� ��尡 ���� ��� ( �ܺ� �����Ͱ� Ʈ���� ����� ��� )
	if (nullptr == m_DragedNode)
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_DropPayLoadName.c_str());
		if (payload)
		{
			m_DroppedNode = _Node;

			if (m_OwnerUI && m_DroppedFromOuterFunc)
				(m_OwnerUI->*m_DroppedFromOuterFunc)((DWORD_PTR)payload->Data, (DWORD_PTR)m_DroppedNode);
		}
	}

	// Self Drag Drop �� ��Ȳ
	else
	{
		assert(m_DragedNode->m_OwnerTree == this);

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetName().c_str());
		if (payload)
		{
			m_DroppedNode = _Node;

			if (m_OwnerUI && m_SelfDragDropFunc)
				(m_OwnerUI->*m_SelfDragDropFunc)((DWORD_PTR)m_DragedNode, (DWORD_PTR)m_DroppedNode);
		}
	}
}

void TreeUI::PopupMenu(TreeNode* _node)
{
	if (m_OwnerUI && m_PopUpFunc)
	{
		(m_OwnerUI->*m_PopUpFunc)((DWORD_PTR)_node);
	}

	SetSelectedNode(_node);
}

void TreeUI::Clear()
{
	if (nullptr != m_Root)
	{
		delete m_Root;
		m_Root = nullptr;
	}
}
