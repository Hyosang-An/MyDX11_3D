#include "pch.h"
#include "Outliner.h"

#include "TreeUI.h"
#include "Inspector.h"
#include "CEditorMgr.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>

Outliner::Outliner()
{
	m_Tree = new TreeUI;
	m_Tree->SetName("OutlinerTree");
	m_Tree->SetOwnerUI(this);
	AddChild(m_Tree);

	// Ʈ�� �ɼ� ����
	// ��Ʈ ������ �ʱ�
	m_Tree->ShowRoot(false);

	// Selected Delegate ���
	m_Tree->AddSelectDelegate((DELEGATE_1)&Outliner::GameObjectSelected);

	// Drag, Drop On
	m_Tree->UseDrag(true);
	m_Tree->UseDrop(true);

	// Self DragDrop Delegate ���
	m_Tree->AddSelfDragDropDelegate((DELEGATE_2)&Outliner::GameObjectAddChild);

	// �ܺ� ��� Delegate ���
	m_Tree->AddDroppedFromOuterDelegate((DELEGATE_2)&Outliner::DroppedFromOuter);
	m_Tree->SetDropPayLoadName("ContentTree"); // ��� ���� �ܺ� TreeUI

	// TreeNode Popup Delegate ���
	m_Tree->AddPopUpDelegate((DELEGATE_1)&Outliner::PopUpMenu);

	// Asset ���¸� Content �� TreeUI �� �ݿ�
	RenewLevel();
}

Outliner::~Outliner()
{

}

void Outliner::Update()
{
	// �ڽ� Ʈ����常 Update�ϸ� ��. �ڱ� �ڽ��� ���� ���� �� ����.

	if (CLevelMgr::GetInst()->IsLevelChanged())
		RenewLevel();
}

void Outliner::RenewLevel()
{
	// ��� ���� ����
	m_Tree->Clear();

	// ��Ʈ��� ����
	TreeNode* pRootNode = m_Tree->AddNode(nullptr, "Root", 0);


	CLevel* pCurrentLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (nullptr == pCurrentLevel)
		return;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurrentLevel->GetLayer(i);
		const vector<CGameObject*>& vecParentObjects = pLayer->GetParentObjects();

		for (size_t i = 0; i < vecParentObjects.size(); ++i)
		{
			// ������Ʈ�� ����ִ� �͸�
			if (!vecParentObjects[i]->IsDead())
				AddGameObject(pRootNode, vecParentObjects[i]);
		}
	}
}

void Outliner::AddGameObject(TreeNode* _ParentNode, CGameObject* _Object)
{
	string ObjectName = string(_Object->GetName().begin(), _Object->GetName().end());

	TreeNode* pObjectNode = m_Tree->AddNode(_ParentNode, ObjectName.c_str(), (DWORD_PTR)_Object);

	const vector<CGameObject*>& vecChild = _Object->GetChildren();

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(pObjectNode, vecChild[i]);
	}
}

void Outliner::GameObjectSelected(DWORD_PTR _Param)
{
	TreeNode* pSelectedNode = (TreeNode*)_Param;

	CGameObject* pObject = (CGameObject*)pSelectedNode->GetData();

	Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
	pInspector->SetTargetObject(pObject);
}

void Outliner::GameObjectAddChild(DWORD_PTR _Param1, DWORD_PTR _Param2)
{
	TreeNode* pDragNode = (TreeNode*)_Param1;
	TreeNode* pDropNode = (TreeNode*)_Param2;

	CGameObject* pDragObject = (CGameObject*)pDragNode->GetData();
	CGameObject* pDropObject = nullptr;

	// Drag ������Ʈ�� Drop ������Ʈ�� �ڽ����� �־��ش�.
	if (pDropNode)
	{
		pDropObject = (CGameObject*)pDropNode->GetData();

		// �ڽ����� �� ������Ʈ�� �θ�(����) �� �ϳ����ٸ� �����Ѵ�.
		if (pDropObject->IsAncestor(pDragObject))
			return;

		pDropObject->AddChild(pDragObject);
	}

	// Drop �������� ���� ������, Drag ������Ʈ�� �ֻ��� �θ�� ������ش�.
	else
	{
		// �̹� �ֻ��� �θ��� ���
		if (!pDragObject->GetParent())
			return;

		// �θ������Ʈ�� ������ ����
		pDragObject->DeregisterThisChildFromParent();

		// ���� �Ҽ� ���̾ �ֻ��� �θ�μ� ���� �Ѵ�.
		CLevelMgr::GetInst()->GetCurrentLevel()->RegisterAsParent(pDragObject->GetLayerIdx(), pDragObject);
	}
}

void Outliner::DroppedFromOuter(DWORD_PTR _OuterData, DWORD_PTR _DropNode)
{
	TreeNode* ContentNode = *((TreeNode**)_OuterData);
	TreeNode* DropNode = (TreeNode*)_DropNode;

	// TODO
}

void Outliner::PopUpMenu(DWORD_PTR _Param)
{
	TreeNode* pTargetNode = (TreeNode*)_Param;


	if (ImGui::MenuItem("Option 1"))
	{
		// Option 1 ���� �� ������ �ڵ�
		int a = 0;
	}
	if (ImGui::MenuItem("Option 2"))
	{
		// Option 2 ���� �� ������ �ڵ�
	}
	if (ImGui::MenuItem("Option 3"))
	{
		// Option 3 ���� �� ������ �ڵ�
	}

}

