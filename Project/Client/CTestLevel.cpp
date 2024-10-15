#include "pch.h"
#include "CTestLevel.h"

#include <Engine/CAssetMgr.h>
#include <Engine/assets.h>

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/components.h>

#include <Engine/CCollisionMgr.h>

#include <Scripts/CPlayerScript.h>
#include <Scripts/CMissileScript.h>
#include <Scripts/CCameraMoveScript.h>

#include "CLevelSaveLoad.h"

#include <Engine/CSetColorCS.h>

#include <Engine/CStructuredBuffer.h>

void CTestLevel::CreateTestLevel()
{
	//// Sound 재생
	//Ptr<CSound> pSound = CAssetMgr::GetInst()->FindAsset<CSound>(L"sound\\BGM_title_screen.wav");
	//pSound->Play(0, 1.f, false);
// 
	// Material
	Ptr<CMaterial> pMtrl = CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std3DMtrl");

	// Level 생성
	CLevel* TestLevel = new CLevel;


	// 카메라 오브젝트
	CGameObject* CamObj = new CGameObject;
	CamObj->SetName(L"MainCamera");
	CamObj->AddComponent(new CTransform);
	CamObj->AddComponent(new CCamera);
	CamObj->AddComponent(new CCameraMoveScript);

	CamObj->Transform()->SetRelativePos(0.f, 0.f, -500.f);

	// 우선순위를 0 : MainCamera 로 설정
	CamObj->Camera()->SetPriority(0);

	// 카메라 레이어 설정 (31번 레이어 제외 모든 레이어를 촬영)
	CamObj->Camera()->SetLayerAll();
	CamObj->Camera()->SetLayer(31, false);
	CamObj->Camera()->SetFar(100000.f);
	CamObj->Camera()->SetProjType(PERSPECTIVE);

	TestLevel->AddObject(0, CamObj);

	CGameObject* pObject = nullptr;

	//// 광원 오브젝트 추가
	//pObject = new CGameObject;
	//pObject->SetName(L"PointLight 1");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CLight2D);

	//pObject->Light2D()->SetLightType(LIGHT_TYPE::POINT);
	//pObject->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//pObject->Light2D()->SetRadius(1000);
	//pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));

	//TestLevel->AddObject(0, pObject);


	// 3D Directional Light
	pObject = new CGameObject;
	pObject->SetName(L"Directional Light");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight3D);

	pObject->Transform()->SetRelativePos(-300.f, 0.f, 0.f);
	pObject->Transform()->SetRelativeRotation(XM_PI / 4.f, XM_PI / 4.f, 0.f);

	pObject->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pObject->Light3D()->SetLightColor(Vec3(0.9f, 0.9f, 0.9f));
	pObject->Light3D()->SetLightAmbient(Vec3(0.1f, 0.1f, 0.1f));
	pObject->Light3D()->SetSpecularCoefficient(0.5f);

	TestLevel->AddObject(0, pObject);

	//// 3D Point Light 1
	//pObject = new CGameObject;
	//pObject->SetName(L"Point Light 1");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CLight3D);

	//pObject->Transform()->SetRelativePos(-300.f, 0.f, 0.f);
	//pObject->Transform()->SetRelativeRotation(XM_PI / 4.f, XM_PI / 4.f, 0.f);

	//pObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	//pObject->Light3D()->SetLightColor(Vec3(1.f, 0.5f, 0.5f));
	//pObject->Light3D()->SetLightAmbient(Vec3(0.f, 0.f, 0.f));
	//pObject->Light3D()->SetSpecularCoefficient(0.3f);
	//pObject->Light3D()->SetRadius(800.f);

	//TestLevel->AddObject(0, pObject);

	//// 3D Point Light 2
	//pObject = new CGameObject;
	//pObject->SetName(L"Point Light 2");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CLight3D);

	//pObject->Transform()->SetRelativePos(300.f, 0.f, 0.f);
	//pObject->Transform()->SetRelativeRotation(XM_PI / 4.f, XM_PI / 4.f, 0.f);

	//pObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	//pObject->Light3D()->SetLightColor(Vec3(0.5f, 0.5f, 1.f));
	//pObject->Light3D()->SetLightAmbient(Vec3(0.f, 0.f, 0.f));
	//pObject->Light3D()->SetSpecularCoefficient(0.3f);
	//pObject->Light3D()->SetRadius(800.f);

	//TestLevel->AddObject(0, pObject);

	//// 3D Spot Light
	//pObject = new CGameObject;
	//pObject->SetName(L"Spot Light");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CLight3D);

	//pObject->Transform()->SetRelativePos(0.f, 500.f, 0.f);
	//pObject->Transform()->SetRelativeRotation(0.5f * XM_PI, 0.f, 0.f);

	//pObject->Light3D()->SetLightType(LIGHT_TYPE::SPOT);
	//pObject->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//pObject->Light3D()->SetLightAmbient(Vec3(0.f, 0.f, 0.f));
	//pObject->Light3D()->SetSpecularCoefficient(0.5f);
	//pObject->Light3D()->SetAngle(15.f * XM_PI / 180);
	//pObject->Light3D()->SetRadius(2000.f);

	//TestLevel->AddObject(0, pObject);


	//// 플레이어 오브젝트
	//CGameObject* pPlayer = new CGameObject;
	//pPlayer->SetName(L"Player");
	//pPlayer->AddComponent(new CTransform);
	//pPlayer->AddComponent(new CMeshRender);

	//pPlayer->Transform()->SetRelativePos(0.f, 0.f, 100.f);
	//pPlayer->Transform()->SetRelativeScale(500.f, 500.f, 500.f);

	//pPlayer->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	//pPlayer->MeshRender()->SetMaterial(pMtrl);

	//TestLevel->AddObject(3, pPlayer);

	// 플레이어 오브젝트 2
	CGameObject* pPlayer = new CGameObject;
	pPlayer->SetName(L"Player 2");
	pPlayer->AddComponent(new CTransform);
	pPlayer->AddComponent(new CMeshRender);

	pPlayer->Transform()->SetRelativePos(0.f, -500.f, 0.f);
	pPlayer->Transform()->SetRelativeScale(1000.f, 1000.f, 1.f);
	pPlayer->Transform()->SetRelativeRotation(XM_PI / 2.f, 0.f, 0.f);

	pPlayer->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pPlayer->MeshRender()->SetMaterial(pMtrl);

	Ptr<CTexture> pTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\LandScapeTexture\\gl1_ground_II_albedo.TGA");
	Ptr<CTexture> pNTex = CAssetMgr::GetInst()->FindAsset<CTexture>(L"texture\\LandScapeTexture\\gl1_ground_II_normal.TGA");

	pPlayer->MeshRender()->GetMaterial()->SetTexParam(TEX_0, pTex);
	//pPlayer->MeshRender()->GetMaterial()->SetTexParam(TEX_1, pNTex);

	TestLevel->AddObject(3, pPlayer);



	ChangeLevel(TestLevel, LEVEL_STATE::STOP);

	// 충돌 지정
	CCollisionMgr::GetInst()->CollisionCheck(3, 4); // Player vs Monster
	CCollisionMgr::GetInst()->CollisionCheck(5, 4); // Player Projectile vs Monster
}

void CTestLevel::CreatePrefab()
{
	//CGameObject* pProto = new CGameObject;

	//pProto->AddComponent(new CTransform);
	//pProto->AddComponent(new CMeshRender);
	//pProto->AddComponent(new CMissileScript);

	//pProto->Transform()->SetRelativeScale(100.f, 100.f, 1.f);

	//pProto->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pProto->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl"));

	//Ptr<CPrefab> pPrefab = new CPrefab;
	//pPrefab->SetProtoObject(pProto);

	//CAssetMgr::GetInst()->AddAsset<CPrefab>(L"MissilePref", pPrefab);




	//wstring FilePath = CPathMgr::GetInst()->GetContentPath();
	//FilePath += L"prefab\\Missile.pref";
	//pPrefab->Save(FilePath);
}
