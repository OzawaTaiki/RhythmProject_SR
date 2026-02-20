#include "HexagonGrid.h"
#include <Features/UI/Collider/UIConvexPolygonCollider.h>
#include <Debug/ImGuiDebugManager.h>
#include <Features/UI/Component/UIColliderComponent.h>
#include <Features/UI/Component/UISpriteRenderComponent.h>
#include <numbers>

using namespace Engine;

namespace
{
// 謠冗判繧ｨ繝ｪ繧｢
Rect drawArea(Vector2(0, 0), Vector2(1280, 720));
}

void HexagonGrid::Initialize(const Rect& area)
{
    moveOffset_ = Vector2(0.0f, 0.0f); // 繧ｪ繝輔そ繝・ヨ繧貞・譛溷喧

    Vector2 halfSize = area.size;
    // 謠冗判繧ｨ繝ｪ繧｢繧呈僑蠑ｵ
    Vector2 expandedMin = area.GetLeftTop() - halfSize;
    Vector2 expandedMax = area.GetRightBottom() + halfSize;
    GenerateHexagonGrid(Rect(expandedMin, expandedMax));

}

void HexagonGrid::Update()
{
    ImGui();
    // 蜈ｭ隗貞ｽ｢縺ｮ蟷・→鬮倥＆繧定ｨ育ｮ・
    float width = radius_ * std::sqrtf(3.0f);
    float height = radius_ * 1.5f;

    // 繧ｰ繝ｪ繝・ラ蜈ｨ菴薙・繧ｪ繝輔そ繝・ヨ繧呈峩譁ｰ
    moveOffset_ += Vector2(-2.0f, 1.0f) * 0.016f;

    // 繝ｫ繝ｼ繝怜・逅・ 繧ｪ繝輔そ繝・ヨ縺御ｸ螳壼､繧定ｶ・∴縺溘ｉ繝ｪ繧ｻ繝・ヨ
    // X譁ｹ蜷代・繝ｫ繝ｼ繝暦ｼ・蛻怜・縺ｧ1蜻ｨ譛滂ｼ・
    if (moveOffset_.x < -width * 2.0f)
    {
        moveOffset_.x += width * 2.0f;
    }
    else if (moveOffset_.x > width * 2.0f)
    {
        moveOffset_.x -= width * 2.0f;
    }

    // Y譁ｹ蜷代・繝ｫ繝ｼ繝暦ｼ・陦悟・縺ｧ1蜻ｨ譛滂ｼ・
    if (moveOffset_.y < -height * 2.0f)
    {
        moveOffset_.y += height * 2.0f;
    }
    else if (moveOffset_.y > height * 2.0f)
    {
        moveOffset_.y -= height * 2.0f;
    }

    // 蜷・・隗貞ｽ｢縺ｮ菴咲ｽｮ繧呈峩譁ｰ・亥・譛滉ｽ咲ｽｮ + 繧ｪ繝輔そ繝・ヨ・・

    for (size_t i = 0; i < elements_.size() && i < initialPositions_.size(); ++i)
    {
        elements_[i]->SetPosition(initialPositions_[i] + moveOffset_);
        elements_[i]->Update();
    }

}

void HexagonGrid::Draw()
{
    for (const auto& element : elements_)
    {
        element->Draw();
    }
}

void HexagonGrid::GetHexagonLocalVertices(std::vector<Vector2>& outVertices) const
{
    float angleStep = std::numbers::pi_v<float> *2.0f / 6.0f;
    float drawRadius = radius_ - margin_; // margin繧貞ｼ輔＞縺溷濠蠕・〒謠冗判

    outVertices.clear();
    for (int i = 0; i < 6; ++i)
    {
        float angle = angleStep * i - std::numbers::pi_v<float> / 6.0f; // -30蠎ｦ髢句ｧ・
        float x = drawRadius * std::cosf(angle);
        float y = drawRadius * std::sinf(angle);
        outVertices.emplace_back(x, y);
    }
}

void HexagonGrid::GenerateHexagonGrid(const Rect& area)
{
    std::vector<Vector2> hexagonVertices;
    GetHexagonLocalVertices(hexagonVertices);

    // 蜈ｭ隗貞ｽ｢縺ｮ蟷・→鬮倥＆繧定ｨ育ｮ・
    // 繝槭ず繝・け繝翫Φ繝舌・ 竏・ 縺ｨ 1.5 縺ｯ豁｣蜈ｭ隗貞ｽ｢縺ｮ蟷ｾ菴募ｭｦ逧・音諤ｧ縺ｫ蝓ｺ縺･縺・
    // 繧峨＠縺・
    float width = radius_ * std::sqrtf(3.0f);
    float height = radius_ * 1.5f;
    // 逶ｴ蠕・
    float diameter = (radius_ - margin_) * 2.0f;

    Vector2 start = area.GetLeftTop();
    Vector2 end   = area.GetRightBottom();
    // margin縺ｯ驟咲ｽｮ險育ｮ励↓蜷ｫ繧√↑縺・
    int32_t colSize = static_cast<int32_t>(std::ceilf((end.x - start.x) / width)) + 2;
    int32_t rowSize = static_cast<int32_t>(std::ceilf((end.y - start.y) / height)) + 2;

    initialPositions_.clear();

    for (int32_t row = 0; row < rowSize; ++row)
    {
        for (int32_t col = 0; col < colSize; ++col)
        {
            Vector2 center = GetHexagonCenter(row, col);
            center += start; // 繧ｨ繝ｪ繧｢縺ｮ髢句ｧ倶ｽ咲ｽｮ繧貞刈邂・

            initialPositions_.push_back(center); // 蛻晄悄菴咲ｽｮ繧剃ｿ晏ｭ・

            auto collider = std::make_unique<UIConvexPolygonCollider>();
            collider->SetTransformMode(IUICollider::TransformMode::Independent);
            collider->SetLocalVertices(hexagonVertices);

            int32_t num = col + row * colSize; // 繧､繝ｳ繝・ャ繧ｯ繧ｹ險育ｮ励ｂ菫ｮ豁｣
            std::string label = "hexagon_";
            if (num < 10)
                label += "0";

            auto hexagon = std::make_unique<UIButtonElement>(label + std::to_string(num), center, Vector2( diameter, diameter ), "", false);
            hexagon->Initialize();
            hexagon->SetPosition(center);
            hexagon->SetAnchor({ 0.5f,0.5f });
            hexagon->SetSize({ diameter , diameter });
            hexagon->GetComponent<UIColliderComponent>()->SetCollider(std::move(collider));
            hexagon->SetHoverColor({ 0.15f, 0.26f, 0.3f, 0.5f });
            hexagon->SetNormalColor({ 0.125f, 0.125f, 0.19f, 0.5f });
            hexagon->GetComponent<UISpriteRenderComponent>()->LoadAndSetTexture("Hexagon.png");

            elements_.push_back(std::move(hexagon));
            // Todo: 繧､繝吶Φ繝郁ｨｭ螳・竊・
            // 繧ｫ繝ｩ繝ｼ縺ｯ螟牙喧縺ｪ縺・
            // 繝帙ヰ繝ｼ荳ｭ縺ｯ繝薙・繝医〒諡｡邵ｮ縺ｪ縺ｩ
            // 蜈ｨ菴薙→縺励※蟶ｸ縺ｫ
            // 荳螳壽婿蜷代∈豬√☆
            // 繝代・繝・ぅ繧ｯ繝ｫ繧定・繧上○繧・
            // 縺ｨ縺・

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

    // 螂・焚陦後ｒx譁ｹ蜷代↓繧ｪ繝輔そ繝・ヨ
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
            // 蜀咲函謌・
            Initialize(drawArea);
        }

        ImGui::End();
    }

#endif // _DEBUG
}
