#pragma once

#include <Application/Result/UI/ResultUI.h>
#include <Application/Result/ResultModelEffect.h>
#include <Application/Result/ResultData.h>

namespace Engine { class Camera; }

/// <summary>
/// 結果エフェクトの流れを管理するクラス
/// </summary>
class ResultEffectFlow
{
public:

    void Initialize(const ResultData& resultData);
    void Update(float deltaTime);
    void Draw3D();
    void Draw2D();


private:

    std::unique_ptr<ResultUI> resultUI_;
    std::unique_ptr<ResultModelEffect> resultModelEffect_;
};