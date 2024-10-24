#include "pch.h"
#include "CDecal.h"

#include "CTransform.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
{
}

CDecal::~CDecal()
{
}

void CDecal::FinalTick()
{
	Vec3 vStart = Transform()->GetWorldPos();
	Vec3 vEnd = vStart + Vec3(0.f, 10000.f, 0.f);

	DrawDebugLine(vStart, vEnd, Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);

	DrawDebugCube(Transform()->GetWorldMat(), Vec4(0.f, 1.f, 0.f, 1.f), 0, true);
}

void CDecal::Render()
{
}

void CDecal::SaveToFile(FILE* _File)
{
}

void CDecal::LoadFromFile(FILE* _File)
{
}
