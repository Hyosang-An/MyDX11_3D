#include "pch.h"
#include "SC_AtlasView.h"

#include "SC_Detail.h"


SE_AtlasView::SE_AtlasView()
{
	m_HorizontalScrollbar = true;
}

SE_AtlasView::~SE_AtlasView()
{
}

void SE_AtlasView::Init()
{
}

void SE_AtlasView::Update()
{
	if (nullptr == m_AtlasTex)
		return;

	WheelCheck();

	// 이미지	
	ImVec2 uv_min = ImVec2(0.0f, 0.0f);
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);

	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 border_col = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

	ImGui::Image(m_AtlasTex->GetSRV().Get(), ImVec2((m_AtlasTex->Width() * m_Ratio), m_AtlasTex->Height() * m_Ratio)
		, uv_min, uv_max, tint_col, border_col);

	// SelectSpriteArea
	SelectSpriteArea();

	// 선택한 영역에 사각형 그리기
	DrawSelectRect();
}

void SE_AtlasView::SetAtlasTex(Ptr<CTexture> _Tex)
{
	if (m_AtlasTex == _Tex)
		return;

	m_AtlasTex = _Tex;

	//m_WidthSize = (float)m_AtlasTex->Width();
}


void SE_AtlasView::WheelCheck()
{
	if (ImGui::IsWindowHovered() == false || ImGui::IsKeyDown(ImGuiKey_LeftCtrl) == false)
		return;

	if (0 < ImGui::GetIO().MouseWheel)
	{
		m_Ratio += 0.05f;
	}

	if (0 > ImGui::GetIO().MouseWheel)
	{
		m_Ratio -= 0.05f;
	}

	if (3.f < m_Ratio)
		m_Ratio = 3.f;
	if (m_Ratio < 0.1f)
		m_Ratio = 0.1f;
}

void SE_AtlasView::SelectSpriteArea()
{
	bool onImage = ImGui::IsItemHovered();

	// Image 위젯 좌상단 좌표
	m_imageRectMin = ImGui::GetItemRectMin();
	auto imageRectMax = ImGui::GetItemRectMax();
	float ArrImageMin[] = { m_imageRectMin.x, m_imageRectMin.y };
	ImGui::InputFloat2("ImageMin", ArrImageMin, "%.0f");


	// 현재 마우스 위치
	m_MousePos = ImGui::GetMousePos();
	float arrMousePos[] = { m_MousePos.x, m_MousePos.y };
	ImGui::InputFloat2("MousePos", arrMousePos, "%.0f");

	ImVec2 vDiff = ImVec2(m_MousePos.x - m_imageRectMin.x, m_MousePos.y - m_imageRectMin.y);
	vDiff = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);

	// 마우스 위치의 아틀라스 픽셀좌표	
	float PixelPos[] = { vDiff.x, vDiff.y };
	ImGui::InputFloat2("PixelPos", PixelPos, "%.0f");

	if (onImage)
	{
		// ========================================================
		// 사각형 수동 만들기
		// ========================================================
		if (GetDetail()->GetSpriteSelectMode() == SpriteSlectMode::ClickAndDrag)
		{

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_stripeBoxLTonOriginalTex = ImGui::GetMousePos();
				ImVec2 vDiff = ImVec2(m_stripeBoxLTonOriginalTex.x - m_imageRectMin.x, m_stripeBoxLTonOriginalTex.y - m_imageRectMin.y);
				m_stripeBoxLTonOriginalTex = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);
			}

			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				m_stripeBoxRBonOriginalTex = ImGui::GetMousePos();
				ImVec2 vDiff = ImVec2(m_stripeBoxRBonOriginalTex.x - m_imageRectMin.x, m_stripeBoxRBonOriginalTex.y - m_imageRectMin.y);
				m_stripeBoxRBonOriginalTex = ImVec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);
			}
		}

		// ========================================================
		// 사각형 자동 만들기
		// ========================================================
		else if (GetDetail()->GetSpriteSelectMode() == SpriteSlectMode::AutoSelectOnClick)
		{

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				Vec2 vPixelPos = Vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
				ImVec2 vDiff = ImVec2(vPixelPos.x - m_imageRectMin.x, vPixelPos.y - m_imageRectMin.y);
				vPixelPos = Vec2(vDiff.x / m_Ratio, vDiff.y / m_Ratio);

				if (0.f <= vPixelPos.x && vPixelPos.x < m_AtlasTex->Width()
					&& 0.f <= vPixelPos.y && vPixelPos.y < m_AtlasTex->Height())
				{
					CalcSpriteSize(vPixelPos);
				}
			}
		}

		// ========================================================
		// 사각형 수동 지정
		// ========================================================
		else if (GetDetail()->GetSpriteSelectMode() == SpriteSlectMode::ManualSpecification)
		{

		}
	}
}

void SE_AtlasView::DrawSelectRect()
{
	if (GetDetail()->GetSpriteSelectMode() == SpriteSlectMode::ClickAndDrag || GetDetail()->GetSpriteSelectMode() == SpriteSlectMode::AutoSelectOnClick)
	{
		ImVec2 BoxLTPosOnUI = ImVec2(m_stripeBoxLTonOriginalTex.x * m_Ratio + m_imageRectMin.x, m_stripeBoxLTonOriginalTex.y * m_Ratio + m_imageRectMin.y);
		ImVec2 BoxRBPosOnUI = ImVec2(m_stripeBoxRBonOriginalTex.x * m_Ratio + m_imageRectMin.x, m_stripeBoxRBonOriginalTex.y * m_Ratio + m_imageRectMin.y);

		if (BoxLTPosOnUI.x == BoxRBPosOnUI.x && BoxLTPosOnUI.y == BoxRBPosOnUI.y)
			return;

		ImGui::GetWindowDrawList()->AddRect(BoxLTPosOnUI, BoxRBPosOnUI
			, ImGui::GetColorU32(ImVec4(1.f, 1.f, 0.f, 1.f)), 0.f, 0, 1.f);
	}

	else if (GetDetail()->GetSpriteSelectMode() == SpriteSlectMode::ManualSpecification)
	{
		Vec2 vStartLT = GetDetail()->GetStartLTandSize().first;
		Vec2 vSize = GetDetail()->GetStartLTandSize().second;
		int count = GetDetail()->GetCount();

		if (vSize.x * vSize.y == 0)
			return;

		for (int i = 0; i < count; i++)
		{
			ImVec2 BoxLTPosOnUI = ImVec2(vStartLT.x * m_Ratio + m_imageRectMin.x, vStartLT.y * m_Ratio + m_imageRectMin.y);
			ImVec2 BoxRBPosOnUI = ImVec2((vStartLT.x + vSize.x) * m_Ratio + m_imageRectMin.x, (vStartLT.y + vSize.y) * m_Ratio + m_imageRectMin.y);

			ImGui::GetWindowDrawList()->AddRect(BoxLTPosOnUI, BoxRBPosOnUI
				, ImGui::GetColorU32(ImVec4(1.f, 1.f, 0.f, 1.f)), 0.f, 0, 1.f);

			vStartLT.x += vSize.x;
		}
	}
}


void SE_AtlasView::CalcSpriteSize(Vec2 _PixelPos)
{
	Vec2 dirDelta[4]{ {0, 1}, {0, -1}, {1, 0}, {-1, 0} };

	//float minLeft = m_AtlasTex->Width() - 1.f;
	//float minTop = m_AtlasTex->Height() - 1.f;
	//float maxRight = 0.f;
	//float maxBottom = 0.f;

	float minLeft = _PixelPos.x;
	float minTop = _PixelPos.y;
	float maxRight = _PixelPos.x;
	float maxBottom = _PixelPos.y;


	// 등록아이디 검사용 Set Clear
	//m_PixelID.clear();
	set<Vec2>       visitedSet;

	// Quene 에 클릭한 최초의 픽셀좌표 입력
	list<Vec2>	queue;
	queue.push_back(_PixelPos);
	// 방문처리
	visitedSet.insert(_PixelPos);

	while (!queue.empty())
	{
		Vec2 vPixelPos = queue.front();
		queue.pop_front();


		// 작업
		if (vPixelPos.x < minLeft)
			minLeft = vPixelPos.x;
		if (vPixelPos.x > maxRight)
			maxRight = vPixelPos.x;
		if (vPixelPos.y < minTop)
			minTop = vPixelPos.y;
		if (vPixelPos.y > maxBottom)
			maxBottom = vPixelPos.y;


		// 주변 픽셀 조건 확인 및 추가
		for (int i = 0; i < 4; i++)
		{
			Vec2 nextPixelPos = vPixelPos + dirDelta[i];

			// 해당 픽셀이 아틀라스 해상도 영역을 벗어난 좌표라면 건너뛰기
			if (nextPixelPos.x < 0 || m_AtlasTex->Width() <= nextPixelPos.x
				|| nextPixelPos.y < 0 || m_AtlasTex->Height() <= nextPixelPos.y)
				continue;

			// 이미 방문했다면 건너뛰기
			if (visitedSet.find(nextPixelPos) != visitedSet.end())
				continue;

			// 해당 픽셀의 알파값이 0이라면 건너뛰기
			int PixelIdx = m_AtlasTex->Width() * (int)nextPixelPos.y + (int)nextPixelPos.x;
			if (0 == m_AtlasTex->GetPixels()[PixelIdx].a)
				continue;

			// 모든 조건을 만족하는 픽셀이면 큐에 넣고 방문처리
			queue.push_back(nextPixelPos);
			visitedSet.insert(nextPixelPos);
		}
	}

	m_stripeBoxLTonOriginalTex = ImVec2(minLeft, minTop);
	m_stripeBoxRBonOriginalTex = ImVec2(maxRight, maxBottom);
}


std::pair<Vec2, Vec2> SE_AtlasView::GetSpriteBoxPosOnOriginalTex()
{
	return std::pair<Vec2, Vec2>({ m_stripeBoxLTonOriginalTex.x, m_stripeBoxLTonOriginalTex.y }, {m_stripeBoxRBonOriginalTex.x, m_stripeBoxRBonOriginalTex.y});
}