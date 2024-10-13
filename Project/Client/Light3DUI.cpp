#include "pch.h"
#include "Light3DUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CLight3D.h>

Light3DUI::Light3DUI()
	: ComponentUI(COMPONENT_TYPE::LIGHT3D)
{
}

Light3DUI::~Light3DUI()
{
}

void Light3DUI::Update()
{
	ImVec2 initial_content_pos = ImGui::GetCursorPos();


	Title();

	CLight3D* pLight = GetTargetObject()->Light3D();

	// ���� ����
	LIGHT_TYPE light_type = pLight->GetLightType();

	const char* items[] = { "DIRECTIONAL", "POINT", "SPOT" };
	const char* combo_preview_value = items[(UINT)light_type];

	if (ImGui::BeginCombo("##3DLightTypeCombo", combo_preview_value))
	{
		for (int i = 0; i < 3; i++)
		{
			const bool is_selected = ((UINT)light_type == i);

			if (ImGui::Selectable(items[i], is_selected))
			{
				light_type = (LIGHT_TYPE)i;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	pLight->SetLightType(light_type);

	// ���� ��������	
	const tLightInfo& info = pLight->GetLightInfo();

	ImGui::Text("Light Color");
	ImGui::SameLine(100);
	ImGui::ColorEdit3("##LightColor", info.light.Color);

	ImGui::Text("Light Ambient");
	ImGui::SameLine(100);
	ImGui::ColorEdit3("##LightAmbient", info.light.Ambient);

	// ������ �ݰ� ( Point, Spot )
	ImGui::BeginDisabled(light_type == LIGHT_TYPE::DIRECTIONAL);

	ImGui::Text("Light Radius");
	ImGui::SameLine(100);
	ImGui::DragFloat("##DragRadius", (float*)&info.Radius, 1.f);

	ImGui::EndDisabled();


	// ������ ���� ����
	ImGui::BeginDisabled(light_type != LIGHT_TYPE::SPOT);

	float Angle = info.Angle;
	Angle = (Angle / XM_PI) * 180.f;

	ImGui::Text("Light Angle");
	ImGui::SameLine(100);
	ImGui::DragFloat("##DragAngle", &Angle, 0.1f);

	Angle = (Angle / 180.f) * XM_PI;
	pLight->SetAngle(Angle);

	ImGui::Text("Specular Coefficient");
	ImGui::SameLine(100);
	ImGui::DragFloat("##SpecularCoef", (float*)&info.light.SpecularCoef, 0.01f, 0.f, 1.f);

	ImGui::EndDisabled();



	ImVec2 last_content_pos = ImGui::GetCursorPos();
	ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);
	SetChildSize(content_size);
}


