#include "pch.h"
#include "Content.h"

#include "CEditorMgr.h"
#include "Inspector.h"
#include "TreeUI.h"

#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>
#include <Engine/CTaskMgr.h>
#include <sstream>  // stringstream을 사용하기 위한 헤더 파일 추가

Content::Content()
{
	m_Tree = new TreeUI;
	m_Tree->SetName("ContentTree");
	m_Tree->SetOwnerUI(this);
	AddChild(m_Tree);

	// 트리 옵션 세팅
	m_Tree->ShowRoot(false);	// 루트 보이지 않기
	m_Tree->UseDrag(true);		// Tree Drag 기능 ON
	m_Tree->ShowNameOnly(true); // 노드의 이름에서 경로 및 확장자는 제외하고 파일이름만 보이기

	m_Tree->AddSelectDelegate((DELEGATE_1)&Content::AssetSelected);
	m_Tree->AddPopUpDelegate((DELEGATE_1)&Content::PopupMenu);
	// Asset 상태를 Content 의 TreeUI 에 반영
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
	// AssetMgr 에 변경점이 생기면, 트리 내용을 재구성한다.
	if (CAssetMgr::GetInst()->IsChanged())
	{
		RenewContent();
	}
}

void Content::RenewContent()
{
	//// 트리의 내용을 전부 제거
	//m_Tree->Clear();

	//// 부모노드를 지정하지 않음 == 루트노드 입력
	//TreeNode* pRoot = m_Tree->AddNode(nullptr, "Root");

	//for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	//{
	//	TreeNode* pNode = m_Tree->AddNode(pRoot, ToString((ASSET_TYPE)i));
	//	pNode->SetFrame(true);

	//	const map<wstring, Ptr<CAsset>>& assetMap = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);

	//	for (const auto& pair : assetMap)
	//	{
	//		// Asset의 주소값을 Data로 넣어준다. 이때, pair가 const & 이므로, pair.second.Get() 도 const 함수여야 한다.
	//		m_Tree->AddNode(pNode, string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second.Get());
	//	}
	//}


	// 트리의 내용을 전부 제거
	m_Tree->Clear();

	// 부모노드를 지정하지 않음 == 루트노드 입력
	TreeNode* pRoot = m_Tree->AddNode(nullptr, "Root");

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		TreeNode* pNode = m_Tree->AddNode(pRoot, ToString((ASSET_TYPE)i));
		pNode->SetFrame(true);

		const std::map<std::wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			// 경로를 분석하여 폴더와 파일을 구분
			std::string fullPath(pair.first.begin(), pair.first.end());
			std::stringstream ss(fullPath);
			std::string segment;
			TreeNode* currentParent = pNode;

			// 경로의 각 부분에 대해 노드를 추가
			while (std::getline(ss, segment, '\\'))
			{
				// 만약 다음에 읽을 부분이 없다면, 이 segment가 파일 이름임을 의미
				if (ss.eof())
				{
					m_Tree->AddNode(currentParent, fullPath, (DWORD_PTR)pair.second.Get());
					break;
				}

				// 폴더 노드 생성

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
	// _Param은 SelectedNode이다.
	TreeNode* pNode = (TreeNode*)_Param;
	if (pNode->IsFrame())
		return;

	Ptr<CAsset> pAsset = (CAsset*)pNode->GetData();

	Inspector* pInspector = (Inspector*)CEditorMgr::GetInst()->FindEditorUI("Inspector");
	pInspector->SetTargetAsset(pAsset);
}

void Content::Reload()
{
	// Content 폴더에 있는 에셋파일들의 경로를 전부 알아낸다.
	wstring ContentsPath = CPathMgr::GetInst()->GetContentPath();
	FindAssetName(ContentsPath, L"*.*");

	// 알아낸 에셋 파일들의 경로를 통해서 Asset 들을 상대경로를 Key값으로 AssetMgr 에 로딩한다.
	for (size_t i = 0; i < m_vecAssetRelativePath.size(); ++i)
	{
		LoadAsset(m_vecAssetRelativePath[i]);
	}


	// 에셋 매니저에는 로딩되어있지만, content 폴더에는 없는 에셋은 AssetMgr 에서 삭제하기
	// 로딩된 에셋에 해당하는 원본 파일이 Content 폴더에 있는지 Exist 체크
	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);
		for (const auto &pair : mapAsset)
		{
			// 엔진에서 제작한 에셋은 원래 원본파일이 없기때문에 넘어간다.
			if (pair.second->IsEngineAsset())
				continue;


			wstring strRelativePath = pair.second->GetRelativePath();

			if (strRelativePath == L"" || false == std::filesystem::exists(strContentPath + strRelativePath))
			{
				// AssetMgr에서만 들고 있는 경우
				if (pair.second->GetRefCount() <= 1)
				{
					// // AssetMgr에서 삭제 요청
					tTask deleteAssetTask{ TASK_TYPE::DEL_ASSET, (DWORD_PTR)pair.second.Get() };
					CTaskMgr::GetInst()->AddTask(deleteAssetTask);
				}

				// 다른 곳에서도 참조하고 있는 경우
				else
				{
					wstring msg = pair.second->GetName() + L"다른 곳에서 참조되고 있을 수 있습니다.";
					int ret = MessageBox(nullptr, msg.c_str(), L"에셋 삭제 경고", MB_YESNO);
					if (ret == IDYES)
					{
						
						CTaskMgr::GetInst()->AddTask(tTask{ TASK_TYPE::DEL_ASSET, (DWORD_PTR)pair.second.Get(), });
					}
				}
			}
		}
	}
}

void Content::FindAssetName(const wstring& _FolderPath, const wstring& _Filter)
{
	WIN32_FIND_DATA tFindData = {};

	// 경로에 맞는 파일 및 폴더를 탐색할 수 있는 커널 오브젝트 생성
	wstring strFindPath = _FolderPath + _Filter;
	HANDLE hFinder = FindFirstFile(strFindPath.c_str(), &tFindData);
	if (hFinder == INVALID_HANDLE_VALUE)
	{
		assert(!"해당 경로를 찾을 수 없습니다");
		return;
	}

	do
	{
		wstring strFindName = tFindData.cFileName;

		if (tFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// 폴더면 재귀 호출
			if (strFindName != L"." && strFindName != L"..")
			{
				FindAssetName(_FolderPath + strFindName + L"\\", _Filter);
			}
		}

		// 파일이면 경로 추가
		else
		{
			wstring RelativePath = CPathMgr::GetInst()->GetRelativePath(_FolderPath + strFindName);
			m_vecAssetRelativePath.push_back(RelativePath);
		}
	} while (FindNextFile(hFinder, &tFindData));

	FindClose(hFinder);
}

void Content::LoadAsset(const path& _Path)
{
	// 상대경로를 키 값으로 AssetMgr에 등록

	path ext = _Path.extension();

	if (ext == L".mesh")
		CAssetMgr::GetInst()->Load<CMesh>(_Path, _Path);
	//else if (ext == L".mdat")
		//CAssetMgr::GetInst()->Load<CMeshData>(_Path, _Path);
	else if (ext == L".mtrl")
		CAssetMgr::GetInst()->Load<CMaterial>(_Path, _Path);
	else if (ext == L".pref")
		CAssetMgr::GetInst()->Load<CPrefab>(_Path, _Path);
	else if (ext == L".png" || ext == L".jpg" || ext == L".jpeg" || ext == L".bmp" || ext == L".dds" || ext == L".tga"
		|| ext == L".PNG" || ext == L".JPG" || ext == L".JPEG" || ext == L".BMP" || ext == L".DDS" || ext == L".TGA")
		CAssetMgr::GetInst()->Load<CTexture>(_Path, _Path);
	else if (ext == L".mp3" || ext == L".mp4" || ext == L".ogg" || ext == L".wav" 
		|| ext == L".MP3" || ext == L".MP4" || ext == L".OGG" || ext == L".WAV")
		CAssetMgr::GetInst()->Load<CSound>(_Path, _Path);
	else if (ext == L".sprite")
		CAssetMgr::GetInst()->Load<CSprite>(_Path, _Path);
	else if (ext == L".flip")
		CAssetMgr::GetInst()->Load<CFlipBook>(_Path, _Path);
}


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