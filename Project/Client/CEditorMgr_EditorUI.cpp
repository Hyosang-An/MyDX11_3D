#include "pch.h"

#include "CEditorMgr.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include <Engine/CDevice.h>

#include "Inspector.h"
#include "Content.h"
#include "Outliner.h"
#include "ListUI.h"
#include "MenuUI.h"
#include "SpriteCreator.h"
#include "SC_AtlasView.h"
#include "SC_Detail.h"
#include "FlipBookEditor.h"

#include "SceneUI.h"

#include "ParamUI.h"

#include "IconsFontAwesome6/IconsFontAwesome6.h"

void CEditorMgr::InitImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }



    ImGui_ImplWin32_Init(CEngine::GetInst()->GetMainWnd());
    ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    /*io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);*/
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // �ѱ� ��Ʈ �߰� (�⺻��Ʈ�� ���)
    ImFontConfig config;
    //config.MergeMode = true;
    io.Fonts->AddFontFromFileTTF("..\\content\\fonts\\malgun.ttf", 16.0f, &config, io.Fonts->GetGlyphRangesKorean());

    // FontAwesome �۲� �߰�
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;

    io.Fonts->AddFontFromFileTTF("..\\content\\fonts\\fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);


	// Room No.703 �۲� �߰�
    io.Fonts->AddFontFromFileTTF("..\\content\\fonts\\Room No.703.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

    // Editor(Tool) �� UI ���� �� m_mapUI�� �߰�
    CreateEditorUI();
}

void CEditorMgr::ObserveContent()
{
    // ������ ��Ȳ�� �߻��ߴ��� Ȯ��
    DWORD dwStatus = WaitForSingleObject(m_hNotifyHandle, 0);

    // ������ ������ �������� �߻��ߴٸ�,
    if (dwStatus == WAIT_OBJECT_0)
    {
        // Content ������ �ִ� ��� ���°� �޸𸮿� �ε��Ǿ��ִ� ������ ����ȭ
        Content* pContent = (Content*)FindEditorUI("Content");
        pContent->Reload();

        // �ٽ� Content ������ �������� �߻��ϴ��� Ȯ���ϵ��� ��
        FindNextChangeNotification(m_hNotifyHandle);
    }
}


void CEditorMgr::CreateEditorUI()
{
    EditorUI* pUI = nullptr;



    // Content
    pUI = new Content;
    pUI->Init();
    pUI->SetName("Content");
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // Outliner
    pUI = new Outliner;
    pUI->SetName("Outliner");
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // ListUI
    pUI = new ListUI;
    pUI->SetName("List");
    pUI->SetActive(false);
    pUI->SetModal(true);
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // Inspector
    pUI = new Inspector;
    pUI->Init();
    pUI->SetName("Inspector");
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // Menu
    pUI = new MenuUI;
    pUI->Init();
    pUI->SetName("MainMenu");
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // SE_AtlasView
    pUI = new SE_AtlasView;
    pUI->Init();
    pUI->SetName("SC_AtlasView");
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // SE_Detail
    pUI = new SE_Detail;
    pUI->Init();
    pUI->SetName("SC_Detail");
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // SpriteCreator
    pUI = new SpriteCreator;
    pUI->Init();
    pUI->SetActive(false);
    pUI->SetName("SpriteCreator");
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    // FlipBookEditor
    pUI = new FlipBookEditor;
    pUI->Init();
    pUI->SetActive(false);
    pUI->SetName("FlipBookEditor");
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));



 //   // Scene UI Test ��
	//pUI = new SceneUI;
	//pUI->SetName("SceneUI");
	//m_mapUI.insert(make_pair(pUI->GetName(), pUI));
}


void CEditorMgr::ImGuiProgress()
{
    // Start ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // ParamUI ID Reset
    ParamUI::ResetID();

    // ImGui Tick
    ImGuiTick();

    // ImGui Render    
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}


EditorUI* CEditorMgr::FindEditorUI(const string& _Name)
{
    map<string, EditorUI*>::iterator iter = m_mapUI.find(_Name);

    if (iter == m_mapUI.end())
    {
        MessageBox(nullptr, (wstring(_Name.begin(), _Name.end()) + L" UI�� ã�� ����").c_str(), L"Error", MB_OK);
        return nullptr;
    }

    return iter->second;
}