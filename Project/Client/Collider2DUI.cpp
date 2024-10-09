#include "pch.h"
#include "Collider2DUI.h"


Collider2DUI::Collider2DUI()
	: ComponentUI(COMPONENT_TYPE::COLLIDER2D)
{
}

Collider2DUI::~Collider2DUI()
{
}

void Collider2DUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();



	Title();

	// Offset Scale

	// Offset Pos

	// Independent Scale CheckBox



	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(content_size);
}