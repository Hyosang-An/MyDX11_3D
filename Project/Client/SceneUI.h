#pragma once
#include "EditorUI.h"
class SceneUI :
    public EditorUI
{
public:
	virtual void Update() override;

public:
	SceneUI();
	~SceneUI();
};

