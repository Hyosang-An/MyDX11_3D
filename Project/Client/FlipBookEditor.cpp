#include "pch.h"
#include "FlipBookEditor.h"
#include "FlipBookViewer.h"
#include "FlipBookInspector.h"

FlipBookEditor::FlipBookEditor()
{
	UseMenuBar(true);
	m_isSizeConstrained = true;
	m_minSize = ImVec2(400, 500);
	m_maxSize = ImVec2(FLT_MAX, FLT_MAX);
}

FlipBookEditor::~FlipBookEditor()
{
	if (m_inspector != nullptr)
		delete m_inspector;

	if (m_viewer != nullptr)
		delete m_viewer;
}



void FlipBookEditor::Init()
{
	m_viewer = new FlipBookViewer;
	m_viewer->m_owner = this;

	m_inspector = new FlipBookInspector;
	m_inspector->m_owner = this;

	m_viewer->m_inspector = m_inspector;
	m_inspector->m_viewer = m_viewer;

	m_viewer->Init();
	m_inspector->Init();

}

void FlipBookEditor::Update()
{
	static bool first_update = true;
	
	if (first_update)
	{
		float available_width = ImGui::GetContentRegionAvail().x;
		
		available_width -= (splitterThickness + 16);


		if (available_width > 500)
		{
			right_pane_width = 300;
			left_pane_width = available_width - right_pane_width;
		}

		else
		{
			right_pane_width = left_pane_width = available_width * 0.5f;
		}
			

		first_update = false;
	}


	// 메뉴바
	bool viewerOpen = m_viewer->IsActive();
	bool inspectorOpen = m_inspector->IsActive();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Window"))
		{


			if (ImGui::MenuItem("FlipBook Viewer", nullptr, &viewerOpen))
			{
				// inspector가 켜져 있는 상태에서 viewer가 꺼질 때 기존 viewer 너비 저장
				if (viewerOpen == false && inspectorOpen)
					m_prevViewerWidth = left_pane_width;

				if (viewerOpen == true)
					left_pane_width = m_prevViewerWidth;

				m_viewer->SetActive(viewerOpen);
			}

			if (ImGui::MenuItem("FlipBook Inspector", nullptr, &inspectorOpen))
			{
				// viewer가 켜져 있는 상태에서 inspector가 꺼질 때 기존 viewer 너비 저장
				if (inspectorOpen == false && viewerOpen)
					m_prevViewerWidth = left_pane_width;

				if (inspectorOpen == true)
					left_pane_width = m_prevViewerWidth;

				m_inspector->SetActive(inspectorOpen);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}




	// Viewer 창
	if (viewerOpen)
	{
		// Viewer만 켜진 경우
		if (!inspectorOpen)
		{
			left_pane_width = ImGui::GetContentRegionAvail().x;
		}

		if (ImGui::BeginChild("FlipBook Viewer", ImVec2(left_pane_width, 0), ImGuiChildFlags_None))
		{
			if (ImGui::BeginTabBar("FlipBook Viewer TabBar"))
			{
				if (ImGui::BeginTabItem("FlipBook Viewer", &viewerOpen, ImGuiTabItemFlags_None))
				{
					m_viewer->Update();

					if (viewerOpen == false)
					{
						// inspector가 켜져 있는 상태에서 viewer가 꺼질 때 기존 viewer 너비 저장
						if (inspectorOpen)
							m_prevViewerWidth = left_pane_width;

						m_viewer->SetActive(viewerOpen);

					}

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
	}
	

	// Inspector 창

	if (!inspectorOpen)
		return;

	ImVec2 remainRegionavail;
	if (viewerOpen && inspectorOpen)
		remainRegionavail = Splitter(splitterThickness, &left_pane_width, &right_pane_width, 100.0f, 100.0f);
	else
		remainRegionavail = ImGui::GetContentRegionAvail();


	if (ImGui::BeginChild("FlipBook Inspector", remainRegionavail, ImGuiChildFlags_None))
	{
		if (ImGui::BeginTabBar("FlipBook Inspector TabBar"))
		{
			if (ImGui::BeginTabItem("FlipBook Inspector", &inspectorOpen, ImGuiTabItemFlags_None))
			{

				m_inspector->Update();

				if (inspectorOpen == false)
				{
					// viewer가 켜져 있는 상태에서 inspector가 꺼질 때 기존 viewer 너비 저장
					if (viewerOpen)
						m_prevViewerWidth = left_pane_width;

					m_inspector->SetActive(inspectorOpen);
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::EndChild();
}

void FlipBookEditor::Activate()
{
}

void FlipBookEditor::Deactivate()
{
}



ImVec2 FlipBookEditor::Splitter(float thickness, float* leftSize, float* rightSize, float min_leftSize, float min_rightSize)
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