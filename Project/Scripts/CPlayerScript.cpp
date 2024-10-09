#include "pch.h"
#include "CPlayerScript.h"

#include "CMissileScript.h"

CPlayerScript::CPlayerScript() :
	CScript(UINT(SCRIPT_TYPE::PLAYERSCRIPT)),
	m_Speed(400.f)
{
	AddScriptParam(SCRIPT_PARAM_TYPE::FLOAT, "PlayerSpeed", &m_Speed);
	AddScriptParam(SCRIPT_PARAM_TYPE::TEXTURE, "Test", &m_Texture);
	AddScriptParam(SCRIPT_PARAM_TYPE::PREFAB, "Missile", &m_MissilePref);
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Begin()
{
	// 2D 파트에서는 최적화하지 않을 것이므로 오브젝트 시작 시 모든 객체가 DynamicMaterial을 각자 들고 있어도 된다.
	GetRenderComponent()->GetDynamicMaterial();

	//m_MissilePref = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"MissilePref");
}

void CPlayerScript::Tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::LEFT))
		vPos.x -= DT * m_Speed;
	if (KEY_PRESSED(KEY::RIGHT))
		vPos.x += DT * m_Speed;
	if (KEY_PRESSED(KEY::UP))
		vPos.y += DT * m_Speed;
	if (KEY_PRESSED(KEY::DOWN))
		vPos.y -= DT * m_Speed;


	if (KEY_PRESSED(KEY::Z))
	{
		//MeshRender()->GetMaterial()->SetScalarParam(INT_0, 1);
		Vec3 vRot = Transform()->GetRelativeRoatation();
		vRot.z += DT * XM_PI;
		Transform()->SetRelativeRotation(vRot);

		/*	Vec3 vScale = Transform()->GetRelativeScale();
			vScale += DT * 100.f * Vec3(1.f, 1.f, 1.f);
			Transform()->SetRelativeScale(vScale);*/
	}
	else
	{
		//MeshRender()->GetMaterial()->SetScalarParam(INT_0, 0);
	}

	if (KEY_JUST_PRESSED(KEY::SPACE))
	{
		// 미사일 발사
		if (nullptr != m_MissilePref)
		{
			Instantiate(m_MissilePref, 5, Transform()->GetWorldPos(), L"Missile");
		}
	}

	Transform()->SetRelativePos(vPos);
}

void CPlayerScript::BeginOverlap(CCollider2D* _OwnCollider, CGameObject* _OtherObject, CCollider2D* _OtherCollider)
{
	DeleteObject(_OtherObject);

	Vec3 vScale = Transform()->GetRelativeScale();

	vScale += Vec3(10.f, 10.f, 0.f);
	Collider2D()->SetScale(Collider2D()->GetScale() + Vec3(0.1f, 0.1f, 0.f));

	Transform()->SetRelativeScale(vScale);
}

void CPlayerScript::SaveToFile(FILE* _File)
{
	fwrite(&m_Speed, sizeof(float), 1, _File);
	SaveAssetRef(m_Texture, _File);
	SaveAssetRef(m_MissilePref, _File);
}

void CPlayerScript::LoadFromFile(FILE* _File)
{
	fread(&m_Speed, sizeof(float), 1, _File);
	LoadAssetRef(m_Texture, _File);
	LoadAssetRef(m_MissilePref, _File);
}