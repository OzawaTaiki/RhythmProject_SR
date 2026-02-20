#include "FocusFrame.h"
#include <Features/UI/Element/UIImageElement.h>
#include <Features/UI/Component/UISpriteRenderComponent.h>

using namespace Engine;

void FocusFrame::Initialize()
{
    jsonBinder_ = std::make_unique<JsonBinder>("FocusFrame", "Resources/Data/UI/");

    // JSON縺九ｉ譌｢蟄倥・繧ｨ繝ｬ繝｡繝ｳ繝亥錐繧定ｪｭ縺ｿ霎ｼ縺ｿ
    //for (size_t i = 0; i < 10; ++i) // 譛螟ｧ10蛟九・繧ｨ繝ｬ繝｡繝ｳ繝医ｒ隧ｦ縺・
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
            // 繝ｪ繧｢繝ｫ繧ｿ繧､繝縺ｧ totalEdgeLength 繧貞・險育ｮ・
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

                // totalEdgeLength繧貞・險育ｮ・
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

    // 蝓ｺ貅悶→縺ｪ繧却rogress繧呈峩譁ｰ
    tracer.progress += progressIncrement;
    if (tracer.progress >= 1.0f)
        tracer.progress -= 1.0f;

    // 繝医Λ繝・き繝ｼ0縺ｨ1縺ｮ菴咲ｽｮ繧定ｨ育ｮ・
    float progress0 = tracer.progress;
    float progress1 = tracer.progress - trailLength_;
    if (progress1 < 0.0f)
        progress1 += 1.0f;

    // 繝医Λ繝・き繝ｼ0: 0縺ｨ1繧偵▽縺ｪ縺千ｷ・
    auto* spriteComponent0 = focusFrames_[0]->GetComponent<UISpriteRenderComponent>();
    auto* spriteComponent1 = focusFrames_[1]->GetComponent<UISpriteRenderComponent>();

    UpdateTrackerLine(tracer, targetPos, progress0, progress1, spriteComponent0, spriteComponent1, 5.0f);

    // 繝医Λ繝・き繝ｼ2縺ｨ3: 蜿榊ｯｾ蛛ｴ
    float progress2 = tracer.progress + 0.5f;
    if (progress2 >= 1.0f)
        progress2 -= 1.0f;
    float progress3 = progress2 - trailLength_;
    if (progress3 < 0.0f)
        progress3 += 1.0f;

    // 繝医Λ繝・き繝ｼ2: 2縺ｨ3繧偵▽縺ｪ縺千ｷ・
    auto* spriteComponent2 = focusFrames_[2]->GetComponent<UISpriteRenderComponent>();
    auto* spriteComponent3 = focusFrames_[3]->GetComponent<UISpriteRenderComponent>();

    UpdateTrackerLine(tracer, targetPos, progress2, progress3, spriteComponent2, spriteComponent3, 5.0f);

    // focusFrames縺ｮ譖ｴ譁ｰ
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
    float lengthAccum = 0.0f;// 迴ｾ蝨ｨ縺ｮ邏ｯ遨崎ｷ晞屬
    // 霎ｺ荳翫・轤ｹ
    Vector2 pointOnEdge;
    for (size_t i = 0; i < edge.vertices.size(); ++i)
    {
        size_t nextIndex = (i + 1) % edge.vertices.size();
        Vector2 start = edge.vertices[i];
        Vector2 end = edge.vertices[nextIndex];

        float segmentLength = (end - start).Length();

        if (lengthAccum + segmentLength >= distanceAlongEdge)
        {
            float remaining = distanceAlongEdge - lengthAccum;// 繧ｻ繧ｰ繝｡繝ｳ繝亥・縺ｧ縺ｮ谿九ｊ霍晞屬
            Vector2 direction = (end - start).Normalize();
            Vector2 newPos = start + direction * remaining;// 譁ｰ縺励＞鬆らせ菴咲ｽｮ
            // 鬆らせ繝・・繧ｿ縺ｮ譖ｴ譁ｰ
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
            // 縺薙・繧ｻ繧ｰ繝｡繝ｳ繝医・譁ｹ蜷代・繧ｯ繝医Ν繧定ｿ斐☆
            if (segmentLength > 0.0f)
                tangent = (end - start) / segmentLength; // 豁｣隕丞喧
            break;
        }
        lengthAccum += segmentLength;
    }

    return tangent;
}

Vector2 FocusFrame::CalcPerpendicularOnEdge(const OutlineTracer& edge, float distanceAlongEdge)
{
    // 謗･邱壹・繧ｯ繝医Ν繧貞叙蠕・
    Vector2 tangent = CalcTangentOnEdge(edge, distanceAlongEdge);

    // 謗･邱壹↓蝙ら峩縺ｪ繝吶け繝医Ν・・0蠎ｦ蝗櫁ｻ｢・・
    Vector2 perpendicular = Vector2(-tangent.y, tangent.x);

    return perpendicular;
}

VertexType FocusFrame::GetVertexTypeFromProgress(float t, int32_t index)
{
    // t繧・~4縺ｮ遽・峇縺ｫ螟画鋤・・縺､縺ｮ霎ｺ・・
    float scaledT = t * 4.0f;
    int segmentIndex = static_cast<int>(scaledT);

    // 繧ｻ繧ｰ繝｡繝ｳ繝医う繝ｳ繝・ャ繧ｯ繧ｹ繧・~3縺ｫ蛻ｶ髯・
    if (segmentIndex >= 4)
        segmentIndex = 3;
    if (segmentIndex < 0)
        segmentIndex = 0;

    // index縺悟・謨ｰ縺ｮ蝣ｴ蜷医・逶ｴ蜑阪・鬆らせ縲∝･・焚縺ｮ蝣ｴ蜷医・逶ｴ蠕後・鬆らせ
    if (index % 2 == 0)
    {
        // 逶ｴ蜑阪・鬆らせ・医そ繧ｰ繝｡繝ｳ繝医・髢句ｧ矩らせ・・
        return static_cast<VertexType>(segmentIndex);
    }
    else
    {
        // 逶ｴ蠕後・鬆らせ・医そ繧ｰ繝｡繝ｳ繝医・邨ゆｺ・らせ・・
        return static_cast<VertexType>((segmentIndex + 1) % 4);
    }
}

bool FocusFrame::IsCrossingVertex(const OutlineTracer& edge, float progress0, float progress1)
{
    // progress繧定ｷ晞屬縺ｫ螟画鋤
    float distance0 = edge.totalEdgeLength * progress0;
    float distance1 = edge.totalEdgeLength * progress1;

    // 蜷・せ縺後←縺ｮ霎ｺ荳翫↓縺ゅｋ縺九ｒ蛻､螳・
    float lengthAccum = 0.0f;
    int edgeIndex0 = -1;
    int edgeIndex1 = -1;

    for (size_t i = 0; i < edge.vertices.size(); ++i)
    {
        size_t nextIndex = (i + 1) % edge.vertices.size();
        Vector2 start = edge.vertices[i];
        Vector2 end = edge.vertices[nextIndex];

        float segmentLength = (end - start).Length();

        // distance0縺後％縺ｮ繧ｻ繧ｰ繝｡繝ｳ繝井ｸ翫↓縺ゅｋ縺・
        if (edgeIndex0 == -1 && lengthAccum + segmentLength >= distance0)
        {
            edgeIndex0 = static_cast<int>(i);
        }

        // distance1縺後％縺ｮ繧ｻ繧ｰ繝｡繝ｳ繝井ｸ翫↓縺ゅｋ縺・
        if (edgeIndex1 == -1 && lengthAccum + segmentLength >= distance1)
        {
            edgeIndex1 = static_cast<int>(i);
        }

        lengthAccum += segmentLength;

        // 荳｡譁ｹ隕九▽縺九▲縺溘ｉ邨ゆｺ・
        if (edgeIndex0 != -1 && edgeIndex1 != -1)
            break;
    }

    // 逡ｰ縺ｪ繧玖ｾｺ荳翫↓縺・ｋ蝣ｴ蜷医・鬆らせ繧偵∪縺溘＞縺ｧ縺・ｋ
    return edgeIndex0 != edgeIndex1;
}

Vector2 FocusFrame::GetCrossedVertex(const OutlineTracer& edge, float progress0, float progress1)
{
    // progress繧定ｷ晞屬縺ｫ螟画鋤
    float distance0 = edge.totalEdgeLength * progress0;
    float distance1 = edge.totalEdgeLength * progress1;

    // 蜷・せ縺後←縺ｮ霎ｺ荳翫↓縺ゅｋ縺九ｒ蛻､螳・
    float lengthAccum = 0.0f;
    int edgeIndex0 = -1;
    int edgeIndex1 = -1;

    for (size_t i = 0; i < edge.vertices.size(); ++i)
    {
        size_t nextIndex = (i + 1) % edge.vertices.size();
        Vector2 start = edge.vertices[i];
        Vector2 end = edge.vertices[nextIndex];

        float segmentLength = (end - start).Length();

        // distance0縺後％縺ｮ繧ｻ繧ｰ繝｡繝ｳ繝井ｸ翫↓縺ゅｋ縺・
        if (edgeIndex0 == -1 && lengthAccum + segmentLength >= distance0)
        {
            edgeIndex0 = static_cast<int>(i);
        }

        // distance1縺後％縺ｮ繧ｻ繧ｰ繝｡繝ｳ繝井ｸ翫↓縺ゅｋ縺・
        if (edgeIndex1 == -1 && lengthAccum + segmentLength >= distance1)
        {
            edgeIndex1 = static_cast<int>(i);
        }

        lengthAccum += segmentLength;

        // 荳｡譁ｹ隕九▽縺九▲縺溘ｉ邨ゆｺ・
        if (edgeIndex0 != -1 && edgeIndex1 != -1)
            break;
    }

    // 逡ｰ縺ｪ繧玖ｾｺ荳翫↓縺ゅｋ蝣ｴ蜷医√◎縺ｮ髢薙・鬆らせ繧定ｿ斐☆
    if (edgeIndex0 != edgeIndex1)
    {
        return edge.vertices[edgeIndex0];
    }

    // 蜷後§霎ｺ荳翫↓縺ゅｋ蝣ｴ蜷医・遨ｺ縺ｮ繝吶け繝医Ν繧定ｿ斐☆・医お繝ｩ繝ｼ繧ｱ繝ｼ繧ｹ・・
    return Vector2(0, 0);
}

void FocusFrame::UpdateTrackerLine(const OutlineTracer& tracer, const Vector2& targetPos, float progress0, float progress1, UISpriteRenderComponent* spriteComponent0, UISpriteRenderComponent* spriteComponent1, float width)
{
    if (!spriteComponent0 || !spriteComponent1)
        return;

    // 鬆らせ繧偵∪縺溘＞縺ｧ縺・ｋ縺九メ繧ｧ繝・け
    bool isCrossing = IsCrossingVertex(tracer, progress0, progress1);

    // 蜷аrogress縺ｮ霍晞屬縺ｨ菴咲ｽｮ繧定ｨ育ｮ・
    float distanceAlongEdge0 = tracer.totalEdgeLength * progress0;
    float distanceAlongEdge1 = tracer.totalEdgeLength * progress1;
    Vector2 pos0 = CalcPointOnEdge(tracer, distanceAlongEdge0);
    Vector2 pos1 = CalcPointOnEdge(tracer, distanceAlongEdge1);

    Vector2 worldPos0 = targetPos + pos0;
    Vector2 worldPos1 = targetPos + pos1;

    // 繧ｹ繝励Λ繧､繝医・鬆らせ繝・・繧ｿ繧貞叙蠕・
    auto* sprite = spriteComponent0->GetSprite();
    auto& vertices1 = sprite->GetVertexData();

    auto* sprite2 = spriteComponent1->GetSprite();
    auto& vertices2 = sprite2->GetVertexData();

    // 蝙ら峩繝吶け繝医Ν繧貞叙蠕・
    Vector2 perpendicular0 = CalcPerpendicularOnEdge(tracer, distanceAlongEdge0);
    Vector2 perpendicular1 = CalcPerpendicularOnEdge(tracer, distanceAlongEdge1);

    if (isCrossing)
    {
        // 鬆らせ繧偵∪縺溘＞縺ｧ縺・ｋ蝣ｴ蜷医・らせ縺ｧ譖ｲ縺後ｋ邱壹ｒ謠冗判
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
        // 蜷後§霎ｺ荳翫↓縺ゅｋ蝣ｴ蜷医・逶ｴ邱壹〒謠冗判
        vertices1[0].position = worldPos0 + perpendicular0 * width;
        vertices1[1].position = worldPos0 - perpendicular0 * width;
        vertices1[2].position = worldPos1 + perpendicular0 * width;
        vertices1[3].position = vertices1[1].position;
        vertices1[4].position = worldPos1 - perpendicular0 * width;
        vertices1[5].position = vertices1[2].position;

        // spriteComponent1縺ｯ菴ｿ繧上↑縺・・縺ｧ遨ｺ縺ｫ縺吶ｋ
        vertices2[0].position = Vector2::zero;
        vertices2[1].position = Vector2::zero;
        vertices2[2].position = Vector2::zero;
        vertices2[3].position = Vector2::zero;
        vertices2[4].position = Vector2::zero;
        vertices2[5].position = Vector2::zero;
    }
}
