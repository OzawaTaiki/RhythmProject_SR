#include "JudgeResult.h"

#include <Features/Event/EventManager.h>
#include <Debug/ImGuiDebugManager.h>

using namespace Engine;


JudgeResult::JudgeResult()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->AddDebugWindow("JudgeResult", [&]() { DebugWindow(); });
#endif // _DEBUG
}

JudgeResult::~JudgeResult()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("JudgeResult");
#endif // _DEBUG
}

void JudgeResult::Initialize()
{
    judgeResult_.clear();

    // None縺ｯ髯､螟・1縺九ｉ
    for (size_t i = 1; i < static_cast<size_t>(JudgeType::MAX); ++i)
    {
        JudgeType type = static_cast<JudgeType>(i);
        judgeResult_[type] = 0;
    }

}


void JudgeResult::AddJudge(JudgeType judgeType)
{
    if (judgeType == JudgeType::None)
        return; // None縺ｯ髯､螟・

    // 蛻､螳夂ｵ先棡縺悟ｭ伜惠縺吶ｋ蝣ｴ蜷医・縺ｿ繧ｫ繧ｦ繝ｳ繝・
    if (judgeResult_.find(judgeType) != judgeResult_.end())
    {
        ++judgeResult_[judgeType]; // 蛻､螳夂ｵ先棡繧偵き繧ｦ繝ｳ繝・
    }
}

void JudgeResult::AddJudge(JudgeType judgeType, int32_t count)
{
    if (judgeType == JudgeType::None || count <= 0)
        return; // None縺ｯ髯､螟悶√き繧ｦ繝ｳ繝医′0莉･荳九・蝣ｴ蜷医・辟｡隕・

    // 蛻､螳夂ｵ先棡縺悟ｭ伜惠縺吶ｋ蝣ｴ蜷医・縺ｿ繧ｫ繧ｦ繝ｳ繝・
    if (judgeResult_.find(judgeType) != judgeResult_.end())
    {
        judgeResult_[judgeType] += count; // 蛻､螳夂ｵ先棡繧偵き繧ｦ繝ｳ繝・
    }
}

int32_t JudgeResult::GetJudgeResult(JudgeType judgeType) const
{
    auto it = judgeResult_.find(judgeType);
    if (it != judgeResult_.end())
    {
        return it->second; // 蛻､螳夂ｵ先棡繧定ｿ斐☆
    }
    return 0; // 蟄伜惠縺励↑縺・ｴ蜷医・0繧定ｿ斐☆
}

void JudgeResult::DebugWindow()
{
#ifdef _DEBUG
    ImGui::PushID(this);
    ImGui::Begin("JudgeResult");
    ImGui::Text("JudgeResult");

    ImGui::Text("Perfect : %d", judgeResult_[JudgeType::Perfect]);
    ImGui::Text("Good : %d", judgeResult_[JudgeType::Good]);
    ImGui::Text("Miss : %d", judgeResult_[JudgeType::Miss]);

    ImGui::Text("None : %d", judgeResult_[JudgeType::None]);

    if (ImGui::Button("Reset"))
    {
        for (auto& [type, count] : judgeResult_)
        {
            count = 0; // 蜷・愛螳夂ｵ先棡繧偵Μ繧ｻ繝・ヨ
        }
    }

    ImGui::End();
    ImGui::PopID();

#endif // _DEBUG
}
