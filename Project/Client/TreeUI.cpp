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

	// openNodeNames에 해당 노드가 있는 경우
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


	// NameOnly (폴더 경로 및 확장자를 뺀 파일 이름만 노출)
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
		// 열려있으면서 자식이 있는 경우 openNodeNames에 추가
		if (!m_vecChildNode.empty())
			m_OwnerTree->GetOpenNodeNames().insert(m_Name);

		// 우클릭 팝업메뉴
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			m_OwnerTree->PopupMenu(this);
			ImGui::EndPopup();
		}

		// 해당 노드가 클릭된 경우 (왼쪽 버튼이 눌린 순간)
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_OwnerTree->SetClickedNode(this);
		}

		// 해당 노드위에서 마우스 왼쪽 버튼이 떼진 순간
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			// 해당 노드가 클릭 된 노드인 경우
			if (this == m_OwnerTree->GetClickedNode())
				m_OwnerTree->SetSelectedNode(this);
			else
				m_OwnerTree->SetClickedNode(nullptr);
		}

		// Drag, Drop 체크는 하위노드 Update 전에 처리해야 한다!
		// Drag 체크	
		DragCheck();
		// Drop 체크
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

	// 트리 노드가 닫혀있을 때
	else
	{
		// 닫혀있는 경우 openNodeNames에서 제거
		auto &openNodeNames = m_OwnerTree->GetOpenNodeNames();
		if (openNodeNames.find(m_Name) != openNodeNames.end())
			openNodeNames.erase(m_Name);

		// Drag 체크	
		DragCheck();
		// Drop 체크
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

			// 해당 트리 노드의 주소값을 전달
			// 포인터의 주소를 페이로드에 전달 (이중 포인터 사용해야함 / 해당 포인터가 가리키는 메모리에서 sizeof(TreeNode*)만큼 복사하여 페이로드로 저장) 
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
	// 모든 노드 순회 완료

	// 마우스 왼쪽 버튼을 뗏을 때
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		// 현재 창 위에서 어떤 노드를 드래그한 상태인데 현재 창의 노드가 아닌 부분에서 마우스를 뗀 경우 SelfDragDrop
		if (ImGui::IsWindowHovered() && m_DragedNode && !m_DroppedNode)
		{
			if (m_OwnerUI && m_SelfDragDropFunc)
			{
				(m_OwnerUI->*m_SelfDragDropFunc)((DWORD_PTR)m_DragedNode, 0);
			}
		}

		//선택된 m_DroppedNode, m_DragedNode 해제
		m_DroppedNode = m_DragedNode = nullptr;
	}
}

TreeNode* TreeUI::AddNode(TreeNode* _ParentNode, const string& _Name, DWORD_PTR _Data)
{
	// _Data 기본값 0
	
	// 노드 생성 및 이름, ID 세팅
	TreeNode* pNode = new TreeNode(m_NodeID++);
	pNode->m_OwnerTree = this;
	pNode->SetName(_Name);
	pNode->m_Data = _Data;

	// 부모가 지정되지 않으면 노드를 루트로 삼겠다.
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
	// 기존에 선택상태였던 노드를 선택상태 해제하고
	if (nullptr != m_SelectedNode)
		m_SelectedNode->m_Selected = false;

	// 새로운 노드를 선택된 노드로 갱신
	m_SelectedNode = _Node;

	// 새로운 노드를 OwnerUI에서 선택된 상태로 만들어줌
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
	// 현재 트리에서 Drag 된 노드가 없는 경우 ( 외부 데이터가 트리로 드랍된 경우 )
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

	// Self Drag Drop 된 상황
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
