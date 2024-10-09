#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"

#include "CConstBuffer.h"

#include "CCamera.h"
#include "CTimeMgr.h"
#include "CAssetMgr.h"
#include "CKeyMgr.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CLevelMgr.h"
#include "CLevel.h"

#include "CLight2D.h"
#include "CStructuredBuffer.h"



CRenderMgr::CRenderMgr()
{
	m_Light2DBuffer = new CStructuredBuffer;
}

CRenderMgr::~CRenderMgr()
{
	if (nullptr != m_DebugObject)
		delete m_DebugObject;

	if (nullptr != m_Light2DBuffer)
		delete m_Light2DBuffer;
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
}


void CRenderMgr::SetPostProcessTex()
{
	for (int i = 0; i < CAssetMgr::GetInst()->GetPostProcessTextCnt(); i++)
	{
		wstring texName = L"PostProcessRTTex_" + std::to_wstring(i);

		m_vecPostProcessRTTex.push_back(CAssetMgr::GetInst()->FindAsset<CTexture>(texName));
	}

	m_PostProcessDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"PostProcessDSTex");
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
		for (size_t i = 0; i < m_vecCam.size(); ++i)
		{
			if (nullptr == m_vecCam[i])
				continue;

			m_vecCam[i]->Render();
		}
	}

	// Level 이 Stop 이나 Pause 인 경우, Editor 용 카메라 시점으로 렌더링 하기
	else
	{
		if (nullptr != m_EditorCamera)
		{
			m_EditorCamera->Render();
		}
	}

	// Debug Render
	RenderDebugShape();

	// Time 정보 출력
	CTimeMgr::GetInst()->Render();

	// Lightvec Clear
	Clear();
}




void CRenderMgr::RenderStart()
{
	// 렌더타겟 지정
	Ptr<CTexture> pRTTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"RenderTargetTex");
	Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
	CONTEXT->OMSetRenderTargets(1, pRTTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());

	// TargetClear
	float color[4] = { 0.f, 0.f, 0.f, 1.f };
	CONTEXT->ClearRenderTargetView(pRTTex->GetRTV().Get(), color);
	CONTEXT->ClearDepthStencilView(pDSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	g_GlobalData.g_Resolution = Vec2((float)pRTTex->Width(), (float)pRTTex->Height());
	g_GlobalData.g_Light2DCount = (int)m_vecLight2D.size();

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

	// GlobalData 바인딩
	static CConstBuffer* pGlobalCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalCB->SetData(&g_GlobalData);
	pGlobalCB->Binding();
}

void CRenderMgr::Clear()
{
	m_vecLight2D.clear();
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
	Ptr<CTexture> pDSTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"DepthStencilTex");
	CONTEXT->CopyResource(m_PostProcessDSTex->GetTex2D().Get(), pDSTex->GetTex2D().Get());
}

void CRenderMgr::RegisterLight2D(CLight2D* _light)
{
	// 중복 등록 방지
	if (std::find(m_vecLight2D.begin(), m_vecLight2D.end(), _light) == m_vecLight2D.end())
	{
		m_vecLight2D.push_back(_light);
	}
}

void CRenderMgr::DeregisterLight2D(CLight2D* _light)
{
	auto it = std::find(m_vecLight2D.begin(), m_vecLight2D.end(), _light);
	if (it != m_vecLight2D.end())
	{
		m_vecLight2D.erase(it);
	}
}