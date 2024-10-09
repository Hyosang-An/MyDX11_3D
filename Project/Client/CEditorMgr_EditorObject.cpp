#include "pch.h"

#include "CEditorMgr.h"
#include "CGameObjectEx.h"
#include <Engine/components.h>

#include "CEditorCameraScript.h"
#include <Engine/CRenderMgr.h>


void CEditorMgr::CreateEditorObject()
{
	CGameObject* pEditorCamera = new CGameObjectEx;
	pEditorCamera->SetName(L"EditorCamera");
	pEditorCamera->AddComponent(new CTransform);
	pEditorCamera->AddComponent(new CCamera);
	pEditorCamera->AddComponent(new CEditorCameraScript);

	pEditorCamera->Camera()->SetLayerAll();
	pEditorCamera->Camera()->SetLayer(31, false);
	pEditorCamera->Camera()->SetFar(100000.f);
	pEditorCamera->Camera()->SetProjType(ORTHOGRAPHIC);

	m_vecEditorObject.push_back(pEditorCamera);
	m_EditorCamera = pEditorCamera;

	CRenderMgr::GetInst()->RegisterEditorCamera(pEditorCamera->Camera()); 
}
