#include "pch.h"
#include "CCamera.h"

#include "CDevice.h"

#include "CRenderMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CTransform.h"
#include "CRenderComponent.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_Priority(-1)
	, m_LayerCheck(0)
	, m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
	, m_Width(0)
	, m_Height(0)
	, m_Far(100000.f)
	, m_FOV(XM_PI / 2.f)
{
	Vec2 vResolution = CDevice::GetInst()->GetResolution();
	m_Width = vResolution.x;
	m_Height = vResolution.y;
	m_AspectRatio = m_Width / m_Height;
}

CCamera::~CCamera()
{
}

void CCamera::Begin()
{
	// 카메라를 등록
	if (-1 != m_Priority)
	{
		CRenderMgr::GetInst()->RegisterCamera(this, m_Priority);
	}
}

void CCamera::FinalTick()
{
	Vec2 vResolution = CDevice::GetInst()->GetResolution();
	m_Width = vResolution.x;
	m_Height = vResolution.y;
	m_AspectRatio = m_Width / m_Height;

	/*Vec3 vWorldPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::LEFT))
		vWorldPos.x -= DT * 1.f;
	if(KEY_PRESSED(KEY::RIGHT))
		vWorldPos.x += DT * 1.f;

	Transform()->SetRelativePos(vWorldPos);*/

	// View Space 란 카메라가 좌표계의 기준이 되는 좌표계
	// 1. 카메라가 원점에 존재
	// 2. 카메라가 바라보는 방향이 Z 축

	// 1. 카메라가 있는 곳이 원점이었을 경우를 기준으로한 물체들의 좌표를 알아내야 한다.
	// 2. 카메라가 월드에서 바라보던 방향을 Z 축으로 돌려두어야 한다.
	//    물체들도 같이 회전을 한다.


	// View 행렬을 계산한다.
	// View 행렬은 World Space -> View Space 로 변경할때 사용하는 행렬		
	Matrix matTrans = XMMatrixTranslation(-Transform()->GetRelativePos().x, -Transform()->GetRelativePos().y, -Transform()->GetRelativePos().z);

	Vec3 vR = Transform()->GetWorldDir(DIR::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR::FRONT);

	Matrix matRot;
	matRot = Matrix(vR, vU, vF); // row 3개로 만들기
	matRot = matRot.Transpose();

	// 요소 하나씩 써서 만드는 방법
	//matRot._11 = vR.x; matRot._12 = vU.x; matRot._13 = vF.x;
	//matRot._21 = vR.y; matRot._22 = vU.y; matRot._23 = vF.y;
	//matRot._31 = vR.z; matRot._32 = vU.z; matRot._33 = vF.z;

	m_matView = matTrans * matRot;

	// 아래 방법도 가능
	//m_matView = XMMatrixLookAtLH(Transform()->GetRelativePos(), vF, vU);

	// Projection Space 투영 좌표계 (NDC)
	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		// 1. 직교투영 (Orthographic)
		// 투영을 일직선으로
		// 시야 범위를 NDC 로 압축
		m_matProj = XMMatrixOrthographicLH(m_Width * m_ProjectionScale, m_Height * m_ProjectionScale, 1.f, m_Far);
	}

	else
	{
		// 2. 원근투영 (Perspective)
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
	}
}

void CCamera::SortGameObject()
{
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (false == (m_LayerCheck & (1 << i)))
		{
			continue;
		}

		CLayer* pLayer = pLevel->GetLayer(i);

		const vector<CGameObject*>& vecObjects = pLayer->GetObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (nullptr == vecObjects[j]->GetRenderComponent()
				|| nullptr == vecObjects[j]->GetRenderComponent()->GetMesh()
				|| nullptr == vecObjects[j]->GetRenderComponent()->GetMaterial()
				|| nullptr == vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader())
			{
				continue;
			}

			Ptr<CGraphicShader> pShader = vecObjects[j]->GetRenderComponent()->GetMaterial()->GetShader();
			SHADER_DOMAIN Domain = pShader->GetDomain();

			switch (Domain)
			{
				case DOMAIN_OPAQUE:
					m_vecOpaque.push_back(vecObjects[j]);
					break;
				case DOMAIN_MASKED:
					m_vecMasked.push_back(vecObjects[j]);
					break;
				case DOMAIN_TRANSPARENT:
					m_vecTransparent.push_back(vecObjects[j]);
					break;
				case DOMAIN_PARTICLE:
					m_vecParticles.push_back(vecObjects[j]);
					break;
				case DOMAIN_POSTPROCESS:
					m_vecPostProcess.push_back(vecObjects[j]);
					break;
				case DOMAIN_UI:
					m_vecUI.push_back(vecObjects[j]);
					break;
			}
		}
	}
}

void CCamera::Render()
{
	// 오브젝트 분류
	SortGameObject();

	// 물체가 렌더링될 때 사용할 View, Proj 행렬
	g_Trans.matView = m_matView;
	g_Trans.matProj = m_matProj;

	// Opaque
	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
	{
		m_vecOpaque[i]->Render();
	}

	// Masked
	for (size_t i = 0; i < m_vecMasked.size(); ++i)
	{
		m_vecMasked[i]->Render();
	}

	// Transparent
	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->Render();
	}

	// Particles
	for (size_t i = 0; i < m_vecParticles.size(); ++i)
	{
		m_vecParticles[i]->Render();
	}

	// PostProcess 
	for (size_t i = 0; i < m_vecPostProcess.size(); ++i)
	{
		

		CRenderMgr::GetInst()->PostProcessCopy((int)i);
		m_vecPostProcess[i]->Render();
	}

	m_vecOpaque.clear();
	m_vecMasked.clear();
	m_vecTransparent.clear();
	m_vecParticles.clear();
	m_vecPostProcess.clear();
	m_vecUI.clear();
}

void CCamera::SaveToFile(FILE* _File)
{
	fwrite(&m_Priority, sizeof(int), 1, _File);
	fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
	fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	//fwrite(&m_Width, sizeof(float), 1, _File);
	//fwrite(&m_Height, sizeof(float), 1, _File);
	//fwrite(&m_AspectRatio, sizeof(float), 1, _File);
	fwrite(&m_Far, sizeof(float), 1, _File);
	fwrite(&m_FOV, sizeof(float), 1, _File);
	fwrite(&m_ProjectionScale, sizeof(float), 1, _File);
}

void CCamera::LoadFromFile(FILE* _File)
{
	fread(&m_Priority, sizeof(int), 1, _File);
	fread(&m_LayerCheck, sizeof(UINT), 1, _File);
	fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	//fread(&m_Width, sizeof(float), 1, _File);
	//fread(&m_Height, sizeof(float), 1, _File);
	//fread(&m_AspectRatio, sizeof(float), 1, _File);
	fread(&m_Far, sizeof(float), 1, _File);
	fread(&m_FOV, sizeof(float), 1, _File);
	fread(&m_ProjectionScale, sizeof(float), 1, _File);
}