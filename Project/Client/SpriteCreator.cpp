#include "pch.h"
#include "SpriteCreator.h"


#include "CEditorMgr.h"
#include "SC_AtlasView.h"
#include "SC_Detail.h"

SpriteCreator::SpriteCreator()
{
	UseMenuBar(true);
	m_HorizontalScrollbar = true;
}

SpriteCreator::~SpriteCreator()
{
}

void SpriteCreator::Init()
{
	m_AtlasView = (SE_AtlasView*)CEditorMgr::GetInst()->FindEditorUI("SC_AtlasView");
	m_Detail = (SE_Detail*)CEditorMgr::GetInst()->FindEditorUI("SC_Detail");

	m_AtlasView->SetMove(false);
	m_Detail->SetMove(true);

	m_AtlasView->m_Owner = this;
	m_Detail->m_Owner = this;

	m_Detail->SetAtlasTex(CAssetMgr::GetInst()->Load<CTexture>(L"texture\\link.png", L"texture\\link.png"));
}

void SpriteCreator::Update()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Window"))
		{
			bool AtlasView = m_AtlasView->IsActive();
			bool Detail = m_Detail->IsActive();

			if (ImGui::MenuItem("AtlasView", nullptr, &AtlasView))
			{
				m_AtlasView->SetActive(AtlasView);
			}

			if (ImGui::MenuItem("Detail", nullptr, &Detail))
			{
				m_Detail->SetActive(Detail);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void SpriteCreator::Activate()
{
	m_AtlasView->SetActive(true);
	m_Detail->SetActive(true);
}

void SpriteCreator::Deactivate()
{
	m_AtlasView->SetActive(false);
	m_Detail->SetActive(false);
}









// ====================================================================================================
// 스플리터 사용 버젼 
// ====================================================================================================

ImVec2 Splitter(float thickness, float* leftSize, float* rightSize, float min_leftSize, float min_rightSize)
{
	ImGui::SameLine();

	ImGui::Button("##splitter", ImVec2(thickness, ImGui::GetContentRegionAvail().y));

	static float initialMousePos = 0;
	static float initial_size1;
	static float initial_size2;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		initialMousePos = ImGui::GetIO().MousePos.x;
		initial_size1 = *leftSize;
		initial_size2 = *rightSize;
	}

	//ImGui::SetItemAllowOverlap();

	if (ImGui::IsItemActive())
	{
		{
			float delta = ImGui::GetIO().MousePos.x - initialMousePos;
			*leftSize = initial_size1 + delta;
			*rightSize = initial_size2 - delta;

			if (*leftSize < min_leftSize)
				*leftSize = min_leftSize;

			if (*rightSize < min_rightSize)
			{
				float diff = min_rightSize - *rightSize;
				*leftSize -= diff;
			}
		}
	}

	ImGui::SameLine();

	auto remainRegionAvail = ImGui::GetContentRegionAvail();

	// 오른쪽 칸에 실제로 사용될 rigthSize 업데이트 (전체 창 자체의 크기를 조절하면서 바뀔 수 있으므로)
	*rightSize = remainRegionAvail.x;

	return remainRegionAvail;
}

//void SpriteCreator::Update()
//{
//	ImVec2 available_size = ImGui::GetContentRegionAvail();
//	available_size.x -= 8.f;
//
//	static float left_pane_width = available_size.x * 0.2f;
//	static float right_pane_width = available_size.x * 0.8f;
//
//	static bool b1 = true;
//	static bool b2 = true;
//
//	ImGui::BeginChild("Left Pane", ImVec2(left_pane_width, 0), true, ImGuiWindowFlags_NoMove);
//	if (ImGui::BeginTabBar("MyTabBar"))
//	{
//		if (ImGui::BeginTabItem("Tab 1", &b1, ImGuiTabItemFlags_None))
//		{
//
//			ImGui::Text("This is the content of the left pane.");
//			ImGui::EndTabItem();
//		}
//		ImGui::EndTabBar();
//	}
//	ImGui::EndChild();
//	
//	ImVec2 remainRegionavail = Splitter(8.0f, &left_pane_width, &right_pane_width, 100.0f, 100.0f);
//
//	ImGui::BeginChild("Right Pane", remainRegionavail, true, ImGuiWindowFlags_NoMove);
//
//	if (ImGui::BeginTabBar("MyTabBar2"))
//	{
//		if (ImGui::BeginTabItem("Tab 2", &b2, ImGuiTabItemFlags_None))
//		{
//			ImGui::Text("This is the content of the right pane.");
//			ImGui::EndTabItem();
//
//		}
//
//		ImGui::EndTabBar();
//	}
//	ImGui::EndChild();
//}


