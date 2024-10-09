#include "pch.h"
#include "CLevel.h"

#include "CLevelMgr.h"

#include "CLayer.h"
#include "CGameObject.h"

CLevel::CLevel() :
	m_vecLayer{}
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_vecLayer[i] = new CLayer(i);

		// 레이어 이름 설정
		switch (i)
		{
		case 0:
			m_vecLayer[i]->SetName(L"Default");
			break;
		case 1:
			m_vecLayer[i]->SetName(L"Background");
			break;
		case 2:
			m_vecLayer[i]->SetName(L"Tile");
			break;
		case 3:
			m_vecLayer[i]->SetName(L"Player");
			break;
		case 4:
			m_vecLayer[i]->SetName(L"Monster");
			break;
		case 5:
			m_vecLayer[i]->SetName(L"PlayerProjectile");
			break;
		case 6:
			m_vecLayer[i]->SetName(L"MonsterProjectile");
			break;
		case 7:
			m_vecLayer[i]->SetName(L"UI");
			break;

		default:
			m_vecLayer[i]->SetName(L"NO_NAME");
		}
	}
}

CLevel::CLevel(const CLevel& _Origin) :
	CEntity(_Origin),
	m_vecLayer{},
	m_State(LEVEL_STATE::STOP)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_vecLayer[i] = _Origin.m_vecLayer[i]->Clone();
	}
}

CLevel::~CLevel()
{
	Delete_Array(m_vecLayer);
}

void CLevel::Begin()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_vecLayer[i]->Begin();
	}
}

void CLevel::Tick()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_vecLayer[i]->Tick();
	}
}

void CLevel::FinalTick()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_vecLayer[i]->FinalTick();
	}
}

void CLevel::AddObject(int LayerIdx, CGameObject* _Object, bool _bMoveChild)
{
	m_vecLayer[LayerIdx]->AddObject(_Object, _bMoveChild);

	CLevelMgr::GetInst()->LevelChanged();
}

void CLevel::RegisterAsParent(int LayerIdx, CGameObject* _Object)
{
	m_vecLayer[LayerIdx]->RegisterAsParent(_Object);
}

CGameObject* CLevel::FindObjectByName(const wstring& _Name)
{
	for (UINT layerIndex = 0; layerIndex < MAX_LAYER; ++layerIndex)
	{
		const vector<CGameObject*>& vecParent = m_vecLayer[layerIndex]->GetParentObjects();

		for (size_t parentIndex = 0; parentIndex < vecParent.size(); ++parentIndex)
		{
			// BFS 탐색
			list<CGameObject*> objectList;
			objectList.push_back(vecParent[parentIndex]);

			while (!objectList.empty())
			{
				CGameObject* pObject = objectList.front();
				objectList.pop_front();

				// 먼저 현재 객체의 이름을 확인
				if (_Name == pObject->GetName())
				{
					return pObject;
				}

				// 자식 객체들을 리스트에 추가
				const vector<CGameObject*>& vecChild = pObject->GetChildren();
				for (size_t childIndex = 0; childIndex < vecChild.size(); ++childIndex)
				{
					objectList.push_back(vecChild[childIndex]);
				}
			}
		}
	}

	return nullptr;
}

void CLevel::ClearObject()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_vecLayer[i]->ClearObject();
	}
}

void CLevel::ChangeState(LEVEL_STATE _NextState)
{
	if (m_State == _NextState)
		return;

	// Stop -> Pause (X)
	if (STOP == m_State && PAUSE == _NextState)
		return;

	// Stop -> Play (정지 상태의 레벨이 시작되면, 레벨에 있던 물체들은 Begin 이 호출되어야 한다.)
	if (STOP == m_State && PLAY == _NextState)
	{
		m_State = _NextState;
		Begin();
	}
	else
		m_State = _NextState;

	// Play -> Stop (최초 레벨이 시작되던 시점으로 복구가 가능해야 한다.)
}