#include "pch.h"
#include "CMRT.h"

#include "CDevice.h"

CMRT::CMRT()
	: m_arrRT{}
	, m_ClearColor{}
	, m_RTCount(0)
	, m_ViewPort{}
{
}

CMRT::~CMRT()
{
}

void CMRT::Create(int _RTCount, Ptr<CTexture>* _arrRT, Ptr<CTexture> _DSTex)
{
	m_RTCount = _RTCount;

	for (int i = 0; i < m_RTCount; ++i)
	{
		m_arrRT[i] = _arrRT[i];
	}

	m_DSTex = _DSTex;

	Ptr<CTexture> pTarget = m_arrRT[0];
	if (nullptr == pTarget)
		pTarget = m_DSTex;
	assert(pTarget);

	// ViewPort ¼³Á¤
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = pTarget->Width();
	m_ViewPort.Height = pTarget->Height();
	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;
}

void CMRT::SetClearColor(Vec4* _Color, bool _Independent)
{
	if (_Independent)
	{
		for (int i = 0; i < m_RTCount; ++i)
		{
			m_ClearColor[i] = _Color[i];
		}
	}

	else
	{
		for (int i = 0; i < m_RTCount; ++i)
		{
			m_ClearColor[i] = _Color[0];
		}
	}
}

void CMRT::OMSet()
{
	ID3D11RenderTargetView* arrRTV[8] = {};
	for (int i = 0; i < m_RTCount; ++i)
	{
		arrRTV[i] = m_arrRT[i]->GetRTV().Get();
	}

	ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
	if (nullptr != m_DSTex)
		pDSV = m_DSTex->GetDSV();

	CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, pDSV.Get());
	CONTEXT->RSSetViewports(1, &m_ViewPort);
}

void CMRT::ClearRT()
{
	for (int i = 0; i < m_RTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_arrRT[i]->GetRTV().Get(), m_ClearColor[i]);
	}
}

void CMRT::ClearDS()
{
	if (nullptr == m_DSTex)
		return;

	CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
