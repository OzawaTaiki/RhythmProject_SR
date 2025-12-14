#include "GameUI.h"

#include <Debug/ImGuiDebugManager.h>
#include <Debug/ImguITools.h>

void GameUI::Initialize()
{
    FontConfig conf = {};
    conf.atlasSize = Vector2(512, 512);
    conf.fontFilePath = "Resources/Fonts/NotoSansJP-Regular.ttf";
    conf.fontSize = 64.0f;
    textGenerator_.Initialize(conf);

    comboValueAnimation_ = std::make_unique<AnimationSequence>("ComboValueAnim");
    comboValueAnimation_->Initialize("Resources/Data/UI/");//保存ディレクトリ



    jsonBinder_ = std::make_unique<JsonBinder>("GameUI", "Resources/Data/UI/");

    jsonBinder_->RegisterVariable("ComboValueTextParam", &comboValueParam_);
    jsonBinder_->RegisterVariable("ComboTextParam", &comboTextParam_);

}

void GameUI::Update(int32_t combo, float deltaTime)
{
    if (combo != comboValue_)
    {
        comboValueAnimation_->SetCurrentTime(0.0f);// リセット

    }
    comboValueAnimation_->Update(deltaTime);

    comboValueParam_.scale = comboValueAnimation_->GetValue<Vector2>("Scale");

    comboValue_ = combo;

    ImGui();
}

void GameUI::Draw()
{
    textGenerator_.Draw(L"Combo", comboTextParam_);
    textGenerator_.Draw(std::format(L"{}", comboValue_), comboValueParam_);
}

void GameUI::ImGui()
{
#ifdef _DEBUG
    if (ImGuiDebugManager::GetInstance()->Begin("GameUI"))
    {
        ImGui::SeparatorText("value");
        ImGui::PushID("val");
        comboValueParam_.ImGui();
        ImGui::PopID();


        ImGui::SeparatorText("text");
        ImGui::PushID("tex");
        comboTextParam_.ImGui();
        ImGui::PopID();

        if (ImGui::Button("Save"))
        {
            jsonBinder_->Save();
            comboValueAnimation_->Save();
        }

        ImGuiTool::TimeLine("ComboValueAnim", comboValueAnimation_.get());

        ImGui::End();

    }
#endif // _DEBUG
}
