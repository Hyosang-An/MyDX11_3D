#include "pch.h"
#include "CRenderComponent.h"

#include "CLevelMgr.h"
#include "CAssetMgr.h"

#include "CLevel.h"
#include "CTransform.h"
#include "CAnimator3D.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: CComponent(_Type)
	, m_Mesh(nullptr)
{
}

CRenderComponent::CRenderComponent(const CRenderComponent& _Origin) :
	CComponent(_Origin),
	m_Mesh(_Origin.m_Mesh),
	m_FrustumCheck(_Origin.m_FrustumCheck)
{
	m_vecMtrls.resize(_Origin.m_vecMtrls.size());

	for (size_t i = 0; i < _Origin.m_vecMtrls.size(); ++i)
	{
		m_vecMtrls[i].pCurMtrl = _Origin.m_vecMtrls[i].pCurMtrl;
		m_vecMtrls[i].pSharedMtrl = _Origin.m_vecMtrls[i].pSharedMtrl;

		// 원본 오브젝트가 공유재질을 참조하고 있고, 현재 사용재질은 공유재질이 아닌경우
		if (_Origin.m_vecMtrls[i].pSharedMtrl != _Origin.m_vecMtrls[i].pCurMtrl)
		{
			assert(_Origin.m_vecMtrls[i].pDynamicMtrl.Get());

			// 복사 렌더 컴포넌트도 별도의 동적재질을 생성한다.
			GetDynamicMaterial(i);

			// 원본 렌더컴포넌트의 동적재질 값을 현재 생성한 동적재질로 복사한다.
			*m_vecMtrls[i].pDynamicMtrl.Get() = *_Origin.m_vecMtrls[i].pDynamicMtrl.Get();
		}
		else
		{
			m_vecMtrls[i].pCurMtrl = m_vecMtrls[i].pSharedMtrl;
		}
	}
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMesh(Ptr<CMesh> _Mesh)
{
	m_Mesh = _Mesh;

	if (!m_vecMtrls.empty())
	{
		m_vecMtrls.clear();
		vector<tMtrlSet> vecMtrls;
		m_vecMtrls.swap(vecMtrls);
	}

	if (nullptr != m_Mesh)
		m_vecMtrls.resize(m_Mesh->GetSubsetCount());
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl, UINT _idx)
{
	// _idx 기본값 : 0
	
	// 재질이 변경되면 기존에 복사본 받아둔 DynamicMaterial 을 삭제한다.
	m_vecMtrls[_idx].pSharedMtrl = _Mtrl;
	m_vecMtrls[_idx].pCurMtrl = _Mtrl;
	m_vecMtrls[_idx].pDynamicMtrl = nullptr;
}

Ptr<CMaterial> CRenderComponent::GetMaterial(UINT _idx)
{
	// _idx 기본값 : 0
	return m_vecMtrls[_idx].pCurMtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(UINT _idx)
{
	// _idx 기본값 : 0
	
	// 공유재질을 가져오는것으로 현재 사용재질을 동적재질에서 회복하도록 한다
	m_vecMtrls[_idx].pCurMtrl = m_vecMtrls[_idx].pSharedMtrl;

	if (m_vecMtrls[_idx].pDynamicMtrl.Get())
	{
		// 이거 그냥 null로 주면 플로팅 포인터 될거같은데?
		m_vecMtrls[_idx].pDynamicMtrl = nullptr;
	}

	return m_vecMtrls[_idx].pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(UINT _idx)
{
	// _idx 기본값 : 0

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (pCurLevel->GetState() != LEVEL_STATE::PLAY)
		return nullptr;

	// 원본 재질이 없다 -> Nullptr 반환
	if (nullptr == m_vecMtrls[_idx].pSharedMtrl)
	{
		m_vecMtrls[_idx].pCurMtrl = nullptr;
		m_vecMtrls[_idx].pDynamicMtrl = nullptr;
		return m_vecMtrls[_idx].pCurMtrl;
	}

	if (nullptr == m_vecMtrls[_idx].pDynamicMtrl)
	{
		m_vecMtrls[_idx].pDynamicMtrl = m_vecMtrls[_idx].pSharedMtrl->Clone();
		m_vecMtrls[_idx].pDynamicMtrl->SetName(m_vecMtrls[_idx].pSharedMtrl->GetName() + L"_Clone");
		m_vecMtrls[_idx].pCurMtrl = m_vecMtrls[_idx].pDynamicMtrl;
	}

	return m_vecMtrls[_idx].pCurMtrl;
}


void CRenderComponent::render_shadowmap()
{
	// 재질은 ShadowMapMtrl 로 이미 Binding 되어있는걸 사용할 것
	// 자신이 선택한 Mesh 로 렌더링 요청을 하면 된다.
	Transform()->Binding();

	Ptr<CMaterial> shadowmapMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DirLightShadowMapMtrl");
	// Animator3D Binding
	if (Animator3D())
	{
		Animator3D()->Binding();

		shadowmapMtrl->SetAnim3D(true); // Animation Mesh 알리기
		shadowmapMtrl->SetBoneCount(Animator3D()->GetBoneCount());
	}

	shadowmapMtrl->Binding();

	// 모든 서브셋 메쉬의 깊이를 저장한다.
	for (int i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		GetMesh()->Render(i);
	}

	shadowmapMtrl->SetAnim3D(false); // clear
	shadowmapMtrl->SetBoneCount(0);
}

void CRenderComponent::SaveAssetDataToFile(FILE* _File)
{
	// 메쉬 참조정보 저장
	SaveAssetRef(m_Mesh, _File);

	// 재질 참조정보 저장
	UINT iMtrlCount = GetMaterialCount();
	fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		SaveAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
	}
}

void CRenderComponent::LoadAssetDataFromFile(FILE* _File)
{
	// 메쉬 참조정보 불러오기
	LoadAssetRef(m_Mesh, _File);

	// 재질 참조정보 불러오기
	UINT iMtrlCount = GetMaterialCount();
	fread(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		LoadAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
	}
}