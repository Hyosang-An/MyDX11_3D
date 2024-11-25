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
	if (!GetMesh() || !GetMaterialCount() /*|| !(GetMaterial()->GetShader())*/)
	{
		assert(false);
	}


	// FlipBookComponent 가 있으면 현재 재생중인 Sprite 정보를 Binding 하게 한다.
	if (FlipBookComponent())
		FlipBookComponent()->Binding();
	else
		CFlipBookComponent::Clear();

	// 위치, 크기, 회전 상태정보 바인딩
	Transform()->Binding();


	for (UINT i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		// 재질 바인딩(재질 상수, 쉐이더 등등)
		if (!GetMaterial(i))
			continue;

		GetMaterial(i)->Binding();

		// 버텍스버퍼, 인덱스버퍼 바인딩 및 렌더링 호출
		GetMesh()->Render(i);
	}
}

void CMeshRender::SaveToFile(FILE* _File)
{
	SaveAssetDataToFile(_File);
}

void CMeshRender::LoadFromFile(FILE* _File)
{
	LoadAssetDataFromFile(_File);
}