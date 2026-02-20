#include "NoteJudge.h"

// Engine
#include <Features/LineDrawer/LineDrawer.h>
#include <Debug/ImGuiDebugManager.h>

// application
#include <Application/Note/Note.h>

using namespace Engine;


NoteJudge::NoteJudge()
{
#ifdef _DEBUG
    // 繝・ヰ繝・げ繧ｦ繧｣繝ｳ繝峨え縺ｮ逋ｻ骭ｲ
    ImGuiDebugManager::GetInstance()->AddDebugWindow("JudgeLine", [this]() { ImGui::Checkbox("DrawLine", &isDrawLine); });
#endif // _DEBUG
}

NoteJudge::~NoteJudge()
{
#ifdef _DEBUG
    // 繝・ヰ繝・げ繧ｦ繧｣繝ｳ繝峨え縺ｮ蜑企勁
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("JudgeLine");
#endif // _DEBUG
}

void NoteJudge::Initialize()
{
    InitializeJsonBinder();

    const float baseFrameTime = 1.0f / 60.0f;// 60FPS蝓ｺ貅・0.0166s
    // 莉ｮ
    timingThresholds_[JudgeType::Perfect]   = baseFrameTime * 4.0f;     //  4繝輔Ξ繝ｼ繝 邏・.066s
    timingThresholds_[JudgeType::Good]      = baseFrameTime * 10.0f;    // 10繝輔Ξ繝ｼ繝 邏・.166s
    timingThresholds_[JudgeType::Bad]       = baseFrameTime * 16.0f;    // 16繝輔Ξ繝ｼ繝 邏・.266s
    timingThresholds_[JudgeType::Miss]      = baseFrameTime * 20.0f;    // 20繝輔Ξ繝ｼ繝 邏・.333s

}

void NoteJudge::DrawJudgeLine()
{
#ifdef _DEBUG
    /// debug逕ｨ
    // 蛻､螳壹Λ繧､繝ｳ縺ｨ蛻､螳夂ｯ・峇縺ｮ謠冗判

    if (!isDrawLine) return;



    float halfWidth = laneTotalWidth_ / 2.0f;

    // 蛻､螳壹Λ繧､繝ｳ繧呈緒逕ｻ
    for (const auto& [i, timingThreshold] : timingThresholds_)
    {
        // 蛻､螳壹Λ繧､繝ｳ繧呈緒逕ｻ
        float position = timingThresholds_[i] * speed_ + position_;

        Vector3 start = { -halfWidth, 0.01f,  position };
        Vector3 end = { halfWidth, 0.01f, position };

        LineDrawer::GetInstance()->RegisterPoint(start, end, Vector4(1, 1, 0, 1));
    }


    // 繝ｩ繧､繝ｳ謇句燕縺ｮ蛻､螳夂ｷ壹ｒ謠冗判
    for (const auto& [i, timingThreshold] : timingThresholds_)
    {
        // 蛻､螳壹Λ繧､繝ｳ繧呈緒逕ｻ
        float position = (-timingThresholds_[i] * speed_) + position_;

        Vector3 start = { -halfWidth, 0,  position };
        Vector3 end = { halfWidth, 0, position };

        LineDrawer::GetInstance()->RegisterPoint(start, end, Vector4(1, 1, 0, 1));
    }
#endif // DEBUG

}

JudgeType NoteJudge::ProcessNoteJudge(Note* note, float elapsedTime)
{
    if (note == nullptr)
        return JudgeType::None; // null繝√ぉ繝・け

    JudgeType result = JudgeType::None;

    float targetTime = note->GetTargetTime();

    for (const auto& [i, timingThreshold] : timingThresholds_)
    {
        // 蛻､螳夂ｯ・峇蜀・°繝√ぉ繝・け
        if (targetTime >= elapsedTime - timingThresholds_[i] &&
            targetTime <= elapsedTime + timingThresholds_[i])
        {
            // 蛻､螳壹ｒ陦後≧
            result = static_cast<JudgeType>(i);
            break;
        }
    }

    return result;
}


void NoteJudge::InitializeJsonBinder()
{
    jsonBinder_ = std::make_unique<JsonBinder>("Judge", "Resources/Data/Note/");

    std::vector<float> timingThresholds;
    jsonBinder_->GetVariableValue("TimingThresholds", timingThresholds_);


    for (size_t i = 0; i < timingThresholds.size(); ++i)
    {
        JudgeType judgeType = static_cast<JudgeType>(i);
        timingThresholds_[judgeType] = timingThresholds[i];
    }

}
