#include "pch.h"
#include "CLandScape.h"

#include "CAssetMgr.h"
#include "CKeyMgr.h"

#include "CTransform.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
{
	SetFrustumCheck(false);
	SetFace(m_FaceX, m_FaceZ);

	// Init(); AddComponent���� �̹� ȣ���ϹǷ� �ּ�ó��
}

CLandScape::~CLandScape()
{
}

void CLandScape::Init()
{
	// LandScape ���� Mesh ����
	CreateMesh();

	// LandScape ���� ���� ����
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"LandScapeMtrl");
	SetMaterial(pMtrl);

	// LandScape ���� ��ǻƮ ���̴� ����
	CreateComputeShader();

	// LandScape �� �ؽ��� ���� �� �ε�
	CreateTextureAndStructuredBuffer();

	// BrushTexture �߰�	
	//AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\brush\\TX_GlowScene_2.png"));
	//AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\brush\\TX_HitFlash_0.png"));
	//AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\brush\\TX_HitFlash02.png"));
	//AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\brush\\TX_Twirl02.png"));
	//AddBrushTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\brush\\FX_Flare.png"));
	//m_BrushIdx = 0;
}

void CLandScape::FinalTick()
{
	// �귯�� �ٲٱ�
	if (KEY_JUST_PRESSED(KEY::NUM7))
	{
		assert(m_BrushIdx != -1);

		++m_BrushIdx;
		if (m_vecBrush.size() <= m_BrushIdx)
			m_BrushIdx = 0;
	}

	if (m_IsHeightMapCreated && KEY_PRESSED(KEY::LBTN))
	{
		// ���̸� ����
		m_HeightMapCS->SetBrushPos(Vec2(0.5f, 0.5f));
		m_HeightMapCS->SetBrushScale(m_BrushScale);
		m_HeightMapCS->SetHeightMap(m_HeightMap);

		if (m_BrushIdx != -1)
			m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
		m_HeightMapCS->Execute();
	}
}

void CLandScape::Render()
{
	Transform()->Binding();

	//GetMaterial()->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);

	// ������ �� ����
	GetMaterial()->SetScalarParam(INT_0, m_FaceX);
	GetMaterial()->SetScalarParam(INT_1, m_FaceZ);

	// ������ �����ų ���̸�
	GetMaterial()->SetTexParam(TEX_0, m_HeightMap);

	// ���� ���ε�
	GetMaterial()->Binding();

	// ������
	GetMesh()->Render();
}

void CLandScape::SetFace(int _X, int _Z)
{
	m_FaceX = _X;
	m_FaceZ = _Z;

	CreateMesh();
	SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"LandScapeMtrl"));
}

void CLandScape::CreateHeightMap(UINT _Width, UINT _Height)
{
	m_IsHeightMapCreated = true;

	m_HeightMap = CAssetMgr::GetInst()->CreateTexture(L"LandScapeHeightMap", _Width, _Height
		, DXGI_FORMAT_R32_FLOAT
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
}

void CLandScape::CreateMesh()
{
	Vtx v;
	vector<Vtx> vecVtx;

	// ����
	for (UINT Row = 0; Row < m_FaceZ + 1; ++Row)
	{
		for (UINT Col = 0; Col < m_FaceX + 1; ++Col)
		{
			v.vPos = Vec3((float)Col, 0.f, (float)Row);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);

			vecVtx.push_back(v);
		}
	}

	// �ε���
	vector<UINT> vecIdx;
	for (UINT Row = 0; Row < m_FaceZ; ++Row)
	{
		for (UINT Col = 0; Col < m_FaceX; ++Col)
		{
			// 0
			// | \
			// 2--1
			vecIdx.push_back((Row * (m_FaceX + 1)) + Col + m_FaceX + 1);
			vecIdx.push_back((Row * (m_FaceX + 1)) + Col + 1);
			vecIdx.push_back((Row * (m_FaceX + 1)) + Col);

			// 1--2 
			//  \ |
			//    0
			vecIdx.push_back((Row * (m_FaceX + 1)) + Col + 1);
			vecIdx.push_back((Row * (m_FaceX + 1)) + Col + m_FaceX + 1);
			vecIdx.push_back((Row * (m_FaceX + 1)) + Col + m_FaceX + 1 + 1);
		}
	}

	Ptr<CMesh> pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	SetMesh(pMesh);
}

void CLandScape::CreateComputeShader()
{
	// HeightMapCS �� ������ ã�ƿ��� ������ �������ؼ� ����Ѵ�.
	m_HeightMapCS = (CHeightMapCS*)CAssetMgr::GetInst()->FindAsset<CComputeShader>(L"HeightMapCS").Get();
	if (nullptr == m_HeightMapCS)
	{
		m_HeightMapCS = new CHeightMapCS;
		CAssetMgr::GetInst()->AddAsset<CComputeShader>(L"HeightMapCS", m_HeightMapCS.Get());
	}
}

void CLandScape::CreateTextureAndStructuredBuffer()
{
}



void CLandScape::SaveToFile(FILE* _File)
{
}

void CLandScape::LoadFromFile(FILE* _File)
{
}



