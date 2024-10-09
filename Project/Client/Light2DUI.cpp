#include "pch.h"
#include "Light2DUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CLight2D.h>

Light2DUI::Light2DUI()
	: ComponentUI(COMPONENT_TYPE::LIGHT2D)
{
}

Light2DUI::~Light2DUI()
{
}

void Light2DUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();



	Title();

	CLight2D* pLight = GetTargetObject()->Light2D();

	// 광원 종류
	LIGHT_TYPE	Type = pLight->GetLightType();

	const char* items[] = { "DIRECTIONAL", "POINT", "SPOT" };
	const char* combo_preview_value = items[(UINT)Type];

	ImGui::Text("Light Type");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(180);

	if (ImGui::BeginCombo("##LightTypeCombo", combo_preview_value))
	{
		for (int i = 0; i < 3; i++)
		{
			const bool is_selected = ((UINT)Type == i);

			if (ImGui::Selectable(items[i], is_selected))
			{
				Type = (LIGHT_TYPE)i;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	pLight->SetLightType(Type);


	// 광원 색상정보	
	const tLightInfo& info = pLight->GetLightInfo();

	ImGui::Text("Light Color");
	ImGui::SameLine(100);
	ImGui::ColorEdit3("##LightColor", info.light.Color);

	ImGui::Text("Light Ambient");
	ImGui::SameLine(100);
	ImGui::ColorEdit3("##LightAmbient", info.light.Ambient);

	// 광원의 반경 ( Point, Spot )
	ImGui::BeginDisabled(Type == LIGHT_TYPE::DIRECTIONAL);

	ImGui::Text("Light Radius");
	ImGui::SameLine(100);
	ImGui::DragFloat("##DragRadius", (float*)&info.Radius, 1.f);

	ImGui::EndDisabled();


	// 광원의 범위 각도
	ImGui::BeginDisabled(Type != LIGHT_TYPE::SPOT);

	float Angle = info.Angle;
	Angle = (Angle / XM_PI) * 180.f;

	ImGui::Text("Light Angle");
	ImGui::SameLine(100);
	ImGui::DragFloat("##DragAngle", &Angle, 0.1f);

	Angle = (Angle / 180.f) * XM_PI;
	pLight->SetAngle(Angle);

	ImGui::EndDisabled();


	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);
	SetChildSize(content_size);
}