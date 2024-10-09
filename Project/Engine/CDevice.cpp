#include "pch.h"
#include "CDevice.h"

#include "CConstBuffer.h"
#include "CAssetMgr.h"

CDevice::CDevice() :
	m_hWnd{},
	m_arrCB{},
	m_Sampler{}
{
}

CDevice::~CDevice()
{
	for (size_t i = 0; i < (size_t)CB_TYPE::END; i++)
	{
		if (m_arrCB[i] != nullptr)
			delete m_arrCB[i];
	}
}

int CDevice::Init(HWND _hWnd, UINT _width, UINT _height)
{
	// 윈도우 핸들 및 해상도 값 저장
	m_hWnd = _hWnd;

	m_vResolution.x = (float)_width;
	m_vResolution.y = (float)_height;

	// Device, Context 생성
	UINT Flag = 0;

#ifdef _DEBUG
	Flag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		Flag, nullptr, 0, D3D11_SDK_VERSION, m_Device.GetAddressOf(), nullptr, m_Context.GetAddressOf())))
	{
		MessageBox(nullptr, L"Device, Context 생성 실패", L"장치초기화 실패", MB_OK);
		return E_FAIL;
	}

	// 스왑체인
	if (FAILED(CreateSwapChain()))
	{
		MessageBox(nullptr, L"SwapChain 생성 실패", L"장치초기화 실패", MB_OK);
		return E_FAIL;
	}

	// RenderTargetView, DepthStencilView
	if (FAILED(CreateView()))
	{
		MessageBox(nullptr, L"View 생성 실패", L"장치초기화 실패", MB_OK);
		return E_FAIL;
	}


	// ViewPort 설정
   // 출력시킬 화면 윈도우 영역을 설정
	D3D11_VIEWPORT viewport = {};

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_vResolution.x;
	viewport.Height = m_vResolution.y;

	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.f;

	m_Context->RSSetViewports(1, &viewport);

	if (FAILED(CreateConstBuffer()))
	{
		MessageBox(nullptr, L"상수버퍼 생성 실패", L"장치 초기화 실패", MB_OK);
		return E_FAIL;
	}

	if (FAILED(CreateRasterizerState()))
	{
		MessageBox(nullptr, L"레스터라이저 스테이트 생성 실패", L"장치 초기화 실패", MB_OK);
		return E_FAIL;
	}

	if (FAILED(CreateDepthStencilState()))
	{
		MessageBox(nullptr, L"DepthStencil 스테이트 생성 실패", L"장치 초기화 실패", MB_OK);
		return E_FAIL;
	}

	if (FAILED(CreateBlendState()))
	{
		MessageBox(nullptr, L"Blend 스테이트 생성 실패", L"장치 초기화 실패", MB_OK);
		return E_FAIL;
	}

	if (FAILED(CreateSamplerState()))
	{
		MessageBox(nullptr, L"샘플러 스테이트 생성 실패", L"장치 초기화 실패", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

int CDevice::CreateSwapChain()
{
	// SwapChain : 화면에 그림을 그리기위한 버퍼를 관리하고,
	//             버퍼에 그려진 이미지를 화면 출력을 담당하는 객체
	DXGI_SWAP_CHAIN_DESC Desc = {};

	Desc.BufferCount = 1; // 백버퍼 개수
	Desc.BufferDesc.Width = (UINT)m_vResolution.x; // 백버퍼 해상도 
	Desc.BufferDesc.Height = (UINT)m_vResolution.y;// 백버퍼 해상도
	Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 픽셀 포맷
	Desc.BufferDesc.RefreshRate.Denominator = 60;
	Desc.BufferDesc.RefreshRate.Numerator = 1;
	Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Desc.Windowed = true;  // 창모드 설정
	Desc.OutputWindow = m_hWnd;// SwapChain 버퍼의 이미지를 출력시킬 윈도우 핸들
	Desc.Flags = 0;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	ComPtr<IDXGIDevice>     Device = nullptr;
	ComPtr<IDXGIAdapter>    Adapter = nullptr;
	ComPtr<IDXGIFactory>    Factory = nullptr;

	if (FAILED(m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)Device.GetAddressOf())))
		return E_FAIL;

	if (FAILED(Device->GetParent(__uuidof(IDXGIAdapter), (void**)Adapter.GetAddressOf())))
		return E_FAIL;

	if (FAILED(Adapter->GetParent(__uuidof(IDXGIFactory), (void**)Factory.GetAddressOf())))
		return E_FAIL;

	if (FAILED(Factory->CreateSwapChain(m_Device.Get(), &Desc, m_SwapChain.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CDevice::CreateView()
{
	// =======================================================
	// RenderTarget Texture, DepthStencil Texture 를 생성시킨다
	// =======================================================
	// 스왚체인의 백버퍼의 주소를 받아온다.
	ComPtr<ID3D11Texture2D> RenderTargetTex;

	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)RenderTargetTex.GetAddressOf()); 
	// (void**)로 강제 형변환을 하도록 하여 실수로 다른 이중 포인터 변수가 들어가는것을 방지

	m_RTTex = CAssetMgr::GetInst()->CreateTexture(L"RenderTargetTex", RenderTargetTex);

	// DepthStencil 텍스쳐 생성
	m_DSTex = CAssetMgr::GetInst()->CreateTexture(L"DepthStencilTex"
		, (UINT)m_vResolution.x, (UINT)m_vResolution.y
		, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	return S_OK;
}

int CDevice::CreateConstBuffer()
{
	CConstBuffer* pCB = nullptr;

	// 월드, 뷰, 투영 행렬 전달
	pCB = new CConstBuffer;
	if (FAILED(pCB->Create(CB_TYPE::TRANSFORM, sizeof(tTransform))))
	{
		MessageBox(nullptr, L"TRANSFORM 상수버퍼 생성 실패", L"초기화 실패", MB_OK);
		return E_FAIL;
	}
	m_arrCB[(UINT)CB_TYPE::TRANSFORM] = pCB;

	pCB = new CConstBuffer;
	if (FAILED(pCB->Create(CB_TYPE::MATERIAL, sizeof(tMtrlConst))))
	{
		MessageBox(nullptr, L"MATERIAL 상수버퍼 생성 실패", L"초기화 실패", MB_OK);
		return E_FAIL;
	}
	m_arrCB[(UINT)CB_TYPE::MATERIAL] = pCB;

	pCB = new CConstBuffer;
	if (FAILED(pCB->Create(CB_TYPE::SPRITE, sizeof(tSpriteInfo))))
	{
		MessageBox(nullptr, L"SPRITE 상수버퍼 생성 실패", L"초기화 실패", MB_OK);
		return E_FAIL;
	}
	m_arrCB[(UINT)CB_TYPE::SPRITE] = pCB;

	pCB = new CConstBuffer;
	if (FAILED(pCB->Create(CB_TYPE::GLOBAL, sizeof(tGlobalData))))
	{
		MessageBox(nullptr, L"상수버퍼 생성 실패", L"초기화 실패", MB_OK);
		return E_FAIL;
	}
	m_arrCB[(UINT)CB_TYPE::GLOBAL] = pCB;

	return S_OK;
}

int CDevice::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC Desc = {};

	// Cull Back (기본값)
	m_RSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	// Cull Front 
	Desc.CullMode = D3D11_CULL_FRONT;
	Desc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

	// Cull None
	Desc.CullMode = D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

	// Wire Frame
	Desc.CullMode = D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_WIREFRAME;
	DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

	return S_OK;
}

int CDevice::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC Desc = {};

	// Less : 더 작은 깊이가 통과
	m_DSState[(UINT)DS_TYPE::LESS] = nullptr;


	// Less Equal
	Desc.DepthEnable = true;    // 깊이판정을 진행
	Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;   // 깊이 판정 방식
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // 깊이판정을 성공시 깊이 기록여부
	Desc.StencilEnable = false;

	if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf())))
	{
		return E_FAIL;
	}

	// Greater
	Desc.DepthEnable = true;    // 깊이판정을 진행
	Desc.DepthFunc = D3D11_COMPARISON_GREATER;   // 깊이 판정 방식
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // 깊이판정을 성공시 깊이 기록여부
	Desc.StencilEnable = false;

	if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER].GetAddressOf())))
	{
		return E_FAIL;
	}

	// NO_TEST
	Desc.DepthEnable = true;    // 깊이판정을 진행
	Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;   // 깊이 판정 방식
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // 깊이판정을 성공시 깊이 기록여부
	Desc.StencilEnable = false;

	if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST].GetAddressOf())))
	{
		return E_FAIL;
	}

	// NO_WRITE
	Desc.DepthEnable = true;                            // 깊이판정을 진행
	Desc.DepthFunc = D3D11_COMPARISON_LESS;             // 깊이 판정 방식
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;   // 깊이판정을 성공시 깊이 기록여부
	Desc.StencilEnable = false;

	if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_WRITE].GetAddressOf())))
	{
		return E_FAIL;
	}

	// NO_TEST_NO_WRITE
	Desc.DepthEnable = true;    // 깊이판정을 진행
	Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;   // 깊이 판정 방식
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;   // 깊이판정을 성공시 깊이 기록여부
	Desc.StencilEnable = false;

	if (FAILED(DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf())))
	{
		return E_FAIL;
	}


	return S_OK;
}

int CDevice::CreateBlendState()
{
	D3D11_BLEND_DESC Desc = {};

	// Default
	m_BSState[(UINT)BS_TYPE::DEFAULT] = nullptr;


	// AlphaBlend - Coverage
	// AlphaToCoverageEnable가 true면, 알파값이 0.5이하인 부분을 렌더링 할 때, 무시한다. -> 경계가 뚜렷한 캐릭터같은 텍스쳐를 렌더링할 때 주로 사용
	// 이걸 쓰지 않고 그냥 AlphaBlend를 하면 앞에 보이는 텍스쳐의 투명한 부분에 뒤에 그려지는 물체들이 가려지는 현상이 발생.
	Desc.AlphaToCoverageEnable = true;	
	Desc.IndependentBlendEnable = false;

	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Src(Pixel RGB) * A     +      Dest(RenderTarget RGB) * (1 - A)
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // 계수
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 계수

	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	if (FAILED(DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHABLEND_COVERAGE].GetAddressOf())))
	{
		return E_FAIL;
	}


	// AlphaBlend
	Desc.AlphaToCoverageEnable = false;
	Desc.IndependentBlendEnable = false;

	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Src(Pixel RGB) * A     +      Dest(RenderTarget RGB) * (1 - A)
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // 계수
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 계수

	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	if (FAILED(DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHABLEND].GetAddressOf())))
	{
		return E_FAIL;
	}


	// One - One Blend
	Desc.AlphaToCoverageEnable = false;
	Desc.IndependentBlendEnable = false;

	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Src(Pixel RGB) * 1  +  Dest(RenderTarget RGB) * 1
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE; // 계수
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; // 계수

	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	if (FAILED(DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ONE_ONE].GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CDevice::CreateSamplerState()
{
	D3D11_SAMPLER_DESC Desc = {};

	// D3D11_TEXTURE_ADDRESS_WRAP 는 uv 0~1 반복, D3D11_TEXTURE_ADDRESS_MIRROR는 대칭
	Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.Filter = D3D11_FILTER_ANISOTROPIC; // 이방성 필터링

	if (FAILED(DEVICE->CreateSamplerState(&Desc, m_Sampler[0].GetAddressOf())))
	{
		return E_FAIL;
	}

	Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 포인트 필터링

	if (FAILED(DEVICE->CreateSamplerState(&Desc, m_Sampler[1].GetAddressOf())))
	{
		return E_FAIL;
	}

	CONTEXT->VSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->HSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->DSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->GSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->PSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
	CONTEXT->CSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());

	CONTEXT->VSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->HSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->DSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->GSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->PSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
	CONTEXT->CSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());

	return S_OK;
}