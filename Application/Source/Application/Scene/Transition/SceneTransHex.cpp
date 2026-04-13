#include "SceneTransHex.h"
#include <System/Time/Time.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Core/WinApp/WinApp.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <Math/Random/RandomGenerator.h>

#include <Debug/ImGuiDebugManager.h>

using namespace Engine;

SceneTransHex::SceneTransHex()
{
    ImGuiDebugManager::GetInstance()->AddDebugWindow("SceneTransHex", [this]() { ImGui(); });
}

void SceneTransHex::Initialize()
{
    InitJsonBinder();

    Vector2 windowSize = WinApp::kWindowSize_;
    // グリッドの幅と高さを計算
    gridWidth_ = static_cast<int32_t>(windowSize.x / (hexagonSize_ * 0.75f + spacing_));
    gridHeight_ = static_cast<int32_t>(windowSize.y / (hexagonSize_ * 0.5f + spacing_));

    totalHexagons_ = gridWidth_ * gridHeight_;

    // 六角形のスプライトを作成
    CreateHexagons();

    LayerSystem::CreateLayer("SceneTransition", 100000); // トランジション用のレイヤーを作成

}

void SceneTransHex::Update()
{
    if (!playing_)
        return;

    // 横列当たりの時間
    const float rowDuration = duration_ / static_cast<float>(gridHeight_);

    float delta = Time::GetDeltaTime<float>();
    if (canSwitch_)
        delta *= -1.0f; // 折り返し後は時間を逆に進める

    elapsedTime_ += delta;

    for (const auto& hexagon : hexagons_)
    {
        if (elapsedTime_ >= hexagon->delay)
        {
            float localElapsed = elapsedTime_ - hexagon->delay;
            float alpha =std::clamp(localElapsed / rowDuration, 0.0f, 1.0f);

            hexagon->sprite->SetColor(Vector4(0, 0, 0, alpha));
            hexagon->sprite->Update();
        }
    }

    if (elapsedTime_ >= duration_)
    {
        // 折り返し
        canSwitch_ = true;
        elapsedTime_ = duration_;
    }
    else if (elapsedTime_ <= 0.0f)
    {
        isEnd_ = true;
        elapsedTime_ = 0.0f;
    }
}

void SceneTransHex::Draw()
{
    if (!playing_)
        return;

    LayerSystem::SetLayer("SceneTransition");

    for (const auto& hexagon : hexagons_)
    {
        hexagon->sprite->Draw();
    }
}

void SceneTransHex::Start()
{
    playing_ = true;
    elapsedTime_ = 0.0f;
    isEnd_ = false;
    canSwitch_ = false;

    for (const auto& hexagon : hexagons_)
    {
        hexagon->elapsedTime = 0.0f; // アニメーションの経過時間をリセット
        hexagon->sprite->SetColor(Vector4(0, 0, 0, 0)); // 初期は透明
    }
}

void SceneTransHex::End()
{
    playing_ = false;
    elapsedTime_ = 0.0f;
    isEnd_ = false;
    canSwitch_ = false;

    for (const auto& hexagon : hexagons_)
    {
        hexagon->elapsedTime = 0.0f; // アニメーションの経過時間をリセット
        hexagon->sprite->SetColor(Vector4(0, 0, 0, 1)); // 初期は不透明
    }
}

void SceneTransHex::ImGui()
{
    ImGui::Begin("SceneTransHex");
    ImGui::DragFloat("Duration", &duration_, 0.1f, 0.1f, 10.0f);
    ImGui::DragFloat("Hexagon Size", &hexagonSize_, 1.0f, 10.0f, 200.0f);
    ImGui::DragFloat("Spacing", &spacing_, 1.0f, 0.0f, 50.0f);
    ImGui::DragInt("Phase Count", &phaseCount_, 1, 1, 10);
    if (ImGui::Button("Start Transition"))
    {
        Start();
    }
    ImGui::SameLine();
    if (ImGui::Button("End Transition"))
    {
        End();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Hexagons"))
    {
        Vector2 windowSize = WinApp::kWindowSize_;
        // グリッドの幅と高さを計算
        gridWidth_ = static_cast<int32_t>(windowSize.x / (hexagonSize_ * 0.75f + spacing_));
        gridHeight_ = static_cast<int32_t>(windowSize.y / (hexagonSize_ * 0.5f + spacing_));

        totalHexagons_ = gridWidth_ * gridHeight_;

        CreateHexagons();
    }
    if (ImGui::Button("Save"))
    {
        jsonBinder_->Save();
    }
    ImGui::End();

}

void SceneTransHex::CreateHexagons()
{
    const float xStep = hexagonSize_ * std::sqrt(3.0f) * 0.5f + spacing_ - 2.0f;
    const float yStep = hexagonSize_ * 0.75f + spacing_;

    auto rand = RandomGenerator::GetInstance();

    for (size_t y = 0; y < gridHeight_; ++y)
    {
        for (size_t x = 0; x < gridWidth_; ++x)
        {
            auto hexagon = std::make_unique<Hexagon>();
            // 六角形の位置を計算
            float posX = x * xStep;
            float posY = y * yStep;

            // 偶数行は少し右にずらす
            if (y % 2 == 1)
                posX += xStep * 0.5f;

            hexagon->position = Vector2(std::round(posX), std::round(posY));
            hexagon->size = hexagonSize_;
            hexagon->elapsedTime = 0.0f;
            hexagon->delay = rand->GetRandValue(1, gridHeight_ / 3) * duration_ / static_cast<float>(gridHeight_ / 2);
            // 六角形のスプライトを作成
            uint32_t handle = TextureManager::GetInstance()->Load("hexagon.png");
            hexagon->sprite = Sprite::Create("HexagonSprite", handle);
            hexagon->sprite->translate_ = hexagon->position;
            hexagon->sprite->SetSize(Vector2(hexagonSize_, hexagonSize_));
            hexagon->sprite->SetColor(Vector4(0, 0, 0, 0)); // 初期は透明
            hexagons_.push_back(std::move(hexagon));
        }
    }
}

void SceneTransHex::InitJsonBinder()
{
    jsonBinder_ = std::make_unique<Engine::JsonBinder>("SceneTransHex", "Resource/Data/");

    jsonBinder_->RegisterVariable("duration", &duration_);
    jsonBinder_->RegisterVariable("hexagonSize", &hexagonSize_);
    jsonBinder_->RegisterVariable("spacing", &spacing_);

    jsonBinder_->RegisterVariable("phaseCount", &phaseCount_);
}
