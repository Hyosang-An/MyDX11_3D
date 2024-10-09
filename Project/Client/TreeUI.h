#pragma once
#include "EditorUI.h"

class TreeNode
{
private:
    class TreeUI*       m_OwnerTree;

    string              m_Name;
    TreeNode*           m_ParentNode;
    vector<TreeNode*>   m_vecChildNode;
    const UINT          m_ID;
    DWORD_PTR           m_Data;

    bool                m_Frame;
    bool                m_Selected;
    bool                m_Folder = false;


public:
    void SetName(const string& _Name) { m_Name = _Name; }
    const string& GetName() { return m_Name; }

    void SetFrame(bool _Frame) { m_Frame = _Frame; }
    bool IsFrame() { return m_Frame; }

    void SetFolder(bool _folder) { m_Folder = _folder; }

    DWORD_PTR GetData() { return m_Data; }

    void AddChildNode(TreeNode* _Node) { m_vecChildNode.push_back(_Node); }
    const vector<TreeNode*>& GetVecChildNode() { return m_vecChildNode; }
    void DragCheck();
    void DropCheck();

public:
    void Update();

public:
    TreeNode(UINT _ID);
    ~TreeNode();

    friend class TreeUI;
};


class TreeUI :
    public EditorUI
{
private:
    EditorUI*   m_OwnerUI = nullptr;

    TreeNode*   m_Root = nullptr;
    TreeNode*   m_ClickedNode = nullptr;
    TreeNode*   m_SelectedNode = nullptr;
    TreeNode*   m_DragedNode = nullptr;
    TreeNode*   m_DroppedNode = nullptr;

    UINT        m_NodeID = 0;
    bool        m_ShowRoot = false;
    bool        m_ShowNameOnly = false;
    bool        m_UseDrag = false;
    bool        m_UseDrop = false;

    DELEGATE_1  m_SelectFunc = nullptr;

    DELEGATE_2  m_SelfDragDropFunc = nullptr;

    string      m_DropPayLoadName;
    DELEGATE_2  m_DroppedFromOuterFunc = nullptr;

    DELEGATE_1  m_PopUpFunc = nullptr;

    set<string> m_openNodeNames;


public:
    TreeNode* AddNode(TreeNode* _Parent, const string& _Name, DWORD_PTR _Data = 0);
    void ShowRoot(bool _Show) { m_ShowRoot = _Show; }
    bool IsShowRoot() { return m_ShowRoot; }
    bool IsShowNameOnly() { return m_ShowNameOnly; }
    void ShowNameOnly(bool _NameOnly) { m_ShowNameOnly = _NameOnly; }

    TreeNode* GetClickedNode() { return m_ClickedNode; }

    void SetClickedNode(TreeNode* _node);
    void SetSelectedNode(TreeNode* _node);
    void SetDragedNode(TreeNode* _node);
    void SetDroppedNode(TreeNode* _node);
    void PopupMenu(TreeNode* _node);

    void UseDrag(bool _Drag) { m_UseDrag = _Drag; }
    void UseDrop(bool _Drop) { m_UseDrop = _Drop; }

    bool IsUsingDrag() { return m_UseDrag; }
    bool IsUsingDrop() { return m_UseDrop; }

    void SetDropPayLoadName(const string& _Name) { m_DropPayLoadName = _Name; }
    const string GetDropPayLoadName() { return m_DropPayLoadName; }

    void SetOwnerUI(EditorUI* _OwnerUI) { m_OwnerUI = _OwnerUI; }

    void AddSelectDelegate(DELEGATE_1 _Func) { m_SelectFunc = _Func; }
    void AddSelfDragDropDelegate(DELEGATE_2 _Func) { m_SelfDragDropFunc = _Func; }
    void AddDroppedFromOuterDelegate(DELEGATE_2 _Func) { m_DroppedFromOuterFunc = _Func; }
    void AddPopUpDelegate(DELEGATE_1 _FUNC) { m_PopUpFunc = _FUNC; }

    void Clear();

	set<string>& GetOpenNodeNames() { return m_openNodeNames; }

public:
    virtual void Update() override;

public:
    TreeUI();
    ~TreeUI();
};

