#include "pch.h"
#include "FlipBookViewer.h"
#include "FlipBookInspector.h"

#include "Engine/CPathMgr.h"

FlipBookViewer::FlipBookViewer()
{
}

FlipBookViewer::~FlipBookViewer()
{

}

void FlipBookViewer::Init()
{
}

//void FlipBookViewer::Update()
//{
//	ImGui::Text("This is the FlipBookViewer.");
//
//	// Get the flipbook from the owner
//	Ptr<CFlipBook> m_selectedFlipBook = m_owner->GetFlipBook();
//	int selectedSpriteIndex = m_inspector->GetSelectedSpriteIndex();
//
//	if (selectedSpriteIndex != -1)
//	{
//		// Get the sprite from the flipbook
//		Ptr<CSprite> sprite = m_selectedFlipBook->GetSprite(selectedSpriteIndex);
//		if (sprite != nullptr)
//		{
//			// Get the atlasTexture from the sprite
//			Ptr<CTexture> atlasTexture = sprite->GetAtlasTexture();
//			if (atlasTexture != nullptr)
//			{
//				Vec2 leftTopInAtlasUV = sprite->GetLeftTopInAtlasUV();
//				Vec2 sliceSizeInAtlasUV = sprite->GetSliceSizeInAtlasUV();
//				Vec2 backgroundSizeInAtlasUV = sprite->GetBackgroundSizeInAtlasUV();
//				Vec2 offsetUV = sprite->GetOffsetUV();
//
//				Vec2 backgroundLeftTopUV = leftTopInAtlasUV - (backgroundSizeInAtlasUV - sliceSizeInAtlasUV) * 0.5f - offsetUV;
//
//
//
//				// 스프라이트 uv 정보
//				ImVec2 uv_min = ImVec2(leftTopInAtlasUV.x, leftTopInAtlasUV.y);
//				ImVec2 uv_max = ImVec2(uv_min.x + sliceSizeInAtlasUV.x, uv_min.y + sliceSizeInAtlasUV.y);
//
//				ImVec4 sprite_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
//				ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
//				ImVec4 background_col = ImVec4(0, 0, 0, 0);
//
//				// 먼저 스프라이트 배경(알파 0)부터 출력
//				ImVec2 backgroundImagePos = ImGui::GetCursorPos(); // 커서 위치는 창의 좌상단 기준
//				ImGui::Image(atlasTexture->GetSRV().Get(), ImVec2(atlasTexture->Width() * backgroundSizeInAtlasUV.x, atlasTexture->Height() * backgroundSizeInAtlasUV.y), uv_min, uv_max, background_col, border_col);
//
//				// 스프라이트 밑에 정보를 출력하기 위한 좌표 저장
//				ImVec2 backgroundRectMin = ImGui::GetItemRectMin(); // 배경 이미지의 좌상단 좌표는 스크린 좌표 기준
//				ImVec2 backgroundRectMax = ImGui::GetItemRectMax(); // 배경 이미지의 우하단 좌표는 스크린 좌표 기준
//
//
//				// 스프라이트 출력
//				ImVec2 spriteImagePos = ImVec2(backgroundImagePos.x + (backgroundSizeInAtlasUV.x - sliceSizeInAtlasUV.x) * 0.5f * atlasTexture->Width() + offsetUV.x, backgroundImagePos.y + (backgroundSizeInAtlasUV.y - sliceSizeInAtlasUV.y) * 0.5f * atlasTexture->Height() + offsetUV.y);
//				ImGui::SetCursorPos(spriteImagePos); // 커서 위치를 이전 이미지의 시작 위치로 설정
//				ImGui::Image(atlasTexture->GetSRV().Get(), ImVec2(atlasTexture->Width() * sliceSizeInAtlasUV.x, atlasTexture->Height() * sliceSizeInAtlasUV.y), uv_min, uv_max, sprite_col, border_col);
//				ImGui::SetCursorPos(ImVec2(backgroundImagePos.x, backgroundImagePos.y + backgroundRectMax.y - backgroundRectMin.y + 20)); // 20은 여백
//
//
//				// 십자선 그리기 (스크린 좌표 기준)
//				ImDrawList* drawList = ImGui::GetWindowDrawList();
//				ImVec2 center = ImVec2((backgroundRectMin.x + backgroundRectMax.x) * 0.5f, (backgroundRectMin.y + backgroundRectMax.y) * 0.5f);
//				ImU32 crosshairColor = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 0.5f)); // 빨간색
//				drawList->AddLine(ImVec2(center.x, backgroundRectMin.y), ImVec2(center.x, backgroundRectMax.y), crosshairColor, 1.0f);
//				drawList->AddLine(ImVec2(backgroundRectMin.x, center.y), ImVec2(backgroundRectMax.x, center.y), crosshairColor, 1.0f);
//
//
//
//				// ========================================================================================================================================================================================================================
//				// ========================================================================================================================================================================================================================
//
//				// Image 위젯 좌상단 좌표
//				float ArrImageMin[] = { backgroundRectMin.x, backgroundRectMin.y };
//				ImGui::InputFloat2("ImageMin", ArrImageMin, "%.0f");
//
//
//				// 현재 마우스 위치
//				auto m_MousePos = ImGui::GetMousePos();
//				float arrMousePos[] = { m_MousePos.x, m_MousePos.y };
//				ImGui::InputFloat2("MousePos", arrMousePos, "%.0f");
//
//				ImVec2 vDiff = ImVec2(m_MousePos.x - backgroundRectMin.x, m_MousePos.y - backgroundRectMin.y);
//				//vDiff = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);
//
//				// 마우스 위치의 아틀라스 픽셀좌표	
//				float PixelPos[] = { vDiff.x, vDiff.y };
//				ImGui::InputFloat2("PixelPos", PixelPos, "%.0f");
//			}
//		}
//
//		
//	}
//}


void FlipBookViewer::Update()
{
    ImGui::Text("This is the FlipBookViewer.");

    // Get the flipbook from the owner
    Ptr<CFlipBook> flipBook = m_owner->GetFlipBook();
    int selectedSpriteIndex = m_inspector->GetSelectedSpriteIndex();

    if (selectedSpriteIndex == -1) return;

    // Get the sprite from the flipbook
    Ptr<CSprite> sprite = flipBook->GetSprite(selectedSpriteIndex);
    if (sprite == nullptr) return;

    // Get the atlasTexture from the sprite
    Ptr<CTexture> atlasTexture = sprite->GetAtlasTexture();
    if (atlasTexture == nullptr) return;

    Vec2 leftTopInAtlasUV = sprite->GetLeftTopInAtlasUV();
    Vec2 sliceSizeInAtlasUV = sprite->GetSliceSizeInAtlasUV();
    Vec2 backgroundSizeInAtlasUV = sprite->GetBackgroundSizeInAtlasUV();
    Vec2 offsetUV = sprite->GetOffsetUV();

    //Vec2 backgroundLeftTopUV = leftTopInAtlasUV - (backgroundSizeInAtlasUV - sliceSizeInAtlasUV) * 0.5f - offsetUV;

    // 스프라이트 uv 정보
    ImVec2 uv_min = ImVec2(leftTopInAtlasUV.x, leftTopInAtlasUV.y);
    ImVec2 uv_max = ImVec2(uv_min.x + sliceSizeInAtlasUV.x, uv_min.y + sliceSizeInAtlasUV.y);

    ImVec4 sprite_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    ImVec4 background_col = ImVec4(0, 0, 0, 0);

    // 먼저 스프라이트 배경(알파 0)부터 출력
    ImVec2 backgroundImagePos = ImGui::GetCursorPos();
    ImGui::Image(atlasTexture->GetSRV().Get(), ImVec2(atlasTexture->Width() * backgroundSizeInAtlasUV.x, atlasTexture->Height() * backgroundSizeInAtlasUV.y), uv_min, uv_max, background_col, border_col);

    // 스프라이트 밑에 정보를 출력하기 위한 좌표 저장
    ImVec2 backgroundRectMin = ImGui::GetItemRectMin(); // 배경 이미지의 좌상단 좌표는 스크린 좌표 기준
    ImVec2 backgroundRectMax = ImGui::GetItemRectMax(); // 배경 이미지의 우하단 좌표는 스크린 좌표 기준

    // 스프라이트 출력
    ImVec2 spriteImagePos = ImVec2(backgroundImagePos.x + ((backgroundSizeInAtlasUV.x - sliceSizeInAtlasUV.x) * 0.5f + offsetUV.x) * atlasTexture->Width(), backgroundImagePos.y + ((backgroundSizeInAtlasUV.y - sliceSizeInAtlasUV.y) * 0.5f + offsetUV.y) * atlasTexture->Height());
    ImGui::SetCursorPos(spriteImagePos);    // 커서 위치를 배경 이미지의 시작 위치로 설정
    ImGui::Image(atlasTexture->GetSRV().Get(), ImVec2(atlasTexture->Width() * sliceSizeInAtlasUV.x, atlasTexture->Height() * sliceSizeInAtlasUV.y), uv_min, uv_max, sprite_col, border_col);
    ImGui::SetCursorPos(ImVec2(backgroundImagePos.x, backgroundImagePos.y + backgroundRectMax.y - backgroundRectMin.y + 20));

    // 십자선 그리기
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2((backgroundRectMin.x + backgroundRectMax.x) * 0.5f, (backgroundRectMin.y + backgroundRectMax.y) * 0.5f);
    ImU32 crosshairColor = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
    drawList->AddLine(ImVec2(center.x, backgroundRectMin.y), ImVec2(center.x, backgroundRectMax.y), crosshairColor, 1.0f);
    drawList->AddLine(ImVec2(backgroundRectMin.x, center.y), ImVec2(backgroundRectMax.x, center.y), crosshairColor, 1.0f);

    // ========================================================================================================================================================================================================================
    // ========================================================================================================================================================================================================================

    // Image 위젯 좌상단 좌표
    float ArrImageMin[] = { backgroundRectMin.x, backgroundRectMin.y };
    ImGui::InputFloat2("ImageMin", ArrImageMin, "%.0f");

    // 현재 마우스 위치
    auto m_MousePos = ImGui::GetMousePos();
    float arrMousePos[] = { m_MousePos.x, m_MousePos.y };
    ImGui::InputFloat2("MousePos", arrMousePos, "%.0f");

    ImVec2 vDiff = ImVec2(m_MousePos.x - backgroundRectMin.x, m_MousePos.y - backgroundRectMin.y);

    // 마우스 위치의 아틀라스 픽셀좌표
    float PixelPos[] = { vDiff.x, vDiff.y };
    ImGui::InputFloat2("PixelPos", PixelPos, "%.0f");
}

