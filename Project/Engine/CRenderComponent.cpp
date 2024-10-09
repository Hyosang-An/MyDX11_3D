#include "pch.h"
#include "CRenderComponent.h"

#include "CLevelMgr.h"
#include "CAssetMgr.h"

#include "CLevel.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: CComponent(_Type)
	, m_Mesh(nullptr)
	, m_Mtrl(nullptr)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Origin) :
	CComponent(_Origin),
	m_Mesh(_Origin.m_Mesh),
	m_Mtrl(_Origin.m_Mtrl),
	m_SharedMtrl(_Origin.m_SharedMtrl)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (nullptr != pCurLevel)
	{
		// 현재 Play 상태가 아닌데 동적 재질이 존재하면 안됨.
		assert(!(pCurLevel->GetState() != LEVEL_STATE::PLAY
			&& nullptr != _Origin.m_DynamicMtrl));
	}

	if (nullptr != _Origin.m_DynamicMtrl)
	{
		GetDynamicMaterial();
	}
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl)
{
	m_Mtrl = m_SharedMtrl = _Mtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMtrl()
{
	m_Mtrl = m_SharedMtrl;
	return m_Mtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	// 동적재질 생성 및 반환은 게임(레벨) 이 Play 모드인 경우에만 호출이 되어야 한다.
	assert(CLevelMgr::GetInst()->GetCurrentLevel()->GetState() == LEVEL_STATE::PLAY);

	if (nullptr != m_DynamicMtrl)
		return m_Mtrl = m_DynamicMtrl;

	m_Mtrl = m_DynamicMtrl = m_SharedMtrl->Clone();
	return m_Mtrl;
}


void CRenderComponent::SaveDataToFile(FILE* _File)
{
	SaveAssetRef(m_Mesh, _File);
	SaveAssetRef(m_Mtrl, _File);
	SaveAssetRef(m_SharedMtrl, _File);
}

void CRenderComponent::LoadDataFromFile(FILE* _File)
{
	LoadAssetRef(m_Mesh, _File);
	LoadAssetRef(m_Mtrl, _File);
	LoadAssetRef(m_SharedMtrl, _File);
}