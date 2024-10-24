#include "pch.h"
#include "CSkyBox.h"

#include "CTransform.h"

#include "CAssetMgr.h"


CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_Type(SPHERE)
{
	SetSkyBoxType(m_Type);
}

CSkyBox::CSkyBox(const CSkyBox& _Origin)
	: CRenderComponent(_Origin)
	, m_Type(_Origin.m_Type)
	, m_SkyBoxTex(_Origin.m_SkyBoxTex)
{
	SetSkyBoxType(m_Type);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::SetSkyBoxType(SKYBOX_TYPE _Type)
{
	m_Type = _Type;

	if (SKYBOX_TYPE::SPHERE == m_Type)
		SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	else if (SKYBOX_TYPE::CUBE == m_Type)
		SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CubeMesh"));

	// Mesh 가 변경되면 재질을 다시 설정해야 한다.
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"SkyBoxMtrl"));
}

void CSkyBox::FinalTick()
{
	// 방법 1.

	// 1. 시점 카메라 의 최대 시야거리로, Transform 의 Scale 을 설정한다.
	//  - Play -> Main Cam
	//  - Pause, Stop -> Editor Cam

	// 2. SkyBox 위치를 시점 카메라 위치로 옮긴다.


	// 방법 2.
	// 쉐이더에서 처리한다.

}

void CSkyBox::Render()
{
	Transform()->Binding();

	GetMaterial()->SetScalarParam(INT_0, (int)m_Type);

	if (m_Type == SPHERE)
	{
		if (!m_SkyBoxTex->IsCubeMap())
			GetMaterial()->SetTexParam(TEX_0, m_SkyBoxTex);
		else
			GetMaterial()->SetTexParam(TEX_0, nullptr);
	}

	else if (m_Type == CUBE)
	{
		if (m_SkyBoxTex->IsCubeMap())
			GetMaterial()->SetTexParam(TEXCUBE_0, m_SkyBoxTex);
		else
			GetMaterial()->SetTexParam(TEXCUBE_0, nullptr);
	}

	GetMaterial()->Binding();

	GetMesh()->Render();
}

void CSkyBox::SaveToFile(FILE* _File)
{

}

void CSkyBox::LoadFromFile(FILE* _File)
{

}