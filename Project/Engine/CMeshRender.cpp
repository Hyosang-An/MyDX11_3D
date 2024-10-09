#include "pch.h"
#include "CMeshRender.h"

#include "CTransform.h"
#include "CFlipBookComponent.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::FinalTick()
{
}

void CMeshRender::Render()
{
	if (!GetMesh() || !GetMaterial() || !(GetMaterial()->GetShader()))
		return;

	// FlipBookComponent 가 있으면 현재 재생중인 Sprite 정보를 Binding 하게 한다.
	if (FlipBookComponent())
		FlipBookComponent()->Binding();
	else
		CFlipBookComponent::Clear();

	// 위치, 크기, 회전 상태정보 바인딩
	Transform()->Binding();

	// 재질 바인딩(재질 상수, 쉐이더 등등)
	GetMaterial()->Binding();

	// 버텍스버퍼, 인덱스버퍼 바인딩 및 렌더링 호출
	GetMesh()->Render();
}

void CMeshRender::SaveToFile(FILE* _File)
{
	SaveDataToFile(_File);
}

void CMeshRender::LoadFromFile(FILE* _File)
{
	LoadDataFromFile(_File);
}