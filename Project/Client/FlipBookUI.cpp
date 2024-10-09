#include "pch.h"
#include "FlipBookUI.h"

FlipBookUI::FlipBookUI()
	: AssetUI(ASSET_TYPE::FLIPBOOK)
{
}

FlipBookUI::~FlipBookUI()
{
}

void FlipBookUI::Update()
{
	Title();
}