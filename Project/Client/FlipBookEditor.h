#pragma once
#include "EditorUI.h"
class FlipBookEditor :
    public EditorUI
{
public:
    FlipBookEditor();
    ~FlipBookEditor();


private:
    Ptr<CFlipBook>        m_flipBook = nullptr;

    class FlipBookViewer* m_viewer = nullptr;
    class FlipBookInspector* m_inspector = nullptr;

    float left_pane_width = 0;
    float right_pane_width = 0;
    float splitterThickness = 8;
    float m_prevViewerWidth = 0;

public:
    Ptr<CFlipBook> GetFlipBook() { return m_flipBook; }
    void SetFlipBook(Ptr<CFlipBook> _flipBook) { m_flipBook = _flipBook; }

private:
    ImVec2 Splitter(float thickness, float* leftSize, float* rightSize, float min_leftSize, float min_rightSize);

public:
    virtual void Init() override;
    virtual void Update() override;

    virtual void Activate() override;
    virtual void Deactivate() override;
};

