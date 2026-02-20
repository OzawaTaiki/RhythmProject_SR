#include "JudgeLine.h"

#include <Debug/ImGuiDebugManager.h>

using namespace Engine;

JudgeLine::JudgeLine()
{
#ifdef _DEBUG
    // デバッグウィンドウの登録
    ImGuiDebugManager::GetInstance()->AddDebugWindow("JudgeLine", [&]() { DebugWindow(); });
#endif // _DEBUG
}

JudgeLine::~JudgeLine()
{
#ifdef _DEBUG
    // デバッグウィンドウの登録解除
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("JudgeLine");
#endif // _DEBUG
}

void JudgeLine::Initialize()
{
    lineDrawer_ = LineDrawer::GetInstance();
}

void JudgeLine::Draw()
{
    CalculateLine();
    // TODO : ラインじゃない描画 モデルやスプライトなど

    lineDrawer_->RegisterPoint(linePoints_[0], linePoints_[1], color_);
}

void JudgeLine::CalculateLine()
{
    // 変更がなければ計算しない
    if(!dirty_)
        return;

    dirty_ = false;

    linePoints_.clear();

    linePoints_.push_back({ 30, 0, position_ });
    linePoints_.push_back({ -30, 0, position_ });
}

void JudgeLine::DebugWindow()
{
#ifdef _DEBUG

    ImGui::PushID(this);

    if (ImGui::DragFloat("Position", &position_, 0.01f))
        dirty_ = true;

    if (ImGui::ColorEdit4("Color", &color_.x))
        dirty_ = true;

    ImGui::PopID();

#endif // _DEBUG
}
