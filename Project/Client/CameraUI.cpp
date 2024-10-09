#include "pch.h"
#include "CameraUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CCamera.h>

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>

CameraUI::CameraUI()
    : ComponentUI(COMPONENT_TYPE::CAMERA)
    , m_ShowLayerCheck(false)
{
}

CameraUI::~CameraUI()
{
}

void CameraUI::Update()
{
    ImVec2 initial_content_pos = ImGui::GetCursorPos();




    Title();



    CCamera* pCam = GetTargetObject()->Camera();

    // 카메라 우선순위
    // m_Priority;

    // 레이어 설정
    LayerCheck();

    // 투영방식
    Projection();

    // 투영 범위






    float Width = pCam->GetWidth();
    ImGui::Text("Width");
    ImGui::SameLine(100);
    ImGui::InputFloat("##Width", &Width);
    pCam->SetWidth(Width);

    float Height = pCam->GetHeight();
    ImGui::Text("Height");
    ImGui::SameLine(100);
    ImGui::InputFloat("##Height", &Height);
    pCam->SetHeight(Height);

    float AR = pCam->GetAspectRatio();
    ImGui::Text("AspectRatio");
    ImGui::SameLine(100);

    char ARbuffer[50];
    sprintf_s(ARbuffer, 50, "%.3f", AR);
    //ImGui::InputFloat("##AspectRatio", &AR, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
    ImGui::InputText("##AspectRatio", ARbuffer, 50, ImGuiInputTextFlags_ReadOnly);
    


    float Far = pCam->GetFar();
    ImGui::Text("Far");
    ImGui::SameLine(100);
    ImGui::InputFloat("##Far", &Far);
    pCam->SetFar(Far);

    // Perspective 전용
    float FOV = pCam->GetFOV();
    FOV = (FOV / XM_PI) * 180.f;

    bool IsPerspective = pCam->GetProjType() == PROJ_TYPE::PERSPECTIVE;
    ImGui::BeginDisabled(!IsPerspective);
    ImGui::Text("FOV");
    ImGui::SameLine(100);
    ImGui::InputFloat("##FOV", &FOV);
    ImGui::EndDisabled();


    ImGui::BeginDisabled(IsPerspective);
    float Scale = pCam->GetScale();
    ImGui::Text("Scale");
    ImGui::SameLine(100);
    ImGui::InputFloat("##Scale", &Scale);
    pCam->SetScale(Scale);
    ImGui::EndDisabled();


    ImVec2 last_content_pos = ImGui::GetCursorPos();
    ImVec2 content_size = ImVec2(last_content_pos.x - initial_content_pos.x, last_content_pos.y - initial_content_pos.y);

    SetChildSize(content_size);
}

void CameraUI::LayerCheck()
{
    ImGui::Text("Layer Check");
    ImGui::SameLine(100);
    if (ImGui::Button("Show##LayerCheckBtn", ImVec2(50.f, 18.f)))
    {
        m_ShowLayerCheck = !m_ShowLayerCheck;
    }

    if (!m_ShowLayerCheck)
        return;

    ImGui::BeginChild("##LayerCheck", ImVec2(300, 400), ImGuiChildFlags_None);
    CCamera* pCam = GetTargetObject()->Camera();

    bool bLayer[32] = {};
    for (int i = 0; i < 32; ++i)
    {
        bLayer[i] = pCam->GetLayerCheck(i);
    }


    int ChangedIdx = -1;
    if (ImGui::BeginTable("##LayerCheckTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        for (int i = 0; i < 32; i++)
        {
            char label[32];
            sprintf_s(label, "Layer %d", i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Selectable(label, &bLayer[i], ImGuiSelectableFlags_SpanAllColumns))
            {
                ChangedIdx = i;
            }

            wstring wlayerName = CLevelMgr::GetInst()->GetCurrentLevel()->GetLayer(i)->GetName();
            string layerName(wlayerName.begin(), wlayerName.end());
            ImGui::TableNextColumn();
            ImGui::Text(layerName.c_str());
        }
        ImGui::EndTable();
    }

    if (ChangedIdx != -1)
    {
        pCam->SetLayer(ChangedIdx, bLayer[ChangedIdx]);
    }
    ImGui::EndChild();
}

void CameraUI::Projection()
{
    CCamera* pCam = GetTargetObject()->Camera();
    PROJ_TYPE Type = pCam->GetProjType();

    const char* items[] = { "Orthographic", "Perspective" };
    const char* combo_preview_value = items[Type];

    ImGui::Text("Projection");
    ImGui::SameLine(100);
    ImGui::SetNextItemWidth(180);

    if (ImGui::BeginCombo("##ProjectionCombo", combo_preview_value))
    {
        for (int i = 0; i < 2; i++)
        {
            const bool is_selected = (Type == i);

            if (ImGui::Selectable(items[i], is_selected))
            {
                Type = (PROJ_TYPE)i;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    pCam->SetProjType(Type);
}
