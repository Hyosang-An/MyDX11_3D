#include "pch.h"
#include "Content.h"

#include "CEditorMgr.h"
#include "Inspector.h"
#include "TreeUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>
#include <Engine/CTaskMgr.h>
#include <sstream>  // stringstream�� ����ϱ� ���� ��� ���� �߰�

Content::Content()
{
	m_Tree = new TreeUI;
	m_Tree->SetName("ContentTree");
	m_Tree->SetOwnerUI(this);
	AddChild(m_Tree);

	// Ʈ�� �ɼ� ����
	m_Tree->ShowRoot(false);	// ��Ʈ ������ �ʱ�
	m_Tree->UseDrag(true);		// Tree Drag ��� ON
	m_Tree->ShowNameOnly(true); // ����� �̸����� ��� �� Ȯ���ڴ� �����ϰ� �����̸��� ���̱�

	m_Tree->AddSelectDelegate((DELEGATE_1)&Content::AssetSelected);
	m_Tree->AddPopUpDelegate((DELEGATE_1)&Content::PopupMenu);
	// Asset ���¸� Content �� TreeUI �� �ݿ�
	RenewContent();
}

Content::~Content()
{
}

void Content::Init()
{
	Reload();
}

void Content::Update()
{
	// AssetMgr �� �������� �����, Ʈ�� ������ �籸���Ѵ�.
	if (CAssetMgr::GetInst()->IsChanged())
	{
		RenewContent();
	}
}

void Content::RenewContent()
{
	//// Ʈ���� ������ ���� ����
	//m_Tree->Clear();

	//// �θ��带 �������� ���� == ��Ʈ��� �Է�
	//TreeNode* pRoot = m_Tree->AddNode(nullptr, "Root");

	//for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	//{
	//	TreeNode* pNode = m_Tree->AddNode(pRoot, ToString((ASSET_TYPE)i));
	//	pNode->SetFrame(true);

	//	const map<wstring, Ptr<CAsset>>& assetMap = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);

	//	for (const auto& pair : assetMap)
	//	{
	//		// Asset�� �ּҰ��� Data�� �־��ش�. �̶�, pair�� const & �̹Ƿ�, pair.second.Get() �� const �Լ����� �Ѵ�.
	//		m_Tree->AddNode(pNode, string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second.Get());
	//	}
	//}


	// Ʈ���� ������ ���� ����
	m_Tree->Clear();

	// �θ��带 �������� ���� == ��Ʈ��� �Է�
	TreeNode* pRoot = m_Tree->AddNode(nullptr, "Root");

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		TreeNode* pNode = m_Tree->AddNode(pRoot, ToString((ASSET_TYPE)i));
		pNode->SetFrame(true);

		const std::map<std::wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			// ��θ� �м��Ͽ� ������ ������ ����
			std::string fullPath(pair.first.begin(), pair.first.end());
			std::stringstream ss(fullPath);
			std::string segment;
			TreeNode* currentParent = pNode;

			// ����� �� �κп� ���� ��带 �߰�
			while (std::getline(ss, segment, '\\'))
			{
				// ���� ������ ���� �κ��� ���ٸ�, �� segment�� ���� �̸����� �ǹ�
				if (ss.eof())
				{
					m_Tree->AddNode(currentParent, fullPath, (DWORD_PTR)pair.second.Get());
					break;
				}

				// ���� ��� ����

				auto& vecChildNode = currentParent->GetVecChildNode();
				auto it = std::find_if(vecChildNode.begin(), vecChildNode.end(),
					[&segment](TreeNode* node) { return node->GetName() == segment; });

				if (it == vecChildNode.end())
				{
					TreeNode* newNode = m_Tree->AddNode(currentParent, segment);
					newNode->SetFolder(true);
					currentParent = newNode;
				}
				else
				{
					currentParent = *it;
				}
			}
		}
	}
}

void Content::AssetSelected(DWORD_PTR _Param)
{
	// _Param�� SelectedNode�̴�.
	TreeNode* pNode = (TreeNode*)_Param;
	if (pNode->IsFrame())
		return;

	Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();

	Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
	pInspector->SetTargetAsset(pAsset);
}

void Content::Reload()
{
	CAssetMgr::GetInst()->GetContentFiles();

	//==============================================

	//// Content ������ �ִ� �������ϵ��� ��θ� ���� �˾Ƴ���.
	//wstring ContentsPath = CPathMgr::GetInst()->GetContentPath();
	//FindAssetName(ContentsPath, L"*.*");

	//// �˾Ƴ� ���� ���ϵ��� ��θ� ���ؼ� Asset ���� ����θ� Key������ AssetMgr �� �ε��Ѵ�.
	//for (size_t i = 0; i < m_vecAssetRelativePath.size(); ++i)
	//{
	//	LoadAsset(m_vecAssetRelativePath[i]);
	//}


	//// ���� �Ŵ������� �ε��Ǿ�������, content �������� ���� ������ AssetMgr ���� �����ϱ�
	//// �ε��� ���¿� �ش��ϴ� ���� ������ Content ������ �ִ��� Exist üũ
	//wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

	//for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	//{
	//	const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);
	//	for (const auto &pair : mapAsset)
	//	{
	//		// �������� ������ ������ ���� ���������� ���⶧���� �Ѿ��.
	//		if (pair.second->IsEngineAsset())
	//			continue;


	//		wstring strRelativePath = pair.second->GetRelativePath();

	//		if (strRelativePath == L"" || false == std::filesystem::exists(strContentPath + strRelativePath))
	//		{
	//			// AssetMgr������ ��� �ִ� ���
	//			if (pair.second->GetRefCount() <= 1)
	//			{
	//				// // AssetMgr���� ���� ��û
	//				tTask deleteAssetTask{ TASK_TYPE::DEL_ASSET, (DWORD_PTR)pair.second.Get() };
	//				CTaskMgr::GetInst()->AddTask(deleteAssetTask);
	//			}

	//			// �ٸ� �������� �����ϰ� �ִ� ���
	//			else
	//			{
	//				wstring msg = pair.second->GetName() + L"�ٸ� ������ �����ǰ� ���� �� �ֽ��ϴ�.";
	//				int ret = MessageBox(nullptr, msg.c_str(), L"���� ���� ���", MB_YESNO);
	//				if (ret == IDYES)
	//				{
	//					
	//					CTaskMgr::GetInst()->AddTask(tTask{ TASK_TYPE::DEL_ASSET, (DWORD_PTR)pair.second.Get(), });
	//				}
	//			}
	//		}
	//	}
	//}
}

//void Content::FindAssetName(const wstring& _FolderPath, const wstring& _Filter)
//{
//	WIN32_FIND_DATA tFindData = {};
//
//	// ��ο� �´� ���� �� ������ Ž���� �� �ִ� Ŀ�� ������Ʈ ����
//	wstring strFindPath = _FolderPath + _Filter;
//	HANDLE hFinder = FindFirstFile(strFindPath.c_str(), &tFindData);
//	if (hFinder == INVALID_HANDLE_VALUE)
//	{
//		assert(!"�ش� ��θ� ã�� �� �����ϴ�");
//		return;
//	}
//
//	do
//	{
//		wstring strFindName = tFindData.cFileName;
//
//		if (tFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//		{
//			// ������ ��� ȣ��
//			if (strFindName != L"." && strFindName != L"..")
//			{
//				FindAssetName(_FolderPath + strFindName + L"\\", _Filter);
//			}
//		}
//
//		// �����̸� ��� �߰�
//		else
//		{
//			wstring RelativePath = CPathMgr::GetInst()->GetRelativePath(_FolderPath + strFindName);
//			m_vecAssetRelativePath.push_back(RelativePath);
//		}
//	} while (FindNextFile(hFinder, &tFindData));
//
//	FindClose(hFinder);
//}
//
//void Content::LoadAsset(const path& _Path)
//{
//	// ����θ� Ű ������ AssetMgr�� ���
//
//	path ext = _Path.extension();
//
//	if (ext == L".mesh")
//		CAssetMgr::GetInst()->Load<CMesh>(_Path, _Path);
//	//else if (ext == L".mdat")
//		//CAssetMgr::GetInst()->Load<CMeshData>(_Path, _Path);
//	else if (ext == L".mtrl")
//		CAssetMgr::GetInst()->Load<CMaterial>(_Path, _Path);
//	else if (ext == L".pref")
//		CAssetMgr::GetInst()->Load<CPrefab>(_Path, _Path);
//	else if (ext == L".png" || ext == L".jpg" || ext == L".jpeg" || ext == L".bmp" || ext == L".dds" || ext == L".tga"
//		|| ext == L".PNG" || ext == L".JPG" || ext == L".JPEG" || ext == L".BMP" || ext == L".DDS" || ext == L".TGA")
//		CAssetMgr::GetInst()->Load<CTexture>(_Path, _Path);
//	else if (ext == L".mp3" || ext == L".mp4" || ext == L".ogg" || ext == L".wav" 
//		|| ext == L".MP3" || ext == L".MP4" || ext == L".OGG" || ext == L".WAV")
//		CAssetMgr::GetInst()->Load<CSound>(_Path, _Path);
//	else if (ext == L".sprite")
//		CAssetMgr::GetInst()->Load<CSprite>(_Path, _Path);
//	else if (ext == L".flip")
//		CAssetMgr::GetInst()->Load<CFlipBook>(_Path, _Path);
//}
//
//

void Content::PopupMenu(DWORD_PTR _Param)
{
	TreeNode* pTargetNode = (TreeNode*)_Param;

	Ptr<CAsset> pAsset = (CAsset*)pTargetNode->GetData();

	if (pAsset->GetAssetType() == ASSET_TYPE::PREFAB)
	{
		if (ImGui::MenuItem("Instantiate"))
		{
			Ptr<CPrefab> pPrefab = (CPrefab*)pAsset.Get();

			CGameObject* CloneObj = pPrefab->Instantiate();

			SpawnObject(CloneObj, 0);

			ImGui::CloseCurrentPopup();
		}
	}

	//if (ImGui::Button("Close"))
	//ImGui::CloseCurrentPopup();
	//ImGui::EndPopup();
}