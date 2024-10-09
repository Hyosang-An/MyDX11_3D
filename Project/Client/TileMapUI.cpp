#include "pch.h"
#include "TileMapUI.h"

#include "CEditorMgr.h"

#include "TreeUI.h"

#include "Engine/CTileMap.h"
#include "Engine/CPathMgr.h"
#include "Engine/CAssetMgr.h"
#include <Engine/CCamera.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CEngine.h>
#include <Engine/CTransform.h>
#include <Engine/CCollider2D.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

TileMapUI::TileMapUI()
	: ComponentUI(COMPONENT_TYPE::TILEMAP)
{
}

TileMapUI::~TileMapUI()
{
}

void TileMapUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();

	Title();

	m_selectedTileMap = GetTargetObject()->TileMap();

	if (m_selectedTileMap == nullptr)
		return;

	// Ÿ�ϸ� ��Ʋ�� ����
	Ptr<CTexture> pAtlasTex = m_selectedTileMap->GetAtlasTexture();
	if (pAtlasTex != nullptr)
	{
		// �̹���	
		ImGui::Text("Atlas");
		ImGui::SameLine(100);
		ImVec2 uv_min = ImVec2(0.0f, 0.0f);
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);

		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

		float aspectRatio = (float)pAtlasTex->Width() / (float)pAtlasTex->Height();

		ImGui::Image(pAtlasTex->GetSRV().Get(), ImVec2(150, 150 / aspectRatio), uv_min, uv_max, tint_col, border_col);
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentTree");
			if (payload)
			{
				TreeNode** ppNode = (TreeNode**)payload->Data;
				TreeNode* pNode = *ppNode;

				Ptr<CAsset> pAsset = reinterpret_cast<CAsset*>(pNode->GetData());
				if (ASSET_TYPE::TEXTURE == pAsset->GetAssetType())
				{
					pAtlasTex = ((CTexture*)pAsset.Get());
					m_selectedTileMap->SetAtlasTexture(pAtlasTex);
					m_selectedTileImgIndex = -1;

					// ��Ʋ�� �ؽ��İ� �ٲ�� tileinfo �ʱ�ȭ
					auto& vecTileInfo = m_selectedTileMap->GetTileInfoVec();
					for (size_t i = 0; i < vecTileInfo.size(); ++i)
					{
						vecTileInfo[i].ImgIdx = 0;
					}
				}
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		if (ImGui::Button("...##Tile AtlasTexture"))
		{
			SelectTileMapAtlasByDialog();
		}
	}

	// TileResolution ����
	Vec2 atlasTileResolution = m_selectedTileMap->GetAtlasTileResolution();
	ImGui::Text("Tile Resolution");
	ImGui::SameLine(120);
	int tileResolution[2] = { (int)atlasTileResolution.x, (int)atlasTileResolution.y };
	ImGui::InputInt2("##TileResolution", tileResolution);
	tileResolution[0] = max(1, tileResolution[0]);
	tileResolution[1] = max(1, tileResolution[1]);
	// InputInt2�� ��Ŀ���� Ǯ���� ����
	if (ImGui::IsItemDeactivatedAfterEdit())
		m_selectedTileMap->SetAtlasTileResolution(Vec2((float)tileResolution[0], (float)tileResolution[1]));


	// Ÿ�ϸ� Row, Col ����
	Vec2 vTileMapRowCol = m_selectedTileMap->GetRowCol();
	int rowcol[2] = { (int)vTileMapRowCol.x, (int)vTileMapRowCol.y };
	ImGui::Text("Row Col");
	ImGui::SameLine(120);
	ImGui::InputInt2("##RowCol", rowcol);
	rowcol[0] = max(1, rowcol[0]);
	rowcol[1] = max(1, rowcol[1]);
	if (ImGui::IsItemDeactivatedAfterEdit())
		m_selectedTileMap->SetRowCol(rowcol[0], rowcol[1]);
	

	// Ÿ�ϸ� Ÿ�� ũ�� ���� (World Scale)
	Vec2 vTileSize = m_selectedTileMap->GetTileSize();
	ImGui::Text("Tile Size");
	ImGui::SameLine(120);
	ImGui::DragFloat2("##Tile Size", vTileSize, 1.f, 0.0f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	vTileSize.x = max(0.f, vTileSize.x);
	vTileSize.y = max(0.f, vTileSize.y);
	if (ImGui::IsItemDeactivatedAfterEdit())
		m_selectedTileMap->SetTileSize(vTileSize);

	ImGui::Separator();


	// ������ ī�޶� �� ���콺 ���� ��ǥ ���, Ÿ�� ���� �� �浹ü ���� (level�� stop ������ ����)
	if (CLevelMgr::GetInst()->GetCurrentLevel()->GetState() == LEVEL_STATE::STOP && CEditorMgr::GetInst()->GetEditorCamera() != nullptr)
	{
		CCamera* editorCamera = CEditorMgr::GetInst()->GetEditorCamera()->Camera();
		ImGui::Text("Editor Camera Info");

		ImGui::Text("Screen World Scale");
		ImGui::SameLine(140);
		float screenWorldWidth = editorCamera->GetWidth() * editorCamera->GetScale();
		float screenWorldHeight = editorCamera->GetHeight() * editorCamera->GetScale();
		float screenWorldScale[2] = { screenWorldWidth, screenWorldHeight };
		ImGui::InputFloat2("##ScreenWorldScale", screenWorldScale, "%.3f", ImGuiInputTextFlags_ReadOnly);

		// ���콺 Ŭ���̾�Ʈ �� ��ǥ
		ImGui::Text("Mouse Pos In Client");
		ImGui::SameLine(140);
		POINT mousePosInClient;
		GetCursorPos(&mousePosInClient);
		ScreenToClient(CEngine::GetInst()->GetMainWnd(), &mousePosInClient);
		float mousePosInClientFloat[2] = { (float)mousePosInClient.x, (float)mousePosInClient.y };
		ImGui::InputFloat2("##Mouse Pos In Client", mousePosInClientFloat, "%.0f", ImGuiInputTextFlags_ReadOnly);


		// ������ī�޶� ��ġ
		ImGui::Text("Editor Camera Pos");
		ImGui::SameLine(140);
		CTransform* pCameraTrans = editorCamera->Transform();
		if (pCameraTrans == nullptr)
			return;
		Vec3 vEditorCameraPos = pCameraTrans->GetWorldPos();
		ImGui::InputFloat3("##EditorCameraPos", vEditorCameraPos, "%.3f", ImGuiInputTextFlags_ReadOnly);


		// Ŭ���̾�Ʈ �»�� world ��ǥ
		ImGui::Text("Client LT World Pos");
		ImGui::SameLine(140);
		Vec3 vClientLTWorldPos = vEditorCameraPos - Vec3(screenWorldWidth * 0.5f, -screenWorldHeight * 0.5f, 0.f);
		ImGui::InputFloat3("##ClientLTWorldPos", vClientLTWorldPos, "%.3f", ImGuiInputTextFlags_ReadOnly);

		// ���콺 world ��ǥ
		ImGui::Text("Mouse World Pos");
		ImGui::SameLine(140);
		Vec3 vMouseWorldPos = vClientLTWorldPos + Vec3(mousePosInClientFloat[0] * editorCamera->GetScale(), -mousePosInClientFloat[1] * editorCamera->GetScale(), 0.f);
		ImGui::InputFloat3("##MouseWorldPos", vMouseWorldPos, "%.3f", ImGuiInputTextFlags_ReadOnly);

		// Ÿ�ϸ� ���� ���콺 world ��ǥ ���
		ImGui::Text("Mouse Pos In TileMap");
		ImGui::SameLine(140);
		Vec3 vMousePosInTileMap = vMouseWorldPos - m_selectedTileMap->Transform()->GetRelativePos();
		ImGui::InputFloat3("##MousePosInTileMap", vMousePosInTileMap, "%.3f", ImGuiInputTextFlags_ReadOnly);

		// ���콺�� ����Ű�� Ÿ���� row col Ȯ��
		ImGui::Text("Mouse Tile Row Col");
		ImGui::SameLine(140);
		m_MouseTileRowCol = Vec2(-vMousePosInTileMap.y / vTileSize.y, vMousePosInTileMap.x / vTileSize.x);
		// row col�� ����ȭ
		m_MouseTileRowCol.x = floor(m_MouseTileRowCol.x);
		m_MouseTileRowCol.y = floor(m_MouseTileRowCol.y);
		ImGui::InputFloat2("##MouseTileRowCol", m_MouseTileRowCol, "%.0f", ImGuiInputTextFlags_ReadOnly);

		// ��ü Ÿ�ϸ� �����ڸ� �׵θ� �׸��� (DebugRender)
		Vec3 vTileMapLTWorldPos = m_selectedTileMap->Transform()->GetRelativePos();
		Vec3 vTileMapRBWorldPos = vTileMapLTWorldPos + Vec3(vTileMapRowCol.y * vTileSize.x, -vTileMapRowCol.x * vTileSize.y, 0.f);
		DrawDebugRect((vTileMapLTWorldPos + vTileMapRBWorldPos) * 0.5f, Vec3(vTileMapRowCol.y * vTileSize.x, vTileMapRowCol.x * vTileSize.y, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(1.f, 0.f, 1.f, 1.f), 0.f, false);


		ImGui::Separator();

		// Sprite SelectMod Combo Box
		ImGui::Text("TileMap Edit Mode");
		ImGui::SameLine(140);
		ImGui::SetNextItemWidth(180.f);
		const char* TileMapEditModes[] = { "Edit Tile", "Edit Collider" };
		if (ImGui::BeginCombo("##TileMapEditModes", TileMapEditModes[(int)m_editMode]))
		{
			if (ImGui::Selectable("Edit Tile"))
				m_editMode = TileMapEditMode::EditTile;
			if (ImGui::Selectable("Edit Collider"))
				m_editMode = TileMapEditMode::EditCollider;
			ImGui::EndCombo();
		}

		// EditTile Mode
		if (m_editMode == TileMapEditMode::EditTile)
		{
			// Ÿ�ϸ� Ÿ�� ����
			ImGui::Text("Tile Select");
			ImGui::SameLine(120);
			ImVec2 uv_min = ImVec2(0.0f, 0.0f);
			ImVec2 uv_max = ImVec2(1.0f, 1.0f);
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
			int atlasMaxRow = (int)m_selectedTileMap->GetAtlasMaxRowCol().x;
			int atlasMaxCol = (int)m_selectedTileMap->GetAtlasMaxRowCol().y;
			int atlasMaxTileSize = atlasMaxRow * atlasMaxCol;
			if (m_selectedTileImgIndex == -1 || m_selectedTileImgIndex >= atlasMaxTileSize)
			{
				tint_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				int row = m_selectedTileImgIndex / atlasMaxCol;
				int col = m_selectedTileImgIndex % atlasMaxCol;
				uv_min = ImVec2((float)col / atlasMaxCol, (float)row / atlasMaxRow);
				uv_max = ImVec2((float)(col + 1) / atlasMaxCol, (float)(row + 1) / atlasMaxRow);
			}
			float tileAspectRatio = atlasTileResolution.x / atlasTileResolution.y;
			ImGui::Image(pAtlasTex->GetSRV().Get(), ImVec2(50, 50 / tileAspectRatio), uv_min, uv_max, tint_col, border_col);

			// Ÿ�� �̹��� ��Ŭ���� �˾� â ���� ��Ʋ���̹����� �����ֱ�
			if (ImGui::IsItemClicked(0))
				ImGui::OpenPopup("TileSelect");
			if (ImGui::BeginPopup("TileSelect"))
			{
				ImGui::Text("Tile Select");
				ImGui::Separator();
				ImVec2 selectedTileLeftTopPos = ImGui::GetCursorScreenPos(); // �ʱ�ȭ
				for (int i = 0; i < atlasMaxTileSize; ++i)
				{
					int row = i / atlasMaxCol;
					int col = i % atlasMaxCol;
					uv_min = ImVec2((float)col / atlasMaxCol, (float)row / atlasMaxRow);
					uv_max = ImVec2((float)(col + 1) / atlasMaxCol, (float)(row + 1) / atlasMaxRow);
					tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
					border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
					if (m_selectedTileImgIndex == i)
					{
						// ������ Ÿ�� �̹��� selectedTileLeftTopPos ����
						selectedTileLeftTopPos = ImGui::GetCursorScreenPos();
					}
					ImGui::Image(pAtlasTex->GetSRV().Get(), ImVec2(50, 50 / tileAspectRatio), uv_min, uv_max, tint_col, border_col);
					if (ImGui::IsItemClicked())
					{
						m_selectedTileImgIndex = i;
						ImGui::CloseCurrentPopup();
					}
					if ((i + 1) % atlasMaxCol != 0)
						ImGui::SameLine();
				}

				// ������ Ÿ�� �ֺ��� �ʷϻ� �׵θ��� �׸���
				if (m_selectedTileImgIndex != -1)
				{
					// ��� �β� ����
					float borderThickness = 4.f;

					// �簢�� ũ�� ����
					ImVec2 borderBoxSize = ImVec2(50, 50 / tileAspectRatio);
					border_col = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // �׵θ� ���� ����
					// �簢�� ��� �׸���
					ImGui::GetWindowDrawList()->AddRect(selectedTileLeftTopPos, ImVec2(selectedTileLeftTopPos.x + borderBoxSize.x, selectedTileLeftTopPos.y + borderBoxSize.y), ImGui::GetColorU32(border_col), 0.0f, 0, borderThickness);
				}
				ImGui::EndPopup();
			}

			// ���콺�� Ÿ�ϸ� ���� ���� ��� �ش� Ÿ�� �ֺ��� �׵θ� �׸���
			if (m_MouseTileRowCol.x >= 0 && m_MouseTileRowCol.x < vTileMapRowCol.x &&
				m_MouseTileRowCol.y >= 0 && m_MouseTileRowCol.y < vTileMapRowCol.y)
			{
				// ���콺�� ȣ���ϴ� Ÿ���� �»�� world ��ǥ
				Vec3 vMouseTileLTWorldPos = m_selectedTileMap->Transform()->GetRelativePos() + Vec3(m_MouseTileRowCol.y * vTileSize.x, -m_MouseTileRowCol.x * vTileSize.y, 0.f);

				// DebugRender �׸���
				DrawDebugRect(vMouseTileLTWorldPos + Vec3(vTileSize.x * 0.5f, -vTileSize.y * 0.5f, 0), Vec3(vTileSize.x, vTileSize.y, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(1.f, 1.f, 0.f, 1.f), 0.f, false);
			}

			// ���콺 ��Ŭ���� Ÿ�ϸʿ� Ÿ�� �̹��� ����
			if (KEY_PRESSED(KEY::LBTN) && m_selectedTileImgIndex != -1)
			{
				// Ÿ�ϸ� ���� ������ Ŭ���� ���
				if (m_MouseTileRowCol.x >= 0 && m_MouseTileRowCol.x < vTileMapRowCol.x &&
					m_MouseTileRowCol.y >= 0 && m_MouseTileRowCol.y < vTileMapRowCol.y)
				{
					// Ÿ�ϸ��� Ÿ�� �ε��� ���
					int mouseTileIndex = (int)m_MouseTileRowCol.x * (int)vTileMapRowCol.y + (int)m_MouseTileRowCol.y;

					vector<tTileInfo>& tileInfoVec = m_selectedTileMap->GetTileInfoVec();

					// ���� �ε��� ���� ���ο� �ε��� ���� �ٸ� ��쿡�� stack�� ���� �� ����
					if (tileInfoVec[mouseTileIndex].ImgIdx != m_selectedTileImgIndex)
					{
						m_undoStack.push({ mouseTileIndex, tileInfoVec[mouseTileIndex].ImgIdx });
						tileInfoVec[mouseTileIndex].ImgIdx = m_selectedTileImgIndex;
					}
				}
			}

			// ���콺 ��Ŭ���� Ÿ�ϸʿ� Ÿ�� �̹��� ����
			if (KEY_PRESSED(KEY::RBTN))
			{
				// Ÿ�ϸ� ���� ������ Ŭ���� ���
				if (m_MouseTileRowCol.x >= 0 && m_MouseTileRowCol.x < vTileMapRowCol.x &&
					m_MouseTileRowCol.y >= 0 && m_MouseTileRowCol.y < vTileMapRowCol.y)
				{
					// Ÿ�ϸ��� Ÿ�� �ε��� ���
					int mouseTileIndex = (int)m_MouseTileRowCol.x * (int)vTileMapRowCol.y + (int)m_MouseTileRowCol.y;

					vector<tTileInfo>& tileInfoVec = m_selectedTileMap->GetTileInfoVec();

					// ���� �ε��� ���� -1�� �ƴ� ��쿡�� stack�� ���� �� ����
					if (tileInfoVec[mouseTileIndex].ImgIdx != -1)
					{
						m_undoStack.push({ mouseTileIndex, tileInfoVec[mouseTileIndex].ImgIdx });
						tileInfoVec[mouseTileIndex].ImgIdx = -1;
					}
				}
			}

			// Undo
			if (KEY_PRESSED(KEY::CTRL) && KEY_JUST_PRESSED(KEY::Z) && !m_undoStack.empty())
			{
				std::pair<int, int> undoData = m_undoStack.top();
				m_undoStack.pop();
				vector<tTileInfo>& tileInfoVec = m_selectedTileMap->GetTileInfoVec();
				tileInfoVec[undoData.first].ImgIdx = undoData.second;
			}
		}

		// Collider Edit Mode 
		else if (m_editMode == TileMapEditMode::EditCollider)
		{
			// Ÿ�ϸ� �浹ü ����
			ImGui::Text("Edit Collider");

			// ���� �������� Ŭ���̾�Ʈ ������ ������
			RECT clientRect;
			GetClientRect(CEngine::GetInst()->GetMainWnd(), &clientRect);

			// ���콺 Ŀ�� ��ġ�� Ŭ���̾�Ʈ ���� �ȿ� �ִ��� Ȯ��
			bool isMouseInClient = PtInRect(&clientRect, mousePosInClient);

			// ���콺�� Ŭ���̾�Ʈ ���� �����鼭 Ÿ�ϸ� ���� �ִ� ��쿡�� ���� ����
			if (isMouseInClient &&
				m_MouseTileRowCol.x >= 0 && m_MouseTileRowCol.x < vTileMapRowCol.x &&
				m_MouseTileRowCol.y >= 0 && m_MouseTileRowCol.y < vTileMapRowCol.y 
				)
			{

				// ���콺 ��Ŭ���� Ÿ�� ��ġ ���
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					m_MouseClickTileRowCol = m_MouseTileRowCol;
				}

				// ���콺�� Ŭ�� ���� �ܼ� ȣ�� ���¿��� Ÿ�ϸ� ���� ���� ��� �ش� Ÿ�� �ֺ��� �׵θ� �׸���
				if (false == ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{

					// ���콺�� ȣ���ϴ� Ÿ���� �»�� world ��ǥ
					Vec3 vMouseTileLTWorldPos = m_selectedTileMap->Transform()->GetRelativePos() + Vec3(m_MouseTileRowCol.y * vTileSize.x, -m_MouseTileRowCol.x * vTileSize.y, 0.f);

					// DebugRender �׸���
					DrawDebugRect(vMouseTileLTWorldPos + Vec3(vTileSize.x * 0.5f, -vTileSize.y * 0.5f, 0), Vec3(vTileSize.x, vTileSize.y, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(1.f, 1.f, 0.f, 1.f), 0.f, false);

				}

				// ���콺 �巡�� ���� ��� Ÿ�� ��ġ�� ���콺 ������ Ÿ�� ��ġ������ �簢���� �׸�
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					// Collider�� Ÿ�� LT Row, Col
					m_MouseReleaseTileRowCol = m_MouseTileRowCol;

					// Collider�� Ÿ�� LT Row, Col
					int colliderLTRow = min((int)m_MouseClickTileRowCol.x, (int)m_MouseReleaseTileRowCol.x);
					int colliderLTCol = min((int)m_MouseClickTileRowCol.y, (int)m_MouseReleaseTileRowCol.y);

					// Collider�� Ÿ�� RB Row, Col
					int colliderRBRow = max((int)m_MouseClickTileRowCol.x, (int)m_MouseReleaseTileRowCol.x);
					int colliderRBCol = max((int)m_MouseClickTileRowCol.y, (int)m_MouseReleaseTileRowCol.y);

					// Collider�� Ÿ�� LT, RB world ��ǥ
					vColliderLTWorldPos = m_selectedTileMap->Transform()->GetRelativePos() + Vec3(colliderLTCol * vTileSize.x, -colliderLTRow * vTileSize.y, 0.f);
					vColliderRBWorldPos = m_selectedTileMap->Transform()->GetRelativePos() + Vec3((colliderRBCol + 1) * vTileSize.x, -(colliderRBRow + 1) * vTileSize.y, 0.f);

					// DebugRender �׸���
					DrawDebugRect((vColliderLTWorldPos + vColliderRBWorldPos) * 0.5f, Vec3((colliderRBCol - colliderLTCol + 1)* vTileSize.x, (colliderRBRow - colliderLTRow + 1)* vTileSize.y, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(0.f, 1.f, 0.f, 1.f), 0.f, false);

					// ���콺 ��Ŭ�� ������ �� �浹ü �����ؼ� Ÿ�ϸʿ�����Ʈ�� �ڽ����� �߰�
					if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
					{
						// �浹ü�� ���� ������Ʈ ����
						CGameObject* pColliderObj = new CGameObject;
						pColliderObj->AddComponent(new CTransform);
						pColliderObj->AddComponent(new CCollider2D);

						// �浹ü�� ��ġ�� Ÿ�ϸ��� ��� ��ǥ
						Vec3 vColliderRelativePos = ((vColliderLTWorldPos + vColliderRBWorldPos) * 0.5f - m_selectedTileMap->Transform()->GetRelativePos()) / m_selectedTileMap->GetOwner()->Transform()->GetRelativeScale();
						pColliderObj->Transform()->SetRelativePos(vColliderRelativePos);

						// �浹ü�� ũ��� Ÿ�ϸ��� ��� ũ��
						Vec3 vColliderRelativeScale = Vec3((colliderRBCol - colliderLTCol + 1) * vTileSize.x , (colliderRBRow - colliderLTRow + 1) * vTileSize.y, 1.f) / m_selectedTileMap->GetOwner()->Transform()->GetRelativeScale();
						pColliderObj->Transform()->SetRelativeScale(vColliderRelativeScale);
						pColliderObj->Collider2D()->SetScale(Vec3(1, 1, 1));

						wstring defaultName = L"TileMapColliderObj";
						UINT idx = 0;
						wstring colliderObjName = defaultName + L" " + std::to_wstring(idx);

						// ���� Ÿ�ϸ��� �ڽ� ������Ʈ ��������
						const vector<CGameObject*>& vecChild = m_selectedTileMap->GetOwner()->GetChildren();

						// �ߺ����� �ʴ� �̸� ����
						while (true)
						{
							bool isExist = false;
							for (size_t i = 0; i < vecChild.size(); ++i)
							{
								if (vecChild[i]->GetName() == colliderObjName)
								{
									isExist = true;
									break;
								}
							}
							
							if (isExist == false)
								break;

							colliderObjName = defaultName + L" " + std::to_wstring(++idx);
						}

						pColliderObj->SetName(colliderObjName);
						// ���� ������ �߰�
						CLevelMgr::GetInst()->GetCurrentLevel()->AddObject(2, pColliderObj);
						m_selectedTileMap->GetOwner()->AddChild(pColliderObj);
					}
				}
			}
		}

		// Ÿ�� �׸��� ǥ�� ���� üũ�ڽ�
		bool isShowTileGrid = m_selectedTileMap->IsShowTileGrid();
		ImGui::Checkbox("Show Tile Grid", &isShowTileGrid);
		m_selectedTileMap->SetTileGridShow(isShowTileGrid);

		ImGui::Separator();
	}


	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(content_size);
}


void TileMapUI::SelectTileMapAtlasByDialog()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			// ���� ���͸� �̹��� ���Ͽ� �°� ����
			COMDLG_FILTERSPEC rgSpec[] = {
				{ L"Image Files", L"*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.dds" },
				{ L"All Files", L"*.*" }
			};
			pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
			pFileOpen->SetFileTypeIndex(1);
			pFileOpen->SetTitle(L"Load Image File");

			IShellItem* psiFolder = nullptr;
			wstring defaultDirectory;
			if (m_lastTextureDirectory.empty())
				defaultDirectory = CPathMgr::GetInst()->GetContentPath() + L"texture";
			else
				defaultDirectory = m_lastTextureDirectory;
			hr = SHCreateItemFromParsingName(defaultDirectory.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
			if (SUCCEEDED(hr) && psiFolder) {
				pFileOpen->SetFolder(psiFolder);
				psiFolder->Release();
			}

			hr = pFileOpen->Show(NULL);

			if (SUCCEEDED(hr)) {
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr)) {

						// �̹��� ������ ó���ϴ� ���� �߰�
						wstring relativePath = CPathMgr::GetInst()->GetRelativePath(pszFilePath);

						// �����ϴ� �̹��� Ȯ���� �˻� (��ҹ��� ���� ����)
						path filePath = pszFilePath;
						wstring extension = filePath.extension().wstring();
						std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower); // Ȯ���ڸ� �ҹ��ڷ� ��ȯ

						if (extension != L".png" && extension != L".jpg" && extension != L".jpeg" &&
							extension != L".bmp" && extension != L".tga" && extension != L".dds")
						{
							MessageBox(NULL, L"������ ������ �����Ǵ� �̹��� ������ �ƴմϴ�.", L"Error", MB_OK);
							return;
						}

						// �̹��� ������ ó���ϴ� ������ ���⿡ �߰�
						// ��: m_owner->SetTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(relativePath));
						m_selectedTileMap->SetAtlasTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(relativePath));
						m_selectedTileImgIndex = -1;

						// ��Ʋ�� �ؽ��İ� �ٲ�� tileinfo �ʱ�ȭ
						auto& vecTileInfo = m_selectedTileMap->GetTileInfoVec();
						for (size_t i = 0; i < vecTileInfo.size(); ++i)
						{
							vecTileInfo[i].ImgIdx = 0;
						}

						m_lastTextureDirectory = filePath.parent_path().wstring();

						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
}
