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

	// 타일맵 아틀라스 설정
	Ptr<CTexture> pAtlasTex = m_selectedTileMap->GetAtlasTexture();
	if (pAtlasTex != nullptr)
	{
		// 이미지	
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

					// 아틀라스 텍스쳐가 바뀌면 tileinfo 초기화
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

	// TileResolution 설정
	Vec2 atlasTileResolution = m_selectedTileMap->GetAtlasTileResolution();
	ImGui::Text("Tile Resolution");
	ImGui::SameLine(120);
	int tileResolution[2] = { (int)atlasTileResolution.x, (int)atlasTileResolution.y };
	ImGui::InputInt2("##TileResolution", tileResolution);
	tileResolution[0] = max(1, tileResolution[0]);
	tileResolution[1] = max(1, tileResolution[1]);
	// InputInt2의 포커스가 풀리면 적용
	if (ImGui::IsItemDeactivatedAfterEdit())
		m_selectedTileMap->SetAtlasTileResolution(Vec2((float)tileResolution[0], (float)tileResolution[1]));


	// 타일맵 Row, Col 설정
	Vec2 vTileMapRowCol = m_selectedTileMap->GetRowCol();
	int rowcol[2] = { (int)vTileMapRowCol.x, (int)vTileMapRowCol.y };
	ImGui::Text("Row Col");
	ImGui::SameLine(120);
	ImGui::InputInt2("##RowCol", rowcol);
	rowcol[0] = max(1, rowcol[0]);
	rowcol[1] = max(1, rowcol[1]);
	if (ImGui::IsItemDeactivatedAfterEdit())
		m_selectedTileMap->SetRowCol(rowcol[0], rowcol[1]);
	

	// 타일맵 타일 크기 설정 (World Scale)
	Vec2 vTileSize = m_selectedTileMap->GetTileSize();
	ImGui::Text("Tile Size");
	ImGui::SameLine(120);
	ImGui::DragFloat2("##Tile Size", vTileSize, 1.f, 0.0f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	vTileSize.x = max(0.f, vTileSize.x);
	vTileSize.y = max(0.f, vTileSize.y);
	if (ImGui::IsItemDeactivatedAfterEdit())
		m_selectedTileMap->SetTileSize(vTileSize);

	ImGui::Separator();


	// 에디터 카메라 및 마우스 월드 좌표 계산, 타일 편집 및 충돌체 편집 (level이 stop 상태일 때만)
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

		// 마우스 클라이언트 내 좌표
		ImGui::Text("Mouse Pos In Client");
		ImGui::SameLine(140);
		POINT mousePosInClient;
		GetCursorPos(&mousePosInClient);
		ScreenToClient(CEngine::GetInst()->GetMainWnd(), &mousePosInClient);
		float mousePosInClientFloat[2] = { (float)mousePosInClient.x, (float)mousePosInClient.y };
		ImGui::InputFloat2("##Mouse Pos In Client", mousePosInClientFloat, "%.0f", ImGuiInputTextFlags_ReadOnly);


		// 에디터카메라 위치
		ImGui::Text("Editor Camera Pos");
		ImGui::SameLine(140);
		CTransform* pCameraTrans = editorCamera->Transform();
		if (pCameraTrans == nullptr)
			return;
		Vec3 vEditorCameraPos = pCameraTrans->GetWorldPos();
		ImGui::InputFloat3("##EditorCameraPos", vEditorCameraPos, "%.3f", ImGuiInputTextFlags_ReadOnly);


		// 클라이언트 좌상단 world 좌표
		ImGui::Text("Client LT World Pos");
		ImGui::SameLine(140);
		Vec3 vClientLTWorldPos = vEditorCameraPos - Vec3(screenWorldWidth * 0.5f, -screenWorldHeight * 0.5f, 0.f);
		ImGui::InputFloat3("##ClientLTWorldPos", vClientLTWorldPos, "%.3f", ImGuiInputTextFlags_ReadOnly);

		// 마우스 world 좌표
		ImGui::Text("Mouse World Pos");
		ImGui::SameLine(140);
		Vec3 vMouseWorldPos = vClientLTWorldPos + Vec3(mousePosInClientFloat[0] * editorCamera->GetScale(), -mousePosInClientFloat[1] * editorCamera->GetScale(), 0.f);
		ImGui::InputFloat3("##MouseWorldPos", vMouseWorldPos, "%.3f", ImGuiInputTextFlags_ReadOnly);

		// 타일맵 기준 마우스 world 좌표 계산
		ImGui::Text("Mouse Pos In TileMap");
		ImGui::SameLine(140);
		Vec3 vMousePosInTileMap = vMouseWorldPos - m_selectedTileMap->Transform()->GetRelativePos();
		ImGui::InputFloat3("##MousePosInTileMap", vMousePosInTileMap, "%.3f", ImGuiInputTextFlags_ReadOnly);

		// 마우스가 가리키는 타일의 row col 확인
		ImGui::Text("Mouse Tile Row Col");
		ImGui::SameLine(140);
		m_MouseTileRowCol = Vec2(-vMousePosInTileMap.y / vTileSize.y, vMousePosInTileMap.x / vTileSize.x);
		// row col을 정수화
		m_MouseTileRowCol.x = floor(m_MouseTileRowCol.x);
		m_MouseTileRowCol.y = floor(m_MouseTileRowCol.y);
		ImGui::InputFloat2("##MouseTileRowCol", m_MouseTileRowCol, "%.0f", ImGuiInputTextFlags_ReadOnly);

		// 전체 타일맵 가장자리 테두리 그리기 (DebugRender)
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
			// 타일맵 타일 선택
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

			// 타일 이미지 좌클릭시 팝업 창 띄우고 아틀라스이미지를 보여주기
			if (ImGui::IsItemClicked(0))
				ImGui::OpenPopup("TileSelect");
			if (ImGui::BeginPopup("TileSelect"))
			{
				ImGui::Text("Tile Select");
				ImGui::Separator();
				ImVec2 selectedTileLeftTopPos = ImGui::GetCursorScreenPos(); // 초기화
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
						// 선택한 타일 이미지 selectedTileLeftTopPos 저장
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

				// 선택한 타일 주변에 초록색 테두리를 그리기
				if (m_selectedTileImgIndex != -1)
				{
					// 경계 두께 설정
					float borderThickness = 4.f;

					// 사각형 크기 설정
					ImVec2 borderBoxSize = ImVec2(50, 50 / tileAspectRatio);
					border_col = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // 테두리 색상 설정
					// 사각형 경계 그리기
					ImGui::GetWindowDrawList()->AddRect(selectedTileLeftTopPos, ImVec2(selectedTileLeftTopPos.x + borderBoxSize.x, selectedTileLeftTopPos.y + borderBoxSize.y), ImGui::GetColorU32(border_col), 0.0f, 0, borderThickness);
				}
				ImGui::EndPopup();
			}

			// 마우스가 타일맵 위에 있을 경우 해당 타일 주변에 테두리 그리기
			if (m_MouseTileRowCol.x >= 0 && m_MouseTileRowCol.x < vTileMapRowCol.x &&
				m_MouseTileRowCol.y >= 0 && m_MouseTileRowCol.y < vTileMapRowCol.y)
			{
				// 마우스가 호버하는 타일의 좌상단 world 좌표
				Vec3 vMouseTileLTWorldPos = m_selectedTileMap->Transform()->GetRelativePos() + Vec3(m_MouseTileRowCol.y * vTileSize.x, -m_MouseTileRowCol.x * vTileSize.y, 0.f);

				// DebugRender 그리기
				DrawDebugRect(vMouseTileLTWorldPos + Vec3(vTileSize.x * 0.5f, -vTileSize.y * 0.5f, 0), Vec3(vTileSize.x, vTileSize.y, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(1.f, 1.f, 0.f, 1.f), 0.f, false);
			}

			// 마우스 좌클릭시 타일맵에 타일 이미지 삽입
			if (KEY_PRESSED(KEY::LBTN) && m_selectedTileImgIndex != -1)
			{
				// 타일맵 범위 내에서 클릭한 경우
				if (m_MouseTileRowCol.x >= 0 && m_MouseTileRowCol.x < vTileMapRowCol.x &&
					m_MouseTileRowCol.y >= 0 && m_MouseTileRowCol.y < vTileMapRowCol.y)
				{
					// 타일맵의 타일 인덱스 계산
					int mouseTileIndex = (int)m_MouseTileRowCol.x * (int)vTileMapRowCol.y + (int)m_MouseTileRowCol.y;

					vector<tTileInfo>& tileInfoVec = m_selectedTileMap->GetTileInfoVec();

					// 기존 인덱스 값과 새로운 인덱스 값이 다를 경우에만 stack에 저장 후 변경
					if (tileInfoVec[mouseTileIndex].ImgIdx != m_selectedTileImgIndex)
					{
						m_undoStack.push({ mouseTileIndex, tileInfoVec[mouseTileIndex].ImgIdx });
						tileInfoVec[mouseTileIndex].ImgIdx = m_selectedTileImgIndex;
					}
				}
			}

			// 마우스 우클릭시 타일맵에 타일 이미지 삭제
			if (KEY_PRESSED(KEY::RBTN))
			{
				// 타일맵 범위 내에서 클릭한 경우
				if (m_MouseTileRowCol.x >= 0 && m_MouseTileRowCol.x < vTileMapRowCol.x &&
					m_MouseTileRowCol.y >= 0 && m_MouseTileRowCol.y < vTileMapRowCol.y)
				{
					// 타일맵의 타일 인덱스 계산
					int mouseTileIndex = (int)m_MouseTileRowCol.x * (int)vTileMapRowCol.y + (int)m_MouseTileRowCol.y;

					vector<tTileInfo>& tileInfoVec = m_selectedTileMap->GetTileInfoVec();

					// 기존 인덱스 값이 -1이 아닌 경우에만 stack에 저장 후 변경
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
			// 타일맵 충돌체 편집
			ImGui::Text("Edit Collider");

			// 메인 윈도우의 클라이언트 영역을 가져옴
			RECT clientRect;
			GetClientRect(CEngine::GetInst()->GetMainWnd(), &clientRect);

			// 마우스 커서 위치가 클라이언트 영역 안에 있는지 확인
			bool isMouseInClient = PtInRect(&clientRect, mousePosInClient);

			// 마우스가 클라이언트 내에 있으면서 타일맵 위에 있는 경우에만 편집 가능
			if (isMouseInClient &&
				m_MouseTileRowCol.x >= 0 && m_MouseTileRowCol.x < vTileMapRowCol.x &&
				m_MouseTileRowCol.y >= 0 && m_MouseTileRowCol.y < vTileMapRowCol.y 
				)
			{

				// 마우스 좌클릭시 타일 위치 기억
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					m_MouseClickTileRowCol = m_MouseTileRowCol;
				}

				// 마우스가 클릭 없이 단순 호버 상태에서 타일맵 위에 있을 경우 해당 타일 주변에 테두리 그리기
				if (false == ImGui::IsMouseDown(ImGuiMouseButton_Left))
				{

					// 마우스가 호버하는 타일의 좌상단 world 좌표
					Vec3 vMouseTileLTWorldPos = m_selectedTileMap->Transform()->GetRelativePos() + Vec3(m_MouseTileRowCol.y * vTileSize.x, -m_MouseTileRowCol.x * vTileSize.y, 0.f);

					// DebugRender 그리기
					DrawDebugRect(vMouseTileLTWorldPos + Vec3(vTileSize.x * 0.5f, -vTileSize.y * 0.5f, 0), Vec3(vTileSize.x, vTileSize.y, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(1.f, 1.f, 0.f, 1.f), 0.f, false);

				}

				// 마우스 드래그 중일 경우 타일 위치와 마우스 릴리즈 타일 위치까지의 사각형을 그림
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					// Collider의 타일 LT Row, Col
					m_MouseReleaseTileRowCol = m_MouseTileRowCol;

					// Collider의 타일 LT Row, Col
					int colliderLTRow = min((int)m_MouseClickTileRowCol.x, (int)m_MouseReleaseTileRowCol.x);
					int colliderLTCol = min((int)m_MouseClickTileRowCol.y, (int)m_MouseReleaseTileRowCol.y);

					// Collider의 타일 RB Row, Col
					int colliderRBRow = max((int)m_MouseClickTileRowCol.x, (int)m_MouseReleaseTileRowCol.x);
					int colliderRBCol = max((int)m_MouseClickTileRowCol.y, (int)m_MouseReleaseTileRowCol.y);

					// Collider의 타일 LT, RB world 좌표
					vColliderLTWorldPos = m_selectedTileMap->Transform()->GetRelativePos() + Vec3(colliderLTCol * vTileSize.x, -colliderLTRow * vTileSize.y, 0.f);
					vColliderRBWorldPos = m_selectedTileMap->Transform()->GetRelativePos() + Vec3((colliderRBCol + 1) * vTileSize.x, -(colliderRBRow + 1) * vTileSize.y, 0.f);

					// DebugRender 그리기
					DrawDebugRect((vColliderLTWorldPos + vColliderRBWorldPos) * 0.5f, Vec3((colliderRBCol - colliderLTCol + 1)* vTileSize.x, (colliderRBRow - colliderLTRow + 1)* vTileSize.y, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(0.f, 1.f, 0.f, 1.f), 0.f, false);

					// 마우스 좌클릭 떼었을 때 충돌체 생성해서 타일맵오브젝트의 자식으로 추가
					if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
					{
						// 충돌체를 가질 오브젝트 생성
						CGameObject* pColliderObj = new CGameObject;
						pColliderObj->AddComponent(new CTransform);
						pColliderObj->AddComponent(new CCollider2D);

						// 충돌체의 위치는 타일맵의 상대 좌표
						Vec3 vColliderRelativePos = ((vColliderLTWorldPos + vColliderRBWorldPos) * 0.5f - m_selectedTileMap->Transform()->GetRelativePos()) / m_selectedTileMap->GetOwner()->Transform()->GetRelativeScale();
						pColliderObj->Transform()->SetRelativePos(vColliderRelativePos);

						// 충돌체의 크기는 타일맵의 상대 크기
						Vec3 vColliderRelativeScale = Vec3((colliderRBCol - colliderLTCol + 1) * vTileSize.x , (colliderRBRow - colliderLTRow + 1) * vTileSize.y, 1.f) / m_selectedTileMap->GetOwner()->Transform()->GetRelativeScale();
						pColliderObj->Transform()->SetRelativeScale(vColliderRelativeScale);
						pColliderObj->Collider2D()->SetScale(Vec3(1, 1, 1));

						wstring defaultName = L"TileMapColliderObj";
						UINT idx = 0;
						wstring colliderObjName = defaultName + L" " + std::to_wstring(idx);

						// 현재 타일맵의 자식 오브젝트 가져오기
						const vector<CGameObject*>& vecChild = m_selectedTileMap->GetOwner()->GetChildren();

						// 중복되지 않는 이름 생성
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
						// 현재 레벨에 추가
						CLevelMgr::GetInst()->GetCurrentLevel()->AddObject(2, pColliderObj);
						m_selectedTileMap->GetOwner()->AddChild(pColliderObj);
					}
				}
			}
		}

		// 타일 그리드 표시 여부 체크박스
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
			// 파일 필터를 이미지 파일에 맞게 변경
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

						// 이미지 파일을 처리하는 로직 추가
						wstring relativePath = CPathMgr::GetInst()->GetRelativePath(pszFilePath);

						// 지원하는 이미지 확장자 검사 (대소문자 구분 없이)
						path filePath = pszFilePath;
						wstring extension = filePath.extension().wstring();
						std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower); // 확장자를 소문자로 변환

						if (extension != L".png" && extension != L".jpg" && extension != L".jpeg" &&
							extension != L".bmp" && extension != L".tga" && extension != L".dds")
						{
							MessageBox(NULL, L"선택한 파일이 지원되는 이미지 파일이 아닙니다.", L"Error", MB_OK);
							return;
						}

						// 이미지 파일을 처리하는 로직을 여기에 추가
						// 예: m_owner->SetTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(relativePath));
						m_selectedTileMap->SetAtlasTexture(CAssetMgr::GetInst()->FindAsset<CTexture>(relativePath));
						m_selectedTileImgIndex = -1;

						// 아틀라스 텍스쳐가 바뀌면 tileinfo 초기화
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
