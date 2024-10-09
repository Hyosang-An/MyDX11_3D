#include "pch.h"
#include "CCameraMoveScript.h"

CCameraMoveScript::CCameraMoveScript() :
	CScript((UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT),
	m_CamSpeed(500.f)
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}


void CCameraMoveScript::Tick()
{
	if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
	{
		OrthoGraphicMove();
	}

	else if (PROJ_TYPE::PERSPECTIVE == Camera()->GetProjType())
	{
		PerspectiveMove();
	}


	if (KEY_JUST_PRESSED(KEY::P))
	{
		if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
			Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		else
			Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	}
}

void CCameraMoveScript::OrthoGraphicMove()
{
	float Speed = m_CamSpeed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		Speed *= 3.f;
	}

	Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vPos.y += DT * Speed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos.y -= DT * Speed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos.x -= DT * Speed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos.x += DT * Speed;
	}

	Transform()->SetRelativePos(vPos);
}

void CCameraMoveScript::PerspectiveMove()
{
	float Speed = m_CamSpeed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		Speed *= 3.f;
	}

	Vec3 vFront = Transform()->GetWorldDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetWorldDir(DIR::RIGHT);

	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vPos += vFront * DT * Speed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos -= vFront * DT * Speed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos -= vRight * DT * Speed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos += vRight * DT * Speed;
	}

	Transform()->SetRelativePos(vPos);

	//if (KEY_PRESSED(KEY::RBTN))
	//{
	//	CKeyMgr::GetInst()->MouseCapture(true);

	//	// 마우스가 이동하는 방향
	//	//vMouseDragDelta.x; ==> y축 회전;
	//	//vMouseDragDelta.y; ==> x축 회전
	//	Vec2 vMouseDragDelta = CKeyMgr::GetInst()->GetMouseDragDelta();

	//	Vec3 vRot = Transform()->GetRelativeRoatation();
	//	vRot.y += vMouseDragDelta.x * XM_PI * DT * 10.f;
	//	vRot.x += vMouseDragDelta.y * XM_PI * DT * 10.f;
	//	Transform()->SetRelativeRotation(vRot);
	//}
	//else if (KEY_RELEASED(KEY::RBTN))
	//{
	//	CKeyMgr::GetInst()->MouseCapture(false);
	//}




	if (KEY_JUST_PRESSED(KEY::RBTN))
	{
		CKeyMgr::GetInst()->MouseCapture(true);
	}

	else if (KEY_RELEASED(KEY::RBTN))
	{
		CKeyMgr::GetInst()->MouseCapture(false);
	}

	Vec2 vMouseDragDelta = CKeyMgr::GetInst()->GetMouseDragDelta();

	Vec3 vRot = Transform()->GetRelativeRoatation();
	vRot.y += vMouseDragDelta.x * XM_PI * 0.001f;
	vRot.x += vMouseDragDelta.y * XM_PI * 0.001f;
	Transform()->SetRelativeRotation(vRot);
	

}


void CCameraMoveScript::SaveToFile(FILE* _File)
{
	fwrite(&m_CamSpeed, sizeof(float), 1, _File);
}

void CCameraMoveScript::LoadFromFile(FILE* _File)
{
	fread(&m_CamSpeed, sizeof(float), 1, _File);
}