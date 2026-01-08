#include "FocusFrame.h"
#include <Features/UI/Element/UIImageElement.h>
#include <Features/UI/Component/UISpriteRenderComponent.h>

void FocusFrame::Initialize()
{
    jsonBinder_ = std::make_unique<JsonBinder>("FocusFrame", "Resources/Data/UI/");

    // JSONから既存のエレメント名を読み込み
    //for (size_t i = 0; i < 10; ++i) // 最大10個のエレメントを試す
    //{
    //    std::string str;
    //    jsonBinder_->GetVariableValue("ElementName_" + std::to_string(i), str);

    //    edgeNameToIndexMap_[str] = i;

    //    OutlineTracer edge;
    //    InitEdgesParams(i, edge);
    //    tracers_.push_back(edge);
    //}
    jsonBinder_->RegisterVariable("CycleTime", &cycleTime_);

    for (size_t i=0; i < focusFrames_.size(); ++i)
    {
        focusFrames_[i] = std::make_unique<UIImageElement>(
            "FocusFrame_Edge" + std::to_string(i),
            Vector2(0, 0),
            Vector2(100, 100)
        );
        focusFrames_[i]->Initialize();
        focusFrames_[i]->SetSize({ 1,1 });
        focusFrames_[i]->SetOrder(5000);
    }

}

void FocusFrame::Update(float deltaTime)
{
#ifdef _DEBUG
    ImGui::Begin("FocusFrame Debug");

    ImGui::DragFloat("CycleTime", &cycleTime_, 0.1f, 0.1f, 10.0f);
    ImGui::DragFloat("TrailLength", &trailLength_, 0.01f, 0.0f, 1.0f);
    ImGui::Text("Current time: %.2f", currentTime_);
    ImGui::Text("Target Element: %s", targetElement_ ? targetElement_->GetName().c_str() : "None");
    if (targetElement_)
    {
        Vector2 targetPos = targetElement_->GetWorldPosition();
        ImGui::Text("Target Position: (%.1f, %.1f)", targetPos.x, targetPos.y);
        Vector2 targetSize = targetElement_->GetSize();
        ImGui::Text("Target Size: (%.1f, %.1f)", targetSize.x, targetSize.y);

        size_t index = edgeNameToIndexMap_[targetElement_->GetName()];
        auto& tracer = tracers_[index];

        float progress0 = tracer.progress;
        float progress1 = tracer.progress - trailLength_;
        if (progress1 < 0.0f) progress1 += 1.0f;

        float progress2 = tracer.progress + 0.5f;
        if (progress2 >= 1.0f) progress2 -= 1.0f;
        float progress3 = progress2 - trailLength_;
        if (progress3 < 0.0f) progress3 += 1.0f;

        bool isCrossing01 = IsCrossingVertex(tracer, progress0, progress1);
        bool isCrossing23 = IsCrossingVertex(tracer, progress2, progress3);

        ImGui::Text("Tracker 0-1 Crossing: %s", isCrossing01 ? "Yes" : "No");
        ImGui::Text("Tracker 2-3 Crossing: %s", isCrossing23 ? "Yes" : "No");
    }
    ImGui::Separator();
    static size_t selectedEdgeIndex = 0;
    {
        std::vector<const char*> edgeNames(edgeNameToIndexMap_.size());
        for (const auto& [name, id] : edgeNameToIndexMap_)
        {
            edgeNames[id] = name.c_str();
        }
        ImGui::Combo("Select Edge", reinterpret_cast<int*>(&selectedEdgeIndex), edgeNames.data(), static_cast<int>(edgeNames.size()));
    }

    if (selectedEdgeIndex < tracers_.size())
    {
        auto& edge = tracers_[selectedEdgeIndex];
        ImGui::Text("Total Edge Length: %.2f", edge.totalEdgeLength);
        ImGui::Text("Progress: %.2f", edge.progress);
        ImGui::Separator();

        if (ImGui::DragFloat2("LeftTop", &edge.vertices[static_cast<size_t>(VertexType::LeftTop)].x, 1.0f))
        {
            // リアルタイムで totalEdgeLength を再計算
            float totalLength = 0.0f;
            Vector2 lt = edge.vertices[static_cast<size_t>(VertexType::LeftTop)];
            Vector2 rt = edge.vertices[static_cast<size_t>(VertexType::RightTop)];
            Vector2 rb = edge.vertices[static_cast<size_t>(VertexType::RightBottom)];
            Vector2 lb = edge.vertices[static_cast<size_t>(VertexType::LeftBottom)];
            totalLength += (rt - lt).Length();
            totalLength += (rb - rt).Length();
            totalLength += (lb - rb).Length();
            totalLength += (lt - lb).Length();
            edge.totalEdgeLength = totalLength;
        }
        if (ImGui::DragFloat2("RightTop", &edge.vertices[static_cast<size_t>(VertexType::RightTop)].x, 1.0f))
        {
            float totalLength = 0.0f;
            Vector2 lt = edge.vertices[static_cast<size_t>(VertexType::LeftTop)];
            Vector2 rt = edge.vertices[static_cast<size_t>(VertexType::RightTop)];
            Vector2 rb = edge.vertices[static_cast<size_t>(VertexType::RightBottom)];
            Vector2 lb = edge.vertices[static_cast<size_t>(VertexType::LeftBottom)];
            totalLength += (rt - lt).Length();
            totalLength += (rb - rt).Length();
            totalLength += (lb - rb).Length();
            totalLength += (lt - lb).Length();
            edge.totalEdgeLength = totalLength;
        }
        if (ImGui::DragFloat2("RightBottom", &edge.vertices[static_cast<size_t>(VertexType::RightBottom)].x, 1.0f))
        {
            float totalLength = 0.0f;
            Vector2 lt = edge.vertices[static_cast<size_t>(VertexType::LeftTop)];
            Vector2 rt = edge.vertices[static_cast<size_t>(VertexType::RightTop)];
            Vector2 rb = edge.vertices[static_cast<size_t>(VertexType::RightBottom)];
            Vector2 lb = edge.vertices[static_cast<size_t>(VertexType::LeftBottom)];
            totalLength += (rt - lt).Length();
            totalLength += (rb - rt).Length();
            totalLength += (lb - rb).Length();
            totalLength += (lt - lb).Length();
            edge.totalEdgeLength = totalLength;
        }
        if (ImGui::DragFloat2("LeftBottom", &edge.vertices[static_cast<size_t>(VertexType::LeftBottom)].x, 1.0f))
        {
            float totalLength = 0.0f;
            Vector2 lt = edge.vertices[static_cast<size_t>(VertexType::LeftTop)];
            Vector2 rt = edge.vertices[static_cast<size_t>(VertexType::RightTop)];
            Vector2 rb = edge.vertices[static_cast<size_t>(VertexType::RightBottom)];
            Vector2 lb = edge.vertices[static_cast<size_t>(VertexType::LeftBottom)];
            totalLength += (rt - lt).Length();
            totalLength += (rb - rt).Length();
            totalLength += (lb - rb).Length();
            totalLength += (lt - lb).Length();
            edge.totalEdgeLength = totalLength;
        }

        ImGui::Separator();
        if (ImGui::Button("Copy from Target Element"))
        {
            if (targetElement_)
            {
                Vector2 size = targetElement_->GetSize();
                edge.vertices[static_cast<size_t>(VertexType::LeftTop)] = Vector2(-size.x / 2, size.y / 2);
                edge.vertices[static_cast<size_t>(VertexType::RightTop)] = Vector2(size.x / 2, size.y / 2);
                edge.vertices[static_cast<size_t>(VertexType::RightBottom)] = Vector2(size.x / 2, -size.y / 2);
                edge.vertices[static_cast<size_t>(VertexType::LeftBottom)] = Vector2(-size.x / 2, -size.y / 2);

                // totalEdgeLengthを再計算
                float totalLength = 0.0f;
                Vector2 lt = edge.vertices[static_cast<size_t>(VertexType::LeftTop)];
                Vector2 rt = edge.vertices[static_cast<size_t>(VertexType::RightTop)];
                Vector2 rb = edge.vertices[static_cast<size_t>(VertexType::RightBottom)];
                Vector2 lb = edge.vertices[static_cast<size_t>(VertexType::LeftBottom)];
                totalLength += (rt - lt).Length();
                totalLength += (rb - rt).Length();
                totalLength += (lb - rb).Length();
                totalLength += (lt - lb).Length();
                edge.totalEdgeLength = totalLength;
            }
        }
    }

    if (ImGui::Button("Save"))
    {
        for (auto& [name, id] : edgeNameToIndexMap_)
            jsonBinder_->SendVariable("ElementName_" + std::to_string(id), name);

        for (OutlineTracer& tracer : tracers_)
        {
            size_t id = &tracer - &tracers_[0];
            std::string tag = "FocusFrame_Edge" + std::to_string(id) + "_";

            jsonBinder_->SendVariable(tag + "LeftTop", tracer.vertices[static_cast<size_t>(VertexType::LeftTop)]);
            jsonBinder_->SendVariable(tag + "RightTop", tracer.vertices[static_cast<size_t>(VertexType::RightTop)]);
            jsonBinder_->SendVariable(tag + "RightBottom", tracer.vertices[static_cast<size_t>(VertexType::RightBottom)]);
            jsonBinder_->SendVariable(tag + "LeftBottom", tracer.vertices[static_cast<size_t>(VertexType::LeftBottom)]);
        }

        jsonBinder_->Save();
    }
    ImGui::End();
#endif // _DEBUG


    if (!targetElement_)
        return;

    currentTime_ = std::fmod(currentTime_ + deltaTime, cycleTime_);
    float progressIncrement = deltaTime / cycleTime_;

    size_t index = edgeNameToIndexMap_[targetElement_->GetName()];
    Vector2 targetPos = targetElement_->GetWorldPosition();
    auto& tracer = tracers_[index];

    // 基準となるprogressを更新
    tracer.progress += progressIncrement;
    if (tracer.progress >= 1.0f)
        tracer.progress -= 1.0f;

    // トラッカー0と1の位置を計算
    float progress0 = tracer.progress;
    float progress1 = tracer.progress - trailLength_;
    if (progress1 < 0.0f)
        progress1 += 1.0f;

    // トラッカー0: 0と1をつなぐ線
    auto* spriteComponent0 = focusFrames_[0]->GetComponent<UISpriteRenderComponent>();
    auto* spriteComponent1 = focusFrames_[1]->GetComponent<UISpriteRenderComponent>();

    UpdateTrackerLine(tracer, targetPos, progress0, progress1, spriteComponent0, spriteComponent1, 5.0f);

    // トラッカー2と3: 反対側
    float progress2 = tracer.progress + 0.5f;
    if (progress2 >= 1.0f)
        progress2 -= 1.0f;
    float progress3 = progress2 - trailLength_;
    if (progress3 < 0.0f)
        progress3 += 1.0f;

    // トラッカー2: 2と3をつなぐ線
    auto* spriteComponent2 = focusFrames_[2]->GetComponent<UISpriteRenderComponent>();
    auto* spriteComponent3 = focusFrames_[3]->GetComponent<UISpriteRenderComponent>();

    UpdateTrackerLine(tracer, targetPos, progress2, progress3, spriteComponent2, spriteComponent3, 5.0f);

    // focusFramesの更新
    for (auto& frame : focusFrames_)
    {
        frame->Update();
    }
}

void FocusFrame::Draw()
{
    if (!targetElement_)
        return;

    for (auto& frame : focusFrames_)
    {
        frame->Draw();
    }
}

void FocusFrame::ChangeTarget(UIElement* target)
{
    if (!target)
    {
        targetElement_ = target;
        return;
    }
    targetElement_ = target;
    if (!edgeNameToIndexMap_.contains(targetElement_->GetName()))
    {
        edgeNameToIndexMap_[targetElement_->GetName()] = edgeNameToIndexMap_.size();
    }
    ClearProgress();
}

void FocusFrame::RegisterTargetName(const UIElement* target)
{
    if (!edgeNameToIndexMap_.contains(target->GetName()))
    {
        size_t id=edgeNameToIndexMap_.size();
        edgeNameToIndexMap_[target->GetName()] =id;
        OutlineTracer edge;
        InitEdgesParams(static_cast<size_t>(id), edge);
        tracers_.push_back(edge);
    }
}

void FocusFrame::ClearProgress()
{
    for (auto& edge : tracers_)
    {
        edge.progress = 0.0f;
    }
}

void FocusFrame::InitEdgesParams(size_t number, OutlineTracer& edge)
{
    std::string tag = "FocusFrame_Edge" + std::to_string(number) + "_";
    if (edge.vertices.size() != 4)
    {
        edge.vertices.resize(4);
    }
    jsonBinder_->GetVariableValue(tag + "LeftTop", edge.vertices[static_cast<size_t>(VertexType::LeftTop)]);
    jsonBinder_->GetVariableValue(tag + "RightTop", edge.vertices[static_cast<size_t>(VertexType::RightTop)]);
    jsonBinder_->GetVariableValue(tag + "RightBottom", edge.vertices[static_cast<size_t>(VertexType::RightBottom)]);
    jsonBinder_->GetVariableValue(tag + "LeftBottom", edge.vertices[static_cast<size_t>(VertexType::LeftBottom)]);

    float totalLength = 0.0f;
    Vector2 lt = edge.vertices[static_cast<size_t>(VertexType::LeftTop)];
    Vector2 rt = edge.vertices[static_cast<size_t>(VertexType::RightTop)];
    Vector2 rb = edge.vertices[static_cast<size_t>(VertexType::RightBottom)];
    Vector2 lb = edge.vertices[static_cast<size_t>(VertexType::LeftBottom)];

    totalLength += (rt - lt).Length();
    totalLength += (rb - rt).Length();
    totalLength += (lb - rb).Length();
    totalLength += (lt - lb).Length();
    edge.totalEdgeLength = totalLength;
}

Vector2 FocusFrame::CalcPointOnEdge(const OutlineTracer& edge, float distanceAlongEdge)
{
    float lengthAccum = 0.0f;// 現在の累積距離
    // 辺上の点
    Vector2 pointOnEdge;
    for (size_t i = 0; i < edge.vertices.size(); ++i)
    {
        size_t nextIndex = (i + 1) % edge.vertices.size();
        Vector2 start = edge.vertices[i];
        Vector2 end = edge.vertices[nextIndex];

        float segmentLength = (end - start).Length();

        if (lengthAccum + segmentLength >= distanceAlongEdge)
        {
            float remaining = distanceAlongEdge - lengthAccum;// セグメント内での残り距離
            Vector2 direction = (end - start).Normalize();
            Vector2 newPos = start + direction * remaining;// 新しい頂点位置
            // 頂点データの更新
            pointOnEdge = newPos;
            break;
        }
        lengthAccum += segmentLength;
    }

    return pointOnEdge;
}

Vector2 FocusFrame::CalcTangentOnEdge(const OutlineTracer& edge, float distanceAlongEdge)
{
    float lengthAccum = 0.0f;
    Vector2 tangent;
    for (size_t i = 0; i < edge.vertices.size(); ++i)
    {
        size_t nextIndex = (i + 1) % edge.vertices.size();
        Vector2 start = edge.vertices[i];
        Vector2 end = edge.vertices[nextIndex];

        float segmentLength = (end - start).Length();

        if (lengthAccum + segmentLength >= distanceAlongEdge)
        {
            // このセグメントの方向ベクトルを返す
            if (segmentLength > 0.0f)
                tangent = (end - start) / segmentLength; // 正規化
            break;
        }
        lengthAccum += segmentLength;
    }

    return tangent;
}

Vector2 FocusFrame::CalcPerpendicularOnEdge(const OutlineTracer& edge, float distanceAlongEdge)
{
    // 接線ベクトルを取得
    Vector2 tangent = CalcTangentOnEdge(edge, distanceAlongEdge);

    // 接線に垂直なベクトル（90度回転）
    Vector2 perpendicular = Vector2(-tangent.y, tangent.x);

    return perpendicular;
}

VertexType FocusFrame::GetVertexTypeFromProgress(float t, int32_t index)
{
    // tを0~4の範囲に変換（4つの辺）
    float scaledT = t * 4.0f;
    int segmentIndex = static_cast<int>(scaledT);

    // セグメントインデックスを0~3に制限
    if (segmentIndex >= 4)
        segmentIndex = 3;
    if (segmentIndex < 0)
        segmentIndex = 0;

    // indexが偶数の場合は直前の頂点、奇数の場合は直後の頂点
    if (index % 2 == 0)
    {
        // 直前の頂点（セグメントの開始頂点）
        return static_cast<VertexType>(segmentIndex);
    }
    else
    {
        // 直後の頂点（セグメントの終了頂点）
        return static_cast<VertexType>((segmentIndex + 1) % 4);
    }
}

bool FocusFrame::IsCrossingVertex(const OutlineTracer& edge, float progress0, float progress1)
{
    // progressを距離に変換
    float distance0 = edge.totalEdgeLength * progress0;
    float distance1 = edge.totalEdgeLength * progress1;

    // 各点がどの辺上にあるかを判定
    float lengthAccum = 0.0f;
    int edgeIndex0 = -1;
    int edgeIndex1 = -1;

    for (size_t i = 0; i < edge.vertices.size(); ++i)
    {
        size_t nextIndex = (i + 1) % edge.vertices.size();
        Vector2 start = edge.vertices[i];
        Vector2 end = edge.vertices[nextIndex];

        float segmentLength = (end - start).Length();

        // distance0がこのセグメント上にあるか
        if (edgeIndex0 == -1 && lengthAccum + segmentLength >= distance0)
        {
            edgeIndex0 = static_cast<int>(i);
        }

        // distance1がこのセグメント上にあるか
        if (edgeIndex1 == -1 && lengthAccum + segmentLength >= distance1)
        {
            edgeIndex1 = static_cast<int>(i);
        }

        lengthAccum += segmentLength;

        // 両方見つかったら終了
        if (edgeIndex0 != -1 && edgeIndex1 != -1)
            break;
    }

    // 異なる辺上にいる場合は頂点をまたいでいる
    return edgeIndex0 != edgeIndex1;
}

Vector2 FocusFrame::GetCrossedVertex(const OutlineTracer& edge, float progress0, float progress1)
{
    // progressを距離に変換
    float distance0 = edge.totalEdgeLength * progress0;
    float distance1 = edge.totalEdgeLength * progress1;

    // 各点がどの辺上にあるかを判定
    float lengthAccum = 0.0f;
    int edgeIndex0 = -1;
    int edgeIndex1 = -1;

    for (size_t i = 0; i < edge.vertices.size(); ++i)
    {
        size_t nextIndex = (i + 1) % edge.vertices.size();
        Vector2 start = edge.vertices[i];
        Vector2 end = edge.vertices[nextIndex];

        float segmentLength = (end - start).Length();

        // distance0がこのセグメント上にあるか
        if (edgeIndex0 == -1 && lengthAccum + segmentLength >= distance0)
        {
            edgeIndex0 = static_cast<int>(i);
        }

        // distance1がこのセグメント上にあるか
        if (edgeIndex1 == -1 && lengthAccum + segmentLength >= distance1)
        {
            edgeIndex1 = static_cast<int>(i);
        }

        lengthAccum += segmentLength;

        // 両方見つかったら終了
        if (edgeIndex0 != -1 && edgeIndex1 != -1)
            break;
    }

    // 異なる辺上にある場合、その間の頂点を返す
    if (edgeIndex0 != edgeIndex1)
    {
        return edge.vertices[edgeIndex0];
    }

    // 同じ辺上にある場合は空のベクトルを返す（エラーケース）
    return Vector2(0, 0);
}

void FocusFrame::UpdateTrackerLine(const OutlineTracer& tracer, const Vector2& targetPos, float progress0, float progress1, UISpriteRenderComponent* spriteComponent0, UISpriteRenderComponent* spriteComponent1, float width)
{
    if (!spriteComponent0 || !spriteComponent1)
        return;

    // 頂点をまたいでいるかチェック
    bool isCrossing = IsCrossingVertex(tracer, progress0, progress1);

    // 各progressの距離と位置を計算
    float distanceAlongEdge0 = tracer.totalEdgeLength * progress0;
    float distanceAlongEdge1 = tracer.totalEdgeLength * progress1;
    Vector2 pos0 = CalcPointOnEdge(tracer, distanceAlongEdge0);
    Vector2 pos1 = CalcPointOnEdge(tracer, distanceAlongEdge1);

    Vector2 worldPos0 = targetPos + pos0;
    Vector2 worldPos1 = targetPos + pos1;

    // スプライトの頂点データを取得
    auto* sprite = spriteComponent0->GetSprite();
    auto& vertices1 = sprite->GetVertexData();

    auto* sprite2 = spriteComponent1->GetSprite();
    auto& vertices2 = sprite2->GetVertexData();

    // 垂直ベクトルを取得
    Vector2 perpendicular0 = CalcPerpendicularOnEdge(tracer, distanceAlongEdge0);
    Vector2 perpendicular1 = CalcPerpendicularOnEdge(tracer, distanceAlongEdge1);

    if (isCrossing)
    {
        // 頂点をまたいでいる場合、頂点で曲がる線を描画
        Vector2 crossedVertex = GetCrossedVertex(tracer, progress0, progress1) + targetPos;

        vertices1[0].position = worldPos0 + perpendicular0 * width;
        vertices1[1].position = worldPos0 - perpendicular0 * width;
        vertices1[2].position = crossedVertex + perpendicular0 * width;
        vertices1[3].position = vertices1[1].position;
        vertices1[4].position = crossedVertex - perpendicular0 * width;
        vertices1[5].position = vertices1[2].position;

        vertices2[0].position = crossedVertex + perpendicular1 * width;
        vertices2[1].position = crossedVertex - perpendicular1 * width;
        vertices2[2].position = worldPos1 + perpendicular1 * width;
        vertices2[3].position = vertices2[1].position;
        vertices2[4].position = worldPos1 - perpendicular1 * width;
        vertices2[5].position = vertices2[2].position;

    }
    else
    {
        // 同じ辺上にある場合は直線で描画
        vertices1[0].position = worldPos0 + perpendicular0 * width;
        vertices1[1].position = worldPos0 - perpendicular0 * width;
        vertices1[2].position = worldPos1 + perpendicular0 * width;
        vertices1[3].position = vertices1[1].position;
        vertices1[4].position = worldPos1 - perpendicular0 * width;
        vertices1[5].position = vertices1[2].position;

        // spriteComponent1は使わないので空にする
        vertices2[0].position = Vector2::zero;
        vertices2[1].position = Vector2::zero;
        vertices2[2].position = Vector2::zero;
        vertices2[3].position = Vector2::zero;
        vertices2[4].position = Vector2::zero;
        vertices2[5].position = Vector2::zero;
    }
}
