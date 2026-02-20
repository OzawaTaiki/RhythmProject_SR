#pragma once

#include <Features/Scene/Interface/BaseScene.h>

class LoadScene : public Engine::BaseScene
{
public:
    LoadScene() = default;
    ~LoadScene() override = default;

    void Initialize(Engine::SceneData* sceneData) override;
    void Update() override;
    void Draw() override;

    void DrawShadow() override {};

private:
    float loadTime_ = 0.0f;
    const float kLoadDuration_ = 2.0f; // ロードシーンの表示時間（秒）
};