#include "GameCompleteEffect.h"
#include <Core/WinApp/WinApp.h>
#include <Debug/ImguITools.h>
#include <Debug/ImGuiDebugManager.h>

void GameCompleteEffect::Initialize()
{
    sequence_ = std::make_unique<AnimationSequence>("GameCompleteEffect");
    sequence_->Initialize("Resources/Data/Game/");

    isEffectActive_ = false;
    isComplete_ = false;

    background_ = std::make_unique<UIImageElement>("GameComplete_BG",
                                                   Vector2(0.0f, 0.0f),
                                                   WinApp::kWindowSize_);
    background_->Initialize();

    banner_ = std::make_unique<UIImageElement>("GameComplete_Banner",
                                               Vector2(WinApp::kWindowSize_.x / 2.0f,
                                                       WinApp::kWindowSize_.y / 2.0f),
                                               Vector2(WinApp::kWindowSize_.x, 200.0f));
    banner_->Initialize();

    FontConfig fontConfig{};
    fontConfig.fontSize = 128.0f;
    text_ = std::make_unique<UITextElement>("GameComplete_Text",
                                            WinApp::kWindowSize_ / 2.0f,
                                            "Temp",
                                            fontConfig);
    text_->Initialize();

}

void GameCompleteEffect::StartEffect(const std::map<JudgeType, int32_t>& judgeResult)
{
    isEffectActive_ = true;
    isComplete_ = false;

    sequence_->SetCurrentTime(0.0f);
    text_->SetText(GenerateResultText(judgeResult));
}

void GameCompleteEffect::Update(float deltaTime)
{
#ifdef _DEBUG

    if(ImGuiDebugManager::GetInstance()->Begin("GameCompleteEffect Debug"))
    {
        if (ImGui::Button("Reset"))
        {
            sequence_->SetCurrentTime(0.0f);
        }
        ImGui::Checkbox("Effect Active", &isEffectActive_);
        ImGuiTool::TimeLine("GameCompleteEffect", sequence_.get());
        ImGui::End();
    }

#endif
    if (!isEffectActive_)
        return;

    sequence_->Update(deltaTime);
    if (sequence_->IsEnd())
    {
        isComplete_ = true;
        isEffectActive_ = false;
    }
    UpdateElements();
}

void GameCompleteEffect::Draw()
{
    if (!isEffectActive_)
        return;

    background_->Draw();
    banner_->Draw();
    text_->Draw();
}

bool GameCompleteEffect::IsEffectComplete() const
{
    return isComplete_;
}

void GameCompleteEffect::UpdateElements()
{
    Vector4 bgColor = sequence_->GetValue<Vector4>("BG_color");
    background_->SetColor(bgColor);
    background_->Update();

    Vector2 bannerSize = sequence_->GetValue<Vector2>("Banner_Size");
    banner_->SetSize(bannerSize);
    banner_->Update();

    Vector2 textScale = sequence_->GetValue<Vector2>("Text_Scale");
    auto textParam = text_->GetTextParam();
    textParam.scale = textScale;
    text_->SetTextParam(textParam);
    text_->Update();
}

std::string GameCompleteEffect::GenerateResultText(const std::map<JudgeType, int32_t>& judgeResult) const
{
    if (judgeResult.at(JudgeType::Miss) == 0)
    {
        if (judgeResult.at(JudgeType::Good) + judgeResult.at(JudgeType::Bad) > 0)
        {
            return "Full Combo!";
        }
        else
        {
            return "All Perfect!";
        }
    }
    else
    {
        return "Game Complete";
    }
}
