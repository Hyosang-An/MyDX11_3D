#include "pch.h"
#include "CCollider2D.h"

#include "CTransform.h"
#include "CScript.h"

CCollider2D::CCollider2D()
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_OverlapCount(0)
	, m_IndependentScale(false)
{
}

CCollider2D::CCollider2D(const CCollider2D& _Origin) :
	CComponent(_Origin),
	m_Offset(_Origin.m_Offset),
	m_Scale(_Origin.m_Scale),
	m_OverlapCount(0),
	m_IndependentScale(_Origin.m_IndependentScale)
{
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::FinalTick()
{
	Matrix matTranslation = XMMatrixTranslation(m_Offset.x, m_Offset.y, m_Offset.z);
	Matrix matScale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	// 크기, 회전, 이동
	Matrix matObjectScaleInv = XMMatrixIdentity();

	if (m_IndependentScale)
	{
		Vec3 vObjectScale = GetOwner()->Transform()->GetWorldScale();
		matObjectScaleInv = XMMatrixScaling(vObjectScale.x, vObjectScale.y, vObjectScale.z);
		matObjectScaleInv = XMMatrixInverse(nullptr, matObjectScaleInv);
	}

	// 디버깅
	auto owner = GetOwner();
	auto transform = owner->Transform();
	auto worldMat = transform->GetWorldMat();

	m_matColWorld = matScale * matTranslation * matObjectScaleInv * GetOwner()->Transform()->GetWorldMat();

	// Debug 렌더링으로 충돌체의 위치를 표시
	if (m_OverlapCount)
		DrawDebugRect(m_matColWorld, Vec4(1.f, 0.f, 0.f, 1.f), 0.f, false);
	else
		DrawDebugRect(m_matColWorld, Vec4(0.f, 1.f, 0.f, 1.f), 0.f, false);

	// 디버깅
	auto worldPos = GetWorldPos();
	auto worldScale = GetOwner()->Transform()->GetWorldScale();
}

void CCollider2D::BeginOverlap(CCollider2D* _Other)
{
	m_OverlapCount += 1;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->BeginOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CCollider2D::Overlap(CCollider2D* _Other)
{
	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->EndOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CCollider2D::EndOverlap(CCollider2D* _Other)
{
	m_OverlapCount -= 1;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->EndOverlap(this, _Other->GetOwner(), _Other);
	}
}

void CCollider2D::SaveToFile(FILE* _File)
{
	fwrite(&m_Offset, sizeof(Vec3), 1, _File);
	fwrite(&m_Scale, sizeof(Vec3), 1, _File);
	fwrite(&m_IndependentScale, sizeof(bool), 1, _File);
}

void CCollider2D::LoadFromFile(FILE* _File)
{
	fread(&m_Offset, sizeof(Vec3), 1, _File);
	fread(&m_Scale, sizeof(Vec3), 1, _File);
	fread(&m_IndependentScale, sizeof(bool), 1, _File);
}