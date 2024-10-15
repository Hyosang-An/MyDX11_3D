#include "pch.h"
#include "CEditorCameraScript.h"

CEditorCameraScript::CEditorCameraScript() :
	CScript(-1),
	m_Speed(500.f)
{

}

CEditorCameraScript::~CEditorCameraScript()
{

}

void CEditorCameraScript::Tick()
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



void CEditorCameraScript::OrthoGraphicMove()
{
	float Speed = m_Speed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		Speed *= 3.f;
	}

	Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vPos.y += EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos.y -= EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos.x -= EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos.x += EngineDT * Speed;
	}

	Transform()->SetRelativePos(vPos);

	float Scale = Camera()->GetScale();
	if (KEY_PRESSED(KEY::_8))
		Scale += EngineDT;

	if (KEY_PRESSED(KEY::_9))
		Scale -= EngineDT;

	Camera()->SetScale(Scale);
}

void CEditorCameraScript::PerspectiveMove()
{
	float Speed = m_Speed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		Speed *= 3.f;
	}

	Vec3 vFront = Transform()->GetWorldDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetWorldDir(DIR::RIGHT);

	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vPos += vFront * EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos -= vFront * EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos -= vRight * EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos += vRight * EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::Q))
	{
		vPos.y -= EngineDT * Speed;
	}

	if (KEY_PRESSED(KEY::E))
	{
		vPos.y += EngineDT * Speed;
	}

	Transform()->SetRelativePos(vPos);

	//if (KEY_PRESSED(KEY::RBTN))
	//{
	//	CKeyMgr::GetInst()->MouseCapture(true);

	//	// ���콺�� �̵��ϴ� ����
	//	//vMouseDragDelta.x; ==> y�� ȸ��;
	//	//vMouseDragDelta.y; ==> x�� ȸ��
	//	Vec2 vMouseDragDelta = CKeyMgr::GetInst()->GetMouseDragDelta();

	//	Vec3 vRot = Transform()->GetRelativeRoatation();
	//	vRot.y += vMouseDragDelta.x * XM_PI * EngineDT * 10.f;
	//	vRot.x += vMouseDragDelta.y * XM_PI * EngineDT * 10.f;
	//	Transform()->SetRelativeRotation(vRot);
	//}
	//else if (KEY_RELEASED(KEY::RBTN))
	//{
	//	CKeyMgr::GetInst()->MouseCapture(false);
	//}



	static bool mousecontrol = false;
	//if (KEY_JUST_PRESSED(KEY::ESC))
	//	mousecontrol = !mousecontrol;
	if (KEY_JUST_PRESSED(KEY::LBTN) && KEY_PRESSED(KEY::ALT))
	{
		mousecontrol = true;
	}

	else if (KEY_RELEASED(KEY::LBTN) || KEY_RELEASED(KEY::ALT))
	{
		mousecontrol = false;
	}

	if (KEY_JUST_PRESSED(KEY::RBTN))
	{
		CKeyMgr::GetInst()->MouseCapture(true);
		mousecontrol = true;

	}

	else if (KEY_RELEASED(KEY::RBTN))
	{
		CKeyMgr::GetInst()->MouseCapture(false);
		mousecontrol = false;

	}

	if (mousecontrol)
	{
		Vec2 vMouseDragDelta = CKeyMgr::GetInst()->GetMouseDragDelta();

		Vec3 vRot = Transform()->GetRelativeRoatation();
		vRot.y += vMouseDragDelta.x * XM_PI * 0.001f;
		vRot.x += vMouseDragDelta.y * XM_PI * 0.001f;
		Transform()->SetRelativeRotation(vRot);
	}

	Vec3 vRot = Transform()->GetRelativeRoatation();


}
