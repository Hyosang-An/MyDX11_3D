#include "pch.h"
#include "CAssetMgr.h"
#include "CDevice.h"

#include "CParticleTickCS.h"
#include "CAnimation3DShader.h"

void CAssetMgr::Init()
{
	CreateEngineMesh();

	CreateEngineTexture();

	CreateEngineSprite();

	CreateEngineGraphicShader();

	CreateEngineComputeShader();

	CreateEngineMaterial();

	// content 폴더에 있는 리소스(에셋) 들을 확인 및 로딩 (content 에셋들은 엔진 에셋에 종속적이므로 엔진 에셋들을 먼저 생성 후 로딩)
	GetContentFiles();
}

void CAssetMgr::GetContentFiles()
{
	// Content 폴더에 있는 에셋파일들의 경로를 전부 알아낸다.
	wstring ContentsPath = CPathMgr::GetInst()->GetContentPath();
	FindAssetName(ContentsPath, L"*.*");

	// 알아낸 에셋 파일들의 경로를 통해서 Asset 들을 상대경로를 Key값으로 AssetMgr 에 로딩한다.
	for (size_t i = 0; i < m_vecAssetRelativePath.size(); ++i)
	{
		LoadAsset(m_vecAssetRelativePath[i]);
	}


	// 에셋 매니저에는 로딩되어있지만, content 폴더에는 없는 에셋은 AssetMgr 에서 삭제하기
	// 로딩된 에셋에 해당하는 원본 파일이 Content 폴더에 있는지 Exist 체크
	wstring strContentPath = CPathMgr::GetInst()->GetContentPath();

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; ++i)
	{
		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetMgr::GetInst()->GetAssetMap((ASSET_TYPE)i);
		for (const auto& pair : mapAsset)
		{
			// 엔진에서 제작한 에셋은 원래 원본파일이 없기때문에 넘어간다.
			if (pair.second->IsEngineAsset())
				continue;


			wstring strRelativePath = pair.second->GetRelativePath();

			// strRelativePath가 비어있으면 파일로부터 로드된게 아니란 뜻
			if (strRelativePath != L"" && false == std::filesystem::exists(strContentPath + strRelativePath))
			{
				// AssetMgr에서만 들고 있는 경우
				if (pair.second->GetRefCount() <= 1)
				{
					// // AssetMgr에서 삭제 요청
					tTask deleteAssetTask{ TASK_TYPE::DEL_ASSET, (DWORD_PTR)pair.second.Get() };
					CTaskMgr::GetInst()->AddTask(deleteAssetTask);
				}

				// 다른 곳에서도 참조하고 있는 경우
				else
				{
					wstring msg = pair.second->GetName() + L"다른 곳에서 참조되고 있을 수 있습니다.";
					int ret = MessageBox(nullptr, msg.c_str(), L"에셋 삭제 경고", MB_YESNO);
					if (ret == IDYES)
					{

						CTaskMgr::GetInst()->AddTask(tTask{ TASK_TYPE::DEL_ASSET, (DWORD_PTR)pair.second.Get(), });
					}
				}
			}
		}
	}
}

void CAssetMgr::CreateEngineMesh()
{
	Ptr<CMesh> pMesh = nullptr;
	Vtx v;

	// ============
	// PointMesh
	// ============
	pMesh = new CMesh;
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);

	UINT i = 0;
	pMesh->Create(&v, 1, &i, 1);
	AddAsset(L"PointMesh", pMesh);


	// ============
	// RectMesh 생성	
	// ============
	// 0 -- 1
	// | \  |
	// 3 -- 2
	Vtx arrVtx[4] = {};

	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);
	arrVtx[0] = v;

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	arrVtx[1] = v;

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	arrVtx[2] = v;

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	arrVtx[3] = v;


	// Index 버퍼 생성
	UINT arrIdx[6] = {};
	arrIdx[0] = 2;	arrIdx[1] = 0;	arrIdx[2] = 1;
	arrIdx[3] = 0; 	arrIdx[4] = 2;	arrIdx[5] = 3;

	pMesh = new CMesh;
	pMesh->Create(arrVtx, 4, arrIdx, 6);
	AddAsset(L"RectMesh", pMesh);

	// ============
	// RectMesh_Debug
	// ============
	arrIdx[0] = 0; arrIdx[1] = 1; arrIdx[2] = 2; arrIdx[3] = 3; arrIdx[4] = 0;
	pMesh = new CMesh;
	pMesh->Create(arrVtx, 4, arrIdx, 5);
	AddAsset(L"RectMesh_Debug", pMesh);

	// ============
	// CircleMesh 
	// ============
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

	vecIdx.clear();

	// ============
	// CircleMesh_Debug
	// ============
	for (size_t i = 1; i < vecVtx.size(); ++i)
	{
		vecIdx.push_back((UINT)i);
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CircleMesh_Debug", pMesh);

	vecIdx.clear();

	// ============
	// CubeMesh
	// ============
	// 24개의 정점이 필요
	// 평면 하나당 정점 4개 x 6면 = 24개
	Vtx arrCube[24] = {};

	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV = Vec2(0.f, 0.f);
	arrCube[0].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[0].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV = Vec2(1.f, 0.f);
	arrCube[1].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[1].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV = Vec2(1.f, 1.f);
	arrCube[2].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[2].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV = Vec2(0.f, 1.f);
	arrCube[3].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[3].vBinormal = Vec3(0.f, 0.f, -1.f);


	// 아랫 면	
	arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUV = Vec2(0.f, 0.f);
	arrCube[4].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[4].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUV = Vec2(1.f, 0.f);
	arrCube[5].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[5].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUV = Vec2(1.f, 1.f);
	arrCube[6].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[6].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUV = Vec2(0.f, 1.f);
	arrCube[7].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[7].vBinormal = Vec3(0.f, 0.f, 1.f);

	// 왼쪽 면
	arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV = Vec2(0.f, 0.f);
	arrCube[8].vTangent = Vec3(0.f, 0.f, -1.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[8].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV = Vec2(1.f, 0.f);
	arrCube[9].vTangent = Vec3(0.f, 0.f, -1.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[9].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV = Vec2(1.f, 1.f);
	arrCube[10].vTangent = Vec3(0.f, 0.f, -1.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[10].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV = Vec2(0.f, 1.f);
	arrCube[11].vTangent = Vec3(0.f, 0.f, -1.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[11].vBinormal = Vec3(0.f, -1.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV = Vec2(0.f, 0.f);
	arrCube[12].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[12].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV = Vec2(1.f, 0.f);
	arrCube[13].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[13].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV = Vec2(1.f, 1.f);
	arrCube[14].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[14].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV = Vec2(0.f, 1.f);
	arrCube[15].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[15].vBinormal = Vec3(0.f, -1.f, 0.f);

	// 뒷 면
	arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUV = Vec2(0.f, 0.f);
	arrCube[16].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[16].vBinormal = Vec3(0.f, -1.f, 1.f);

	arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUV = Vec2(1.f, 0.f);
	arrCube[17].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[17].vBinormal = Vec3(0.f, -1.f, 1.f);

	arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUV = Vec2(1.f, 1.f);
	arrCube[18].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[18].vBinormal = Vec3(0.f, -1.f, 1.f);

	arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUV = Vec2(0.f, 1.f);
	arrCube[19].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[19].vBinormal = Vec3(0.f, -1.f, 1.f);

	// 앞 면
	arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUV = Vec2(0.f, 0.f);
	arrCube[20].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[20].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUV = Vec2(1.f, 0.f);
	arrCube[21].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[21].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUV = Vec2(1.f, 1.f);
	arrCube[22].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[22].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUV = Vec2(0.f, 1.f);
	arrCube[23].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[23].vBinormal = Vec3(0.f, -1.f, 0.f);

	// 인덱스
	//for (int i = 0; i < 12; i += 2)
	//{
	//	vecIdx.push_back(i * 2);
	//	vecIdx.push_back(i * 2 + 1);
	//	vecIdx.push_back(i * 2 + 2);

	//	vecIdx.push_back(i * 2);
	//	vecIdx.push_back(i * 2 + 2);
	//	vecIdx.push_back(i * 2 + 3);
	//}

	for (int i = 0; i < 6; ++i)
	{
		vecIdx.push_back(i * 4);
		vecIdx.push_back(i * 4 + 1);
		vecIdx.push_back(i * 4 + 2);

		vecIdx.push_back(i * 4);
		vecIdx.push_back(i * 4 + 2);
		vecIdx.push_back(i * 4 + 3);
	}

	pMesh = new CMesh;
	pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CubeMesh", pMesh);

	vecIdx.clear();


	// ============
	// CubeMesh_Debug
	// ============
	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2); vecIdx.push_back(3); vecIdx.push_back(4);
	vecIdx.push_back(7); vecIdx.push_back(6); vecIdx.push_back(5); vecIdx.push_back(4); vecIdx.push_back(3);
	vecIdx.push_back(0); vecIdx.push_back(7); vecIdx.push_back(6); vecIdx.push_back(1); vecIdx.push_back(2);
	vecIdx.push_back(5);

	pMesh = new CMesh();
	pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CubeMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();


	// ============
	// Sphere Mesh
	// ============
	float fRadius = 0.5f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// Body
	UINT iStackCount = 400; // 가로 분할 개수
	UINT iSliceCount = 400; // 세로 분할 개수

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));

			v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent.Normalize();

			v.vNormal.Cross(v.vTangent, v.vBinormal);
			v.vBinormal.Normalize();

			vecVtx.push_back(v);
		}
	}

	// Bottom
	v.vPos = Vec3(0.f, -fRadius, 0.f);
	v.vUV = Vec2(0.5f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();

	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// 인덱스
	// 북극점
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 몸통
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	// 남극점
	UINT iBottomIdx = (UINT)vecVtx.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"SphereMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();

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



	Ptr<CTexture> pEffectTarget = CreateTexture(
		L"EffectTargetTex"
		, (UINT)(Resolution.x), (UINT)(Resolution.y)
		, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	Ptr<CTexture> pEffectDepth = CreateTexture(
		L"EffectDepthStencilTex"
		, (UINT)(Resolution.x), (UINT)(Resolution.y)
		, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

	Ptr<CTexture> pEffectBlurTarget = CreateTexture(
		L"EffectBlurTargetTex"
		, (UINT)(Resolution.x), (UINT)(Resolution.y)
		, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);





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


	// Std3DShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\std3d.fx", "VS_Std3D");
	pShader->CreatePixelShader(L"shader\\std3d.fx", "PS_Std3D");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
	pShader->AddTexParam(TEX_0, "Albedo Texture");
	pShader->AddTexParam(TEX_1, "Normal Texture");
	AddAsset(L"Std3DShader", pShader);


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
	//pShader->CreateGeometryShader(L"shader\\debug.fx", "GS_DebugShape");
	pShader->CreatePixelShader(L"shader\\debug.fx", "PS_DebugShape");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEBUG);

	AddAsset(L"DebugShapeShader", pShader);


	// DebugLineShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\debug.fx", "VS_DebugLine");
	pShader->CreateGeometryShader(L"shader\\debug.fx", "GS_DebugLine");
	pShader->CreatePixelShader(L"shader\\debug.fx", "PS_DebugLine");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEBUG);

	AddAsset(L"DebugLineShader", pShader);


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


	// SkyBoxShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\skybox.fx", "VS_SkyBox");
	pShader->CreatePixelShader(L"shader\\skybox.fx", "PS_SkyBox");
	pShader->SetRSType(RS_TYPE::CULL_FRONT);	// SkyBox는 카메라가 안쪽을 보기 때문에 Front로 설정
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);  // NDC좌표계에서 z값이 1이므로 LESS_EQUAL로 설정 (Less로 하면 SkyBox가 안 보임)
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	pShader->AddTexParam(TEX_0, "Albedo Texture");

	AddAsset(L"SkyBoxShader", pShader);


	// Std3D_DeferredShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\std3d_deferred.fx", "VS_Std3D_Deferred");
	pShader->CreatePixelShader(L"shader\\std3d_deferred.fx", "PS_Std3D_Deferred");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);
	pShader->AddTexParam(TEX_0, "Albedo Texture");
	pShader->AddTexParam(TEX_1, "Normal Texture");
	AddAsset(L"Std3D_DeferredShader", pShader);


	// DecalShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\decal.fx", "VS_Decal");
	pShader->CreatePixelShader(L"shader\\decal.fx", "PS_Decal");
	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DECAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DECAL);

	pShader->AddTexParam(TEX_0, "Decal Texture");

	AddAsset(L"DecalShader", pShader);


	// DirLightShadowMap Shader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\shadowmap.fx", "VS_DirLightShadowMap");
	pShader->CreatePixelShader(L"shader\\shadowmap.fx", "PS_DirLightShadowMap");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_SHADOWMAP);
	AddAsset(L"DirLightShadowMap", pShader);


	// Tessellation Test Shader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\tess_test.fx", "VS_Tess");
	pShader->CreateHullShader(L"shader\\tess_test.fx", "HS_Tess");
	pShader->CreateDomainShader(L"shader\\tess_test.fx", "DS_Tess");
	pShader->CreatePixelShader(L"shader\\tess_test.fx", "PS_Tess");

	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	pShader->AddScalarParam(FLOAT_0, "TessFactor");
	pShader->AddScalarParam(INT_0, "TessFactor");

	AddAsset(L"TessTestShader", pShader);


	// LandScape Shader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\landscape.fx", "VS_LandScape");
	pShader->CreateHullShader(L"shader\\landscape.fx", "HS_LandScape");
	pShader->CreateDomainShader(L"shader\\landscape.fx", "DS_LandScape");
	pShader->CreatePixelShader(L"shader\\landscape.fx", "PS_LandScape");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);

	AddAsset(L"LandScapeShader", pShader);


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

	// BlurShader
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_Blur");
	pShader->CreatePixelShader(L"shader\\postprocess_0.fx", "PS_Blur");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"BlurShader", pShader);

	// EffectMerge
	pShader = new CGraphicShader;
	pShader->CreateVertexShader(L"shader\\postprocess_0.fx", "VS_EffectMerge");
	pShader->CreatePixelShader(L"shader\\postprocess_0.fx", "PS_EffectMerge");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddAsset(L"EffectMergeShader", pShader);
}

void CAssetMgr::CreateEngineComputeShader()
{
	// ParticleTick
	Ptr<CComputeShader> pCS = nullptr;

	pCS = new CParticleTickCS;
	AddAsset<CComputeShader>(L"ParticleTickCS", pCS);


	// Animation3D
	pCS = new CAnimation3DShader;
	AddAsset<CComputeShader>(L"Animation3DUpdateCS", pCS);
}

void CAssetMgr::CreateEngineMaterial()
{
	Ptr<CMaterial>	pMtrl = nullptr;

	// Std2DMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std2DShader"));
	AddAsset(L"Std2DMtrl", pMtrl);

	// Std3DMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std3DShader"));
	AddAsset(L"Std3DMtrl", pMtrl);

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

	// SkyBoxMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"SkyBoxShader"));
	AddAsset(L"SkyBoxMtrl", pMtrl);

	// Std3D_DeferredMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"Std3D_DeferredShader"));
	AddAsset(L"Std3D_DeferredMtrl", pMtrl);

	// DecalMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"DecalShader"));
	AddAsset(L"DecalMtrl", pMtrl);

	// DirLightShadowMapMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"DirLightShadowMap"));
	AddAsset(L"DirLightShadowMapMtrl", pMtrl);

	// TessTestMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"TessTestShader"));
	AddAsset(L"TessTestMtrl", pMtrl);

	// LandScapeMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"LandScapeShader"));
	AddAsset(L"LandScapeMtrl", pMtrl);

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


	// BlurMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"BlurShader"));
	AddAsset(L"BlurMtrl", pMtrl);

	// EffectMergeMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindAsset<CGraphicShader>(L"EffectMergeShader"));
	AddAsset(L"EffectMergeMtrl", pMtrl);
}

void CAssetMgr::FindAssetName(const wstring& _FolderPath, const wstring& _Filter)
{
	WIN32_FIND_DATA tFindData = {};

	// 경로에 맞는 파일 및 폴더를 탐색할 수 있는 커널 오브젝트 생성
	wstring strFindPath = _FolderPath + _Filter;
	HANDLE hFinder = FindFirstFile(strFindPath.c_str(), &tFindData);
	if (hFinder == INVALID_HANDLE_VALUE)
	{
		assert(!"해당 경로를 찾을 수 없습니다");
		return;
	}

	do
	{
		wstring strFindName = tFindData.cFileName;

		if (tFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// 폴더면 재귀 호출
			if (strFindName != L"." && strFindName != L"..")
			{
				FindAssetName(_FolderPath + strFindName + L"\\", _Filter);
			}
		}

		// 파일이면 경로 추가
		else
		{
			wstring RelativePath = CPathMgr::GetInst()->GetRelativePath(_FolderPath + strFindName);
			m_vecAssetRelativePath.push_back(RelativePath);
		}
	} while (FindNextFile(hFinder, &tFindData));

	FindClose(hFinder);
}

void CAssetMgr::LoadAsset(const path& _Path)
{
	// 상대경로를 키 값으로 AssetMgr에 등록

	path ext = _Path.extension();

	if (ext == L".mesh")
		CAssetMgr::GetInst()->Load<CMesh>(_Path, _Path);
	else if (ext == L".mdat")
		CAssetMgr::GetInst()->Load<CMeshData>(_Path, _Path);
	else if (ext == L".mtrl")
		CAssetMgr::GetInst()->Load<CMaterial>(_Path, _Path);
	else if (ext == L".prefab")
		CAssetMgr::GetInst()->Load<CPrefab>(_Path, _Path);
	else if (ext == L".png" || ext == L".jpg" || ext == L".jpeg" || ext == L".bmp" || ext == L".dds" || ext == L".tga"
		|| ext == L".PNG" || ext == L".JPG" || ext == L".JPEG" || ext == L".BMP" || ext == L".DDS" || ext == L".TGA")
		CAssetMgr::GetInst()->Load<CTexture>(_Path, _Path);
	else if (ext == L".mp3" || ext == L".mp4" || ext == L".ogg" || ext == L".wav"
		|| ext == L".MP3" || ext == L".MP4" || ext == L".OGG" || ext == L".WAV")
		CAssetMgr::GetInst()->Load<CSound>(_Path, _Path);
	else if (ext == L".sprite")
		CAssetMgr::GetInst()->Load<CSprite>(_Path, _Path);
	else if (ext == L".flip")
		CAssetMgr::GetInst()->Load<CFlipBook>(_Path, _Path);
}