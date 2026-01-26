#include "SelectUI.h"

void SelectUI::Initialize()
{
    musicSelectUI_ =  std::make_unique<MusicSelectUI>();
    musicSelectUI_->Initialize();
}

void SelectUI::Update()
{
    musicSelectUI_->Update();
}

void SelectUI::Draw()
{
    musicSelectUI_->Draw();
}
