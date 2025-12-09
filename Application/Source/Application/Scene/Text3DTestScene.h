#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/TextRenderer/Text3DRenderer.h>
#include <Features/TextRenderer/AtlasData.h>
#include <System/Input/Input.h>

/// <summary>
/// Text3DRendererのテストシーン
/// </summary>
class Text3DTestScene : public BaseScene
{
public:
    Text3DTestScene() = default;
    ~Text3DTestScene() override = default;

    void Initialize(SceneData* sceneData) override;
    void Update() override;
    void Draw() override;
    void DrawShadow() override {}

private:
    Camera camera_;
    Input* input_ = nullptr;

    Text3DRenderer* text3DRenderer_ = nullptr;
    AtlasData* fontAtlas_ = nullptr;

    // テスト用パラメータ
    Vector3 textPosition_ = { 0, 0, 5 };
    Vector3 textRotation_ = { 0, 0, 0 };
    Vector2 textScale_ = { 1, 1 };
    Vector4 textColor_ = { 1, 1, 1, 1 };
};
