#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer(int _LayerIdx) :
	m_LayerIdx{_LayerIdx}
{
}

CLayer::CLayer(const CLayer& _Origin) :
	CEntity(_Origin),
	m_LayerIdx(_Origin.m_LayerIdx)
{
	for (size_t i = 0; i < _Origin.m_vecParents.size(); ++i)
	{
		AddObject(_Origin.m_vecParents[i]->Clone(), false);
	}
}

CLayer::~CLayer()
{
	Delete_Vec(m_vecParents);
}

void CLayer::Begin()
{
	for (size_t i = 0; i < m_vecParents.size(); ++i)
	{
		m_vecParents[i]->Begin();
	}
}

void CLayer::Tick()
{
	for (size_t i = 0; i < m_vecParents.size(); ++i)
	{
		m_vecParents[i]->Tick();
	}
}

void CLayer::FinalTick()
{
	vector<CGameObject*>::iterator iter = m_vecParents.begin();

	for (; iter != m_vecParents.end(); )
	{
		// 죽었어도 finaltick은 돌려야 함
		(*iter)->FinalTick();

		if ((*iter)->IsDead())
			iter = m_vecParents.erase(iter);
		else
			++iter;
	}
}

void CLayer::AddObject(CGameObject* _Object, bool _bChildMove)
{
	// 1. 오브젝트가 다른 레이어 소속인경우
	if (-1 != _Object->GetLayerIdx())
	{
		// 이미 이 레이어에 속해있으면 안됨
		assert(_Object->GetLayerIdx() != m_LayerIdx);

		// 기존에 소속된 레이어에서 빠져나와야한다.
		_Object->DisconnectWithLayer();
	}

	// 2. 최상위 오브젝트인 경우
	if (nullptr == _Object->GetParent())
	{
		m_vecParents.push_back(_Object);
	}

	// 자식들까지 이동시킬지 말지
	static list<CGameObject*> queue;
	queue.clear();
	queue.push_back(_Object);
	_Object->m_LayerIdx = m_LayerIdx;

	while (!queue.empty())
	{
		CGameObject* pObject = queue.front();
		queue.pop_front();

		if (_bChildMove || pObject->m_LayerIdx == -1)
		{
			pObject->m_LayerIdx = m_LayerIdx;
		}

		const vector<CGameObject*>& vecChildren = pObject->GetChildren();
		for (size_t i = 0; i < vecChildren.size(); ++i)
		{
			queue.push_back(vecChildren[i]);
		}
	}
}


void CLayer::DisconnectWithObject(CGameObject* _Object)
{
	vector<CGameObject*>::iterator iter = m_vecParents.begin();
	for (; iter != m_vecParents.end(); ++iter)
	{
		if (_Object == (*iter))
		{
			m_vecParents.erase(iter);
			return;
		}
	}

	assert(nullptr);
}

void CLayer::RegisterAsParent(CGameObject* _Object)
{
	assert(_Object->GetLayerIdx() == m_LayerIdx && !_Object->GetParent());

	m_vecParents.push_back(_Object);
	return;
}

void CLayer::DeregisterAsParent(CGameObject* _Object)
{
	vector<CGameObject*>::iterator iter = m_vecParents.begin();

	for (; iter != m_vecParents.end(); ++iter)
	{
		if (_Object == (*iter))
		{
			m_vecParents.erase(iter);
			return;
		}
	}

	assert(nullptr);
}

void CLayer::DeregisterObject(CGameObject* _Object)
{
	assert(_Object->m_LayerIdx == m_LayerIdx);

	_Object->m_LayerIdx = -1;

	if (nullptr == _Object->GetParent())
	{
		DeregisterAsParent(_Object);

		/*vector<CGameObject*>::iterator iter = m_vecParents.begin();

		for (; iter != m_vecParents.end(); ++iter)
		{
			if (_Object == (*iter))
			{
				m_vecParents.erase(iter);
				return;
			}
		}

		assert(nullptr);*/
	}
}