#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"

#include "CConstBuffer.h"

#include "CCamera.h"
#include "CTimeMgr.h"
#include "CAssetMgr.h"
#include "CKeyMgr.h"
#include "CDevice.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CLevelMgr.h"
#include "CLevel.h"

#include "CLight2D.h"
#include "CLight3D.h"
#include "CStructuredBuffer.h"

#include "CMRT.h"



CRenderMgr::CRenderMgr()
{
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light3DBuffer = new CStructuredBuffer;
}

CRenderMgr::~CRenderMgr()
{
	if (nullptr != m_DebugObject)
		delete m_DebugObject;

	if (nullptr != m_Light2DBuffer)
		delete m_Light2DBuffer;

	if (nullptr != m_Light3DBuffer)
		delete m_Light3DBuffer;

	Delete_Array(m_arrMRT);
}



void CRenderMgr::Init()
{
	// AssetMgr 가 초기화될때 만들어둔 후처리용 텍스쳐를 참조한다.
	SetPostProcessTex();


	// 디버그 렌더링용 게임 오브젝트
	m_DebugObject = new CGameObject;
	m_DebugObject->AddComponent(new CTransform);
	m_DebugObject->AddComponent(new CMeshRender);
	m_DebugObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"DebugShapeMtrl"));

	// MRT 생성
	CreateMRT();

	// RenderMgr 전용 재질 생성
	CreateMaterial();
}


void CRenderMgr::SetPostProcessTex()
{
	for (int i = 0; i < CAssetMgr::GetInst()->GetPostProcessTextCnt(); i++)
	{
		wstring texName = L"PostProcessRTTex_" + std::to_wstring(i);

		m_vecPostProcessRTTex.push_back(CAssetMgr::GetInst()->FindAsset<CTexture>(texName));
	}

	// m_PostProcessDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"PostProcessDSTex");
}

void CRenderMgr::Tick()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	if (nullptr == pCurLevel)
		return;

	RenderStart();

	// Level 이 Play 상태인 경우, Level 내에 있는 카메라 시점으로 렌더링하기
	if (PLAY == pCurLevel->GetState())
	{
		if (nullptr != m_vecCam[0])
			Render(m_vecCam[0]);

		for (size_t i = 1; i < m_vecCam.size(); ++i)
		{
			if (nullptr == m_vecCam[i])
				continue;

			Render_Sub(m_vecCam[1]);
		}
	}

	// Level 이 Stop 이나 Pause 인 경우, Editor 용 카메라 시점으로 렌더링 하기
	else
	{
		if (nullptr != m_EditorCamera)
		{
			Render(m_EditorCamera);
		}
	}

	//// Scene UI Test용
	//Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	//Ptr<CTexture> pSceneTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RTTex_For_SceneUI");
	//if (pSceneTex != nullptr)
	//	CONTEXT->CopyResource(pSceneTex->GetTex2D().Get(), pRTTex->GetTex2D().Get());

	// Debug Render
	RenderDebugShape();

	// Time 정보 출력
	CTimeMgr::GetInst()->Render();

	// Lightvec Clear
	Clear();
}




void CRenderMgr::RenderStart()
{
	// 렌더타겟 클리어 및 지정
	//m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->ClearRT();
	//m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->ClearDS(); // 어차피 Render들어가면서 ClearMRT()를 해주기 때문에 필요없다.
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();

	// GlobalData 설정
	g_GlobalData.g_Resolution = CDevice::GetInst()->GetResolution();
	g_GlobalData.g_Light2DCount = (int)m_vecLight2D.size();
	g_GlobalData.g_Light3DCount = (int)m_vecLight3D.size();

	// Light2D 정보 업데이트 및 바인딩
	vector<tLightInfo> vecLight2DInfo;
	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		vecLight2DInfo.push_back(m_vecLight2D[i]->GetLightInfo());
	}

	if (m_Light2DBuffer->GetElementCount() < vecLight2DInfo.size())
	{
		m_Light2DBuffer->Create(sizeof(tLightInfo), (UINT)vecLight2DInfo.size());
	}

	if (!vecLight2DInfo.empty())
	{
		m_Light2DBuffer->SetData(vecLight2DInfo.data());
		m_Light2DBuffer->Binding(11);
	}

	// Light3D 정보 업데이트 및 바인딩
	vector<tLightInfo> vecLight3DInfo;
	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		vecLight3DInfo.push_back(m_vecLight3D[i]->GetLightInfo());
	}

	if (m_Light3DBuffer->GetElementCount() < vecLight3DInfo.size())
	{
		m_Light3DBuffer->Create(sizeof(tLightInfo), (UINT)vecLight3DInfo.size());
	}

	if (!vecLight3DInfo.empty())
	{
		m_Light3DBuffer->SetData(vecLight3DInfo.data());
		m_Light3DBuffer->Binding(12);
	}

	// 현재 화면을 렌더링하는 카메라의 월드포즈를 Global 데이터에 전달 (0번 카메라만 전달)
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	CCamera* pCam = nullptr;
	if (PLAY == pCurLevel->GetState())
		pCam = m_vecCam[0];
	else
		pCam = m_EditorCamera;

	if (pCam == nullptr)
		g_GlobalData.g_CamWorldPos = Vec3(0.f, 0.f, 0.f);
	else
		g_GlobalData.g_CamWorldPos = pCam->Transform()->GetWorldPos();

	// GlobalData 바인딩
	static CConstBuffer* pGlobalCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalCB->SetData(&g_GlobalData);
	pGlobalCB->Binding();
}

void CRenderMgr::Render(CCamera* _Cam)
{
	// 오브젝트 분류
	_Cam->SortGameObject();

	// 카메라 변환행렬 설정
	// 물체가 렌더링될 때 사용할 View, Proj 행렬
	g_Trans.matView = _Cam->GetViewMat();
	g_Trans.matProj = _Cam->GetProjMat();

	// MRT 모두 클리어
	ClearMRT();

	// ==================
	// DEFERRED RENDERING
	// ==================
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();
	_Cam->render_deferred();

	// ===============
	// LIGHT RENDERING
	// ===============
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->OMSet();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		m_vecLight3D[i]->Render();
	}

	// ===================================
	// MERGE ALBEDO + LIGHTS ==> SwapChain
	// ===================================
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();
	m_MergeMtrl->Binding();
	m_RectMesh->Render();


	// =================
	// FORWARD RENDERING
	// =================
	// 분류된 물체들 렌더링
	_Cam->render_opaque();
	_Cam->render_masked();
	_Cam->render_effect();
	_Cam->render_transparent();
	_Cam->render_particle();
	_Cam->render_postprocess();
	_Cam->render_ui();

	// 정리
	_Cam->clear();
}

void CRenderMgr::Render_Sub(CCamera* _Cam)
{
}

void CRenderMgr::Clear()
{
	m_vecLight2D.clear();
	m_vecLight3D.clear();
}

void CRenderMgr::RenderDebugShape()
{
	list<tDebugShapeInfo>::iterator iter = m_DebugShapeList.begin();

	for (; iter != m_DebugShapeList.end(); )
	{
		// 디버그 요청 모양에 맞는 메시를 가져옴
		switch ((*iter).Shape)
		{
			case DEBUG_SHAPE::RECT:
				m_DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh_Debug"));
				break;
			case DEBUG_SHAPE::CIRCLE:
				m_DebugObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CircleMesh_Debug"));
				break;
			case DEBUG_SHAPE::LINE:

				break;
			case DEBUG_SHAPE::CUBE:

				break;
			case DEBUG_SHAPE::SPHERE:

				break;
		}

		// 위치 세팅
		m_DebugObject->Transform()->SetWorldMatrix((*iter).matWorld);

		// 재질 세팅
		m_DebugObject->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, (*iter).vColor);

		// 깊이판정 여부에 따라서, 쉐이더의 깊이판정 방식을 결정한다.
		if ((*iter).DepthTest)
			m_DebugObject->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::LESS);
		else
			m_DebugObject->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

		// 렌더링
		m_DebugObject->MeshRender()->Render();


		// 수명이 다한 디버그 정보를 삭제
		(*iter).Age += EngineDT;
		if ((*iter).LifeTime < (*iter).Age)
		{
			iter = m_DebugShapeList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CRenderMgr::CreateMRT()
{
	CMRT* pMRT = nullptr;

	// =============
	// SwapChain MRT
	// =============
	{
		Ptr<CTexture> arrRT[8] = { CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex"), };
		Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
		Vec4		  arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f), };

		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->SetName(L"SwapChain");
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(1, arrRT, pDSTex);
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->SetClearColor(arrClearColor, false);
	}

	// =============
	// Effect MRT
	// =============
	{
		Ptr<CTexture> arrRT[8] = { CAssetMgr::GetInst()->FindAsset<CTexture>(L"EffectTargetTex"), };
		Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"EffectDepthStencilTex");
		Vec4		  arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f), };

		m_arrMRT[(UINT)MRT_TYPE::EFFECT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::EFFECT]->SetName(L"Effect");
		m_arrMRT[(UINT)MRT_TYPE::EFFECT]->Create(1, arrRT, pDSTex);
		m_arrMRT[(UINT)MRT_TYPE::EFFECT]->SetClearColor(arrClearColor, false);
	}

	// ===============
	// EffectBlur MRT
	// ===============
	{
		Ptr<CTexture> arrRT[8] = { CAssetMgr::GetInst()->FindAsset<CTexture>(L"EffectBlurTargetTex"), };
		Ptr<CTexture> pDSTex = nullptr;
		Vec4		  arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f), };

		m_arrMRT[(UINT)MRT_TYPE::EFFECT_BLUR] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::EFFECT_BLUR]->SetName(L"EffectBlur");
		m_arrMRT[(UINT)MRT_TYPE::EFFECT_BLUR]->Create(1, arrRT, nullptr);
		m_arrMRT[(UINT)MRT_TYPE::EFFECT_BLUR]->SetClearColor(arrClearColor, false);
	}

	// ========
	// Deferred
	// ========
	{
		Vec2 vResolution = CDevice::GetInst()->GetResolution();

		Ptr<CTexture> arrRT[8] =
		{
			CAssetMgr::GetInst()->CreateTexture(L"AlbedoTargetTex"
											, (UINT)vResolution.x, (UINT)vResolution.y
											, DXGI_FORMAT_R8G8B8A8_UNORM
											, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"NormalTargetTex"
											, (UINT)vResolution.x, (UINT)vResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT
											, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"PositionTargetTex"
											, (UINT)vResolution.x, (UINT)vResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT
											, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"EmissiveTargetTex"
											, (UINT)vResolution.x, (UINT)vResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT
											, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"DataTargetTex"
											, (UINT)vResolution.x, (UINT)vResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT
											, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};
		Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
		Vec4		  arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f), };

		m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->SetName(L"Deferred");
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(5, arrRT, pDSTex);
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->SetClearColor(arrClearColor, false);
	}

	// =====
	// LIGHT
	// =====
	{
		Vec2 vResolution = CDevice::GetInst()->GetResolution();

		Ptr<CTexture> arrRT[8] =
		{
			CAssetMgr::GetInst()->CreateTexture(L"DiffuseTargetTex"
											, (UINT)vResolution.x, (UINT)vResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT
											, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CAssetMgr::GetInst()->CreateTexture(L"SpecularTargetTex"
											, (UINT)vResolution.x, (UINT)vResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT
											, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};
		Ptr<CTexture> pDSTex = nullptr;
		Vec4		  arrClearColor[8] = { Vec4(0.f, 0.f, 0.f, 0.f), };

		m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->SetName(L"Light");
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(2, arrRT, pDSTex);
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->SetClearColor(arrClearColor, false);
	}
}

void CRenderMgr::ClearMRT()
{
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Clear();

	// 어차피 다른 MRT는 같은 DepthStencilTex를 사용하므로, 한번만 지워주면 된다.
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->ClearRT();
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->ClearRT();
}

void CRenderMgr::CreateMaterial()
{
	// DirLightShader
	Ptr<CGraphicShader> pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\light.fx", "VS_DirLight");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_DirLight");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);
	CAssetMgr::GetInst()->AddAsset(L"DirLightShader", pShader);

	// DirLightMtrl
	Ptr<CMaterial> pMtrl = new CMaterial(true);
	pMtrl->SetShader(pShader);
	pMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"NormalTargetTex"));
	CAssetMgr::GetInst()->AddAsset(L"DirLightMtrl", pMtrl);


	// MergeShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\merge.fx", "VS_Merge");
	pShader->CreatePixelShader(L"shader\\merge.fx", "PS_Merge");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_NONE);

	// MergeMtrl
	m_MergeMtrl = new CMaterial(true);
	m_MergeMtrl->SetShader(pShader);
	m_MergeMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"AlbedoTargetTex"));
	m_MergeMtrl->SetTexParam(TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"DiffuseTargetTex"));
	m_MergeMtrl->SetTexParam(TEX_2, CAssetMgr::GetInst()->FindAsset<CTexture>(L"SpecularTargetTex"));

	// RectMesh
	m_RectMesh = CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh");
}

void CRenderMgr::RegisterCamera(CCamera* _cam, int _camPriority)
{
	// 카메라 우선순위에 따라서 벡터의 공간의 마련되어야 한다.
	if (m_vecCam.size() <= _camPriority + 1)
		m_vecCam.resize(_camPriority + 1);

	// 카메라 우선순위에 맞는 위치에 넣는다
	m_vecCam[_camPriority] = _cam;
}

void CRenderMgr::PostProcessCopy(int _postProcessRTTex_index)
{
	assert(m_vecPostProcessRTTex.size() > _postProcessRTTex_index);

	// RenderTarget -> PostProcessRTTex
	Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_vecPostProcessRTTex[_postProcessRTTex_index]->GetTex2D().Get(), pRTTex->GetTex2D().Get());

	// DepthStencilTex -> PostProcessDSTex
	//Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
	//CONTEXT->CopyResource(m_PostProcessDSTex->GetTex2D().Get(), pDSTex->GetTex2D().Get());
}

CCamera* CRenderMgr::GetPOVCam()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	if (nullptr == pCurLevel)
		return nullptr;

	if (LEVEL_STATE::PLAY == pCurLevel->GetState())
	{
		if (m_vecCam.empty())
			return nullptr;

		return m_vecCam[0];
	}

	else
	{
		return m_EditorCamera;
	}
}

void CRenderMgr::RegisterLight2D(CLight2D* _light)
{
	// 중복 등록 방지
	if (std::find(m_vecLight2D.begin(), m_vecLight2D.end(), _light) == m_vecLight2D.end())
	{
		m_vecLight2D.push_back(_light);
	}
}