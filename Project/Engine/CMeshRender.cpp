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


	// FlipBookComponent �� ������ ���� ������� Sprite ������ Binding �ϰ� �Ѵ�.
	if (FlipBookComponent())
		FlipBookComponent()->Binding();
	else
		CFlipBookComponent::Clear();

	// ��ġ, ũ��, ȸ�� �������� ���ε�
	Transform()->Binding();


	for (UINT i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		// ���� ���ε�(���� ���, ���̴� ���)
		if (!GetMaterial(i))
			continue;

		GetMaterial(i)->Binding();

		// ���ؽ�����, �ε������� ���ε� �� ������ ȣ��
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