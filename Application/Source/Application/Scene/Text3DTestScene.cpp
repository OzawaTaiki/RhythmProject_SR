#include "Text3DTestScene.h"

#include <Debug/ImGuiDebugManager.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <Features/TextRenderer/FontCache.h>

void Text3DTestScene::Initialize([[maybe_unused]]SceneData* sceneData)
{
    // カメラの初期化
    camera_.Initialize(CameraType::Perspective);
    camera_.translate_ = { 0, 0, -10 };
    camera_.rotate_ = { 0, 0, 0 };
    camera_.UpdateMatrix();

    // Text3DRendererの取得
    text3DRenderer_ = Text3DRenderer::GetInstance();

    // フォントアトラスの取得（既存のフォントを使用）
    fontAtlas_ = FontCache::GetInstance()->GetAtlasData("Resources/Fonts/NotoSansJP-Regular.ttf", 64);

    input_ = Input::GetInstance();
}

void Text3DTestScene::Update()
{
    // カメラの更新
    camera_.Update();

#ifdef _DEBUG
    // ImGuiでパラメータ調整
    if (ImGuiDebugManager::GetInstance()->Begin("Text3D Test"))
    {
        ImGui::Text("Camera Controls:");
        ImGui::DragFloat3("Camera Position", &camera_.translate_.x, 0.1f);
        ImGui::DragFloat3("Camera Rotation", &camera_.rotate_.x, 0.01f);

        ImGui::Separator();
        ImGui::Text("Text Controls:");
        ImGui::DragFloat3("Text Position", &textPosition_.x, 0.1f);
        ImGui::DragFloat3("Text Rotation", &textRotation_.x, 0.01f);
        ImGui::DragFloat2("Text Scale", &textScale_.x, 0.1f, 0.1f, 10.0f);
        ImGui::ColorEdit4("Text Color", &textColor_.x);

        if (ImGui::Button("Reset Camera"))
        {
            camera_.translate_ = { 0, 0, -10 };
            camera_.rotate_ = { 0, 0, 0 };
        }

        if (ImGui::Button("Reset Text"))
        {
            textPosition_ = { 0, 0, 5 };
            textRotation_ = { 0, 0, 0 };
            textScale_ = { 1, 1 };
            textColor_ = { 1, 1, 1, 1 };
        }

        ImGui::End();
    }
#endif

    camera_.UpdateMatrix();
}

void Text3DTestScene::Draw()
{
    if (!fontAtlas_)
        return;

    // Text3DRendererの描画開始
    text3DRenderer_->BeginFrame();

    // 3Dテキストの描画テスト
    text3DRenderer_->DrawText3D(
        L"Hello 3D World!",
        fontAtlas_,
        &camera_,
        textPosition_,
        textRotation_,
        textScale_,
        textColor_
    );

    // 複数のテキストを配置
    text3DRenderer_->DrawText3D(
        L"Text at (5, 2, 0)",
        fontAtlas_,
        &camera_,
        Vector3{ 5, 2, 0 },
        Vector3::zero,
        Vector2{ 0.5f, 0.5f },
        Vector4{ 1, 0, 0, 1 }  // 赤
    );

    text3DRenderer_->DrawText3D(
        L"Text at (-5, 2, 0)",
        fontAtlas_,
        &camera_,
        Vector3{ -5, 2, 0 },
        Vector3::zero,
        Vector2{ 0.5f, 0.5f },
        Vector4{ 0, 1, 0, 1 }  // 緑
    );

    text3DRenderer_->DrawText3D(
        L"Text at (0, -2, 0)",
        fontAtlas_,
        &camera_,
        Vector3{ 0, -2, 0 },
        Vector3::zero,
        Vector2{ 0.5f, 0.5f },
        Vector4{ 0, 0, 1, 1 }  // 青
    );

    // グラデーションテキストのテスト
    text3DRenderer_->DrawText3D(
        L"Gradient Text",
        fontAtlas_,
        &camera_,
        Vector3{ 0, 4, 0 },
        Vector3::zero,
        Vector2{ 1, 1 },
        Vector4{ 1, 1, 0, 1 },  // 上：黄色
        Vector4{ 1, 0, 1, 1 }   // 下：マゼンタ
    );

    // 回転したテキスト
    text3DRenderer_->DrawText3D(
        L"Rotated",
        fontAtlas_,
        &camera_,
        Vector3{ 0, -4, 0 },
        Vector3{ 0, 0, 0.785f },  // 45度回転
        Vector2{ 0.8f, 0.8f },
        Vector4{ 0, 1, 1, 1 }  // シアン
    );

    // Text3DRendererの描画終了
    text3DRenderer_->EndFrame();

#ifdef _DEBUG
    // 座標軸を表示（デバッグ用）
    //auto lineDrawer = LineDrawer::GetInstance();
    //lineDrawer->DrawLine3D({ 0, 0, 0 }, { 5, 0, 0 }, { 1, 0, 0, 1 });  // X軸（赤）
    //lineDrawer->DrawLine3D({ 0, 0, 0 }, { 0, 5, 0 }, { 0, 1, 0, 1 });  // Y軸（緑）
    //lineDrawer->DrawLine3D({ 0, 0, 0 }, { 0, 0, 5 }, { 0, 0, 1, 1 });  // Z軸（青）
#endif
}
