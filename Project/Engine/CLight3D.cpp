#include "pch.h"
#include "CLight3D.h"

#include "CRenderMgr.h"
#include "CAssetMgr.h"

#include "CTransform.h"
#include "CCamera.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
{
	SetLightType(LIGHT_TYPE::DIRECTIONAL);

	// 광원 전용 카메라
	m_Cam = new CGameObject;
	m_Cam->AddComponent(new CTransform);
	m_Cam->AddComponent(new CCamera);
}

CLight3D::CLight3D(const CLight3D& _Origin)
	: CComponent(_Origin)
	, m_Info(_Origin.m_Info)
	, m_LightIdx(-1)
{
	SetLightType(m_Info.Type);

	// 카메라 복사
	m_Cam = _Origin.m_Cam->Clone();
}

CLight3D::~CLight3D()
{
	if (nullptr != m_Cam)
		delete m_Cam;
}

void CLight3D::SetLightType(LIGHT_TYPE _Type)
{
	m_Info.Type = _Type;

	if (m_Info.Type == LIGHT_TYPE::DIRECTIONAL)
	{
		m_VolumeMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
		m_LightMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DirLightMtrl");
	}

	else if (m_Info.Type == LIGHT_TYPE::POINT)
	{
		m_VolumeMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh");
		m_LightMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"PointLightMtrl");
	}

	else if (m_Info.Type == LIGHT_TYPE::SPOT)
	{
		m_VolumeMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"ConeMesh");
		m_LightMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SpotLightMtrl");
	}
}

void CLight3D::CreateShadowMap()
{
}

void CLight3D::FinalTick()
{
	m_Info.WorldPos = Transform()->GetWorldPos();
	m_Info.WorldDir = Transform()->GetWorldDir(DIR::FRONT);

	// PointLight, SphereMesh, r = 0.5f
	Transform()->SetRelativeScale(m_Info.Radius * 2.f, m_Info.Radius * 2.f, m_Info.Radius * 2.f);

	// 자신을 RenderMgr 에 등록시킴
	m_LightIdx = CRenderMgr::GetInst()->RegisterLight3D(this);


	// DebugShape
	if (m_Info.Type == LIGHT_TYPE::POINT)
	{
		DrawDebugSphere(Transform()->GetWorldMat(), Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);
	}
}

void CLight3D::Render()
{
	Transform()->Binding();

	m_LightMtrl->SetScalarParam(INT_0, m_LightIdx);
	m_LightMtrl->Binding();

	m_VolumeMesh->Render();
}

void CLight3D::SetRadius(float _Radius)
{
	m_Info.Radius = _Radius;
}

void CLight3D::SaveToFile(FILE* _File)
{
	fwrite(&m_Info, sizeof(tLightInfo), 1, _File);
}

void CLight3D::LoadFromFile(FILE* _File)
{
	fread(&m_Info, sizeof(tLightInfo), 1, _File);
}