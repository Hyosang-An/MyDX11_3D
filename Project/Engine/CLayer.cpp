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
		// �׾�� finaltick�� ������ ��
		(*iter)->FinalTick();

		if ((*iter)->IsDead())
			iter = m_vecParents.erase(iter);
		else
			++iter;
	}
}

void CLayer::AddObject(CGameObject* _Object, bool _bChildMove)
{
	// 1. ������Ʈ�� �ٸ� ���̾� �Ҽ��ΰ��
	if (-1 != _Object->GetLayerIdx())
	{
		// �̹� �� ���̾ ���������� �ȵ�
		assert(_Object->GetLayerIdx() != m_LayerIdx);

		// ������ �Ҽӵ� ���̾�� �������;��Ѵ�.
		_Object->DisconnectWithLayer();
	}

	// 2. �ֻ��� ������Ʈ�� ���
	if (nullptr == _Object->GetParent())
	{
		m_vecParents.push_back(_Object);
	}

	// �ڽĵ���� �̵���ų�� ����
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