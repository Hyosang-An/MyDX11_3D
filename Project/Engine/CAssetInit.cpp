#include "pch.h"
#include "CAssetMgr.h"
#include "CDevice.h"

#include "CParticleTickCS.h"

void CAssetMgr::Init()
{

	CreateEngineMesh();

	CreateEngineTexture();

	CreateEngineSprite();

	CreateEngineGraphicShader();

	CreateEngineComputeShader();

	CreateEngineMaterial();
}

void CAssetMgr::CreateEngineMesh()
{
	Ptr<CMesh> pMesh = nullptr;
	Vtx v;

	// PointMesh
	pMesh = new CMesh;
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);

	UINT i = 0;
	pMesh->Create(&v, 1, &i, 1);
	AddAsset(L"PointMesh", pMesh);


	// RectMesh 생성	
	// 0 -- 1
	// | \  |
	// 3 -- 2
	Vtx arrVtx[4] = {};

	arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrVtx[0].vUV = Vec2(0.f, 0.f);

	arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	arrVtx[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrVtx[1].vUV = Vec2(1.f, 0.f);

	arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	arrVtx[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrVtx[2].vUV = Vec2(1.f, 1.f);

	arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	arrVtx[3].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrVtx[3].vUV = Vec2(0.f, 1.f);


	// Index 버퍼 생성
	UINT arrIdx[6] = {};
	arrIdx[0] = 0;	arrIdx[1] = 1;	arrIdx[2] = 2;
	arrIdx[3] = 0; 	arrIdx[4] = 2;	arrIdx[5] = 3;

	pMesh = new CMesh;
	pMesh->Create(arrVtx, 4, arrIdx, 6);
	AddAsset(L"RectMesh", pMesh);

	// RectMesh_Debug
	arrIdx[0] = 0; arrIdx[1] = 1; arrIdx[2] = 2; arrIdx[3] = 3; arrIdx[4] = 0;
	pMesh = new CMesh;
	pMesh->Create(arrVtx, 4, arrIdx, 5);
	AddAsset(L"RectMesh_Debug", pMesh);

	// CircleMesh 
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;


	int Slice = 40;
	float fTheta = XM_2PI / Slice;
	float Radius = 0.5f;  // 지름의 길이가 1이 되도록

	// 중심점
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.5f, 0.5f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	// 테두리
	float Angle = 0.f;
	for (int i = 0; i < Slice + 1; ++i)
	{
		v.vPos = Vec3(Radius * cosf(Angle), Radius * sinf(Angle), 0.f);
		v.vUV = Vec2(v.vPos.x + 0.5f, -(v.vPos.y - 0.5f));
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);

		Angle += fTheta;
	}

	// 인덱스
	for (int i = 0; i < Slice; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CircleMesh", pMesh);

	// CircleMesh_Debug
	vecIdx.clear();
	for (size_t i = 1; i < vecVtx.size(); ++i)
	{
		vecIdx.push_back((UINT)i);
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CircleMesh_Debug", pMesh);
}

void CAssetMgr::CreateEngineTexture()
{
	// PostProcess 용도 텍스쳐 생성 -> RenderMgr로 옮겼음
	Vec2 Resolution = CDevice::GetInst()->GetResolution();
	for (int i = 0; i < m_PostProcessTextCnt; i++)
	{
		//// PostProcess 용도 텍스쳐 생성
		wstring texName = L"PostProcessRTTex_" + std::to_wstring(i);
		CAssetMgr::CreateTexture(texName, (UINT)Resolution.x, (UINT)Resolution.y
			, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);
	}

	// PostProcess 용도 DepthStencil 텍스쳐 생성
	CreateTexture(L"PostProcessDSTex", (UINT)Resolution.x, (UINT)Resolution.y
		, DXGI_FORMAT_R24_UNORM_X8_TYPELESS , D3D11_BIND_SHADER_RESOURCE);



	// Noise Texture
	Load<CTexture>(L"texture\\noise\\noise_01.png", L"texture\\noise\\noise_01.png");
	Load<CTexture>(L"texture\\noise\\noise_02.png", L"texture\\noise\\noise_02.png");
	Load<CTexture>(L"texture\\noise\\noise_03.jpg", L"texture\\noise\\noise_03.jpg");
}

void CAssetMgr::CreateEngineSprite()
{
	//wstring strContentsPath = CPathMgr::GetInst()->GetContentPath();

	//Ptr<CTexture> pAtlasTex = Load<CTexture>(L"texture\\link_3.png", L"texture\\link.png");


	//// Sprite 생성 및 저장	================================================
	//Ptr<CSprite> pSprite = nullptr;
	//for (int i = 0; i < 10; ++i)
	//{
	//	//wchar_t szKey[50] = {};
	//	//swprintf_s(szKey, 50, L"Link_MoveDown_%d", i);

	//	wstring szKey = L"Link_MoveDown_" + std::to_wstring(i);

	//	pSprite = new CSprite;
	//	pSprite->Create(pAtlasTex, Vec2((float)i * 120.f, 520.f), Vec2(120.f, 130.f));
	//	pSprite->SetBackgroundPixelSize(Vec2(200.f, 200.f));

	//	pSprite->SetRelativePath(wstring(L"Animation\\") + szKey + L".sprite");
	//	pSprite->Save(strContentsPath + L"Animation\\" + szKey + L".sprite");

	//	AddAsset(szKey, pSprite);
	//}

	//// FlipBook 생성 및 저장	================================================
	//Ptr<CFlipBook> pFlipBook = nullptr;
	//pFlipBook = new CFlipBook;
	//for (int i = 0; i < 10; ++i)
	//{
	//	//wchar_t szKey[50] = {};
	//	//swprintf_s(szKey, 50, L"Link_MoveDown_%d", i);

	//	wstring szKey = L"Link_MoveDown_" + std::to_wstring(i);
	//	pFlipBook->AddSprite(FindAsset<CSprite>(szKey));
	//}

	//AddAsset(L"Link_MoveDown", pFlipBook);
	//pFlipBook->Save(strContentsPath + L"Animation\\" + L"Link_MoveDown" + L".flip");






	//Ptr<CSprite> pSprite = nullptr;

	//for (int i = 0; i < 10; ++i)
	//{
	//	wchar_t Buffer[50] = {};
	//	swprintf_s(Buffer, 50, L"Link_MoveDown_%d", i);

	//	pSprite = Load<CSprite>(Buffer, wstring(L"Animation\\") + Buffer + L".sprite");		

	//	pSprite->SetRelativePath(wstring(L"Animation\\") + Buffer + L".sprite");
	//	pSprite->Save(strContentsPath + L"Animation\\" + Buffer + L".sprite");
	//}


	//Ptr<CFlipBook> pFilpBook = new CFlipBook;

	//for (int i = 0; i < 10; ++i)
	//{
	//	wchar_t Buffer[50] = {};
	//	swprintf_s(Buffer, 50, L"Link_MoveDown_%d", i);
	//	pFilpBook->AddSprite(FindAsset<CSprite>(Buffer));		
	//}

	//AddAsset(L"Link_MoveDown", pFilpBook);
	//pFilpBook->Save(strContentsPath + L"Animation\\" + L"Link_MoveDown" + L".flip");



	// 저장된 FlipBook 불러오기	================================================
	/*Ptr<CFlipBook> pFilpBook = new CFlipBook;
	pFilpBook->Load(strContentsPath + L"Animation\\" + L"Link_MoveDown" + L".flip");
	AddAsset(L"Link_MoveDown", pFilpBook);*/


	//auto flipbook = Load<CFlipBook>(L"Link_MoveDown", L"Animation\\Link_MoveDown.flip");

	//flipbook->Save(L"Test\\flipbook_Save_Test.flip");
}

void CAssetMgr::CreateEngineGraphicShader()
{
	Ptr<CGraphicShader> pShader = nullptr;

	// Std2DShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASKED);

	pShader->AddTexParam(TEX_0, "OutputTexture");

	AddAsset(L"Std2DShader", pShader);


	// Std2DAlphaBlend
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_Alphablend");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddAsset(L"Std2DAlphaBlendShader", pShader);


	// DebugShapeShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\debug.fx", "VS_DebugShape");
	pShader->CreateGeometryShader(L"shader\\debug.fx", "GS_DebugShape");
	pShader->CreatePixelShader(L"shader\\debug.fx", "PS_DebugShape");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEBUG);

	AddAsset(L"DebugShapeShader", pShader);


	// TileMapShader
	pShader = new CGraphicShader;

	pShader->CreateVertexShader(L"shader\\tilemap.fx", "VS_TileMap");
	pShader->CreatePixelShader(L"shader\\tilemap.fx", "PS_TileMap");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASKED);

	AddAsset(L"TileMapShader", pShader);


	// ParticleShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\particle.fx", "VS_Particle");
	pShader->CreateGeometryShader(L"shader\\particle.fx", "GS_Particle");
	pShader->CreatePixelShader(L"shader\\particle.fx", "PS_Particle");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_PARTICLE);

	AddAsset(L"ParticleRenderShader", pShader);


	// GrayFilterShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_Screen");
	pShader->CreatePixelShader(L"shader\\postprocess_0.fx", "PS_GrayFilter");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE); // 후처리는 항상 모든 픽셀에 깊이 관계없이 적용되어야 하므로.
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"GrayFilterShader", pShader);

	// DistortionShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_Distortion");
	pShader->CreatePixelShader(L"shader\\postprocess_0.fx", "PS_Distortion");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"DistortionShader", pShader);

	// ConvexLensShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_ConvexLens");
	pShader->CreatePixelShader(L"shader\\postprocess_0.fx", "PS_ConvexLens");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"ConvexLensShader", pShader);

	// RippleEffectShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_ScreenZoom");
	pShader->CreatePixelShader(L"shader\\postprocess_0.fx", "PS_RippleEffect");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"RippleEffectShader", pShader);

	// ExtractBrightShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_Screen");
	pShader->CreatePixelShader(L"shader\\postprocess_0.fx", "PS_ExtractBright");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"ExtractBrightShader", pShader);

	// GaussianBlurShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_Screen");
	pShader->CreatePixelShader(L"shader\\postprocess_1.fx", "PS_GaussianBlur");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"GaussianBlurShader", pShader);

	// BloomShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_Screen");
	pShader->CreatePixelShader(L"shader\\postprocess_1.fx", "PS_Bloom");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"BloomShader", pShader);
}

void CAssetMgr::CreateEngineComputeShader()
{
	// ParticleTick
	Ptr<CComputeShader> pCS = nullptr;

	pCS = new CParticleTickCS;
	AddAsset<CComputeShader>(L"ParticleTickCS", pCS);
}

void CAssetMgr::CreateEngineMaterial()
{
	Ptr<CMaterial>	pMtrl = nullptr;

	// Std2DMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DShader"));
	AddAsset(L"Std2DMtrl", pMtrl);

	// Std2DAlphaBlendMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DAlphaBlendShader"));
	AddAsset(L"Std2DAlphaBlendMtrl", pMtrl);

	// DebugShapeMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"DebugShapeShader"));
	AddAsset(L"DebugShapeMtrl", pMtrl);

	// TileMapMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"TileMapShader"));
	AddAsset(L"TileMapMtrl", pMtrl);

	// ParticleRenderMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"ParticleRenderShader"));
	AddAsset(L"ParticleRenderMtrl", pMtrl);

	// GrayFilterMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"GrayFilterShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessRTTex_0"));
	pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"texture\\noise\\noise_01.png"));
	pMtrl->SetTexParam(TEX_2, FindAsset<CTexture>(L"texture\\noise\\noise_02.png"));
	pMtrl->SetTexParam(TEX_3, FindAsset<CTexture>(L"texture\\noise\\noise_03.jpg"));
	AddAsset(L"GrayFilterMtrl", pMtrl);

	// DistortionMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"DistortionShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessRTTex_0"));
	pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"texture\\noise\\noise_01.png"));
	pMtrl->SetTexParam(TEX_2, FindAsset<CTexture>(L"texture\\noise\\noise_02.png"));
	pMtrl->SetTexParam(TEX_3, FindAsset<CTexture>(L"texture\\noise\\noise_03.jpg"));
	AddAsset(L"DistortionMtrl", pMtrl);

	// ConvexLensMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"ConvexLensShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessRTTex_0"));
	pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"PostProcessDSTex"));
	AddAsset(L"ConvexLensMtrl", pMtrl);

	// RippleEffectMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"RippleEffectShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessRTTex_0"));
	pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"PostProcessDSTex"));
	AddAsset(L"RippleEffectMtrl", pMtrl);

	// ExtractBrightMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"ExtractBrightShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessRTTex_0"));
	AddAsset(L"ExtractBrightMtrl", pMtrl);

	// GaussianBlurMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"GaussianBlurShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessRTTex_0"));
	pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"PostProcessRTTex_1"));
	AddAsset(L"GaussianBlurMtrl", pMtrl);

	// GaussianBlurMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"BloomShader"));
	pMtrl->SetTexParam(TEX_0, FindAsset<CTexture>(L"PostProcessRTTex_0"));
	pMtrl->SetTexParam(TEX_1, FindAsset<CTexture>(L"PostProcessRTTex_1"));
	AddAsset(L"BloomMtrl", pMtrl);
}

