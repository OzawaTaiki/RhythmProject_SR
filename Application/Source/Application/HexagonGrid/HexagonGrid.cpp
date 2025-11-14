#include "HexagonGrid.h"
#include <Features/UI/Collider/UIConvexPolygonCollider.h>
#include <Debug/ImGuiDebugManager.h>

namespace
{
// 描画エリア
Rect drawArea(Vector2(0, 0), Vector2(1280, 720));
}

void HexagonGrid::Initialize(const Rect& area)
{
    uiGroup_ = std::make_unique<UIGroup>();
    drawArea = area;
    GenerateHexagonGrid(drawArea);
}

void HexagonGrid::Update()
{
    ImGui();
    if (uiGroup_)
    {
        uiGroup_->Update();
    }
}

void HexagonGrid::Draw()
{
    if (uiGroup_)
    {
        uiGroup_->Draw();
    }
}

void HexagonGrid::GetHexagonLocalVertices(std::vector<Vector2>& outVertices) const
{
    float angleStep = std::numbers::pi_v<float> *2.0f / 6.0f;
    float drawRadius = radius_ - margin_; // marginを引いた半径で描画

    outVertices.clear();
    for (int i = 0; i < 6; ++i)
    {
        float angle = angleStep * i - std::numbers::pi_v<float> / 6.0f; // -30度開始
        float x = drawRadius * std::cosf(angle);
        float y = drawRadius * std::sinf(angle);
        outVertices.emplace_back(x, y);
    }
}

void HexagonGrid::GenerateHexagonGrid(const Rect& area)
{
    std::vector<Vector2> hexagonVertices;
    GetHexagonLocalVertices(hexagonVertices);

    // 六角形の幅と高さを計算
    // マジックナンバー √3 と 1.5 は正六角形の幾何学的特性に基づく
    // らしい
    float width = radius_ * std::sqrtf(3.0f);
    float height = radius_ * 1.5f;

    Vector2 start = area.GetLeftTop();
    Vector2 end   = area.GetRightBottom();

    // marginは配置計算に含めない
    int32_t colSize = static_cast<int32_t>(std::ceilf((end.x - start.x) / width)) + 2;
    int32_t rowSize = static_cast<int32_t>(std::ceilf((end.y - start.y) / height)) + 2;

    for (int32_t row = 0; row < rowSize; ++row)
    {
        for (int32_t col = 0; col < colSize; ++col)
        {
            Vector2 center = GetHexagonCenter(row, col);
            center += start; // エリアの開始位置を加算

            auto collider = std::make_unique<UIConvexPolygonCollider>();
            collider->SetTransformMode(IUICollider::TransformMode::Independent);
            collider->SetLocalVertices(hexagonVertices);

            int32_t num = col + row * colSize; // インデックス計算も修正
            std::string label = "hexagon_";
            if (num < 10)
                label += "0";

            auto hexagon = uiGroup_->CreateButton(label + std::to_string(num));
            hexagon->SetPos(center);
            hexagon->SetSize({ radius_ , radius_ });
            hexagon->SetCollider(std::move(collider));
            hexagon->SetHoverColor({ 0.8f, 0.8f, 1.0f, 1.0f });
            hexagon->SetDefaultColor({ 0.6f, 0.6f, 0.9f, 1.0f });
            // Todo: イベント設定 ↓
            // カラーは変化なし
            // ホバー中はビートで拡縮など
            // 全体として常に
            // 一定方向へ流す
            // パーティクルを舞わせる
            // とか
        }
    }
}

Vector2 HexagonGrid::GetHexagonCenter(int32_t row, int32_t col) const
{
    float width  = radius_ * std::sqrtf(3.0f);
    float height = radius_ * 1.5f;

    Vector2 center;
    center.x = col * width;
    center.y = row * height;

    // 奇数行をx方向にオフセット
    if (row % 2 == 1)
    {
        center.x += width * 0.5f;
    }

    return center;
}

void HexagonGrid::ImGui()
{
#ifdef _DEBUG

    if (ImGuiDebugManager::GetInstance()->Begin("HexagonGrid Settings"))
    {
        ImGui::DragFloat("Radius", &radius_, 1.0f, 10.0f, 500.0f);
        ImGui::DragFloat("Margin", &margin_, 0.5f, 0.0f, radius_ - 1.0f);
        if (ImGui::Button("Regenerate Grid"))
        {
            // 再生成
            Initialize(drawArea);
        }

        ImGui::End();
    }

#endif // _DEBUG
}
