#include "pch.h"
#include "ParticleSystemUI.h"


ParticleSystemUI::ParticleSystemUI()
	: ComponentUI(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
}

ParticleSystemUI::~ParticleSystemUI()
{
}

void ParticleSystemUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();







	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

	SetChildSize(content_size);
}