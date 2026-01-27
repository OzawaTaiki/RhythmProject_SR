#include "SelectUI.h"

void SelectUI::Initialize()
{
    musicSelectUI_ =  std::make_unique<MusicSelectUI>();
    musicSelectUI_->Initialize();
}

void SelectUI::Update(float deltaTime)
{
    musicSelectUI_->Update(deltaTime);
}

void SelectUI::Draw()
{
    musicSelectUI_->Draw();
}
