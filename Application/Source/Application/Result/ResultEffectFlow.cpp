#include "ResultEffectFlow.h"

void ResultEffectFlow::Initialize(const ResultData& resultData)
{
    resultUI_ = std::make_unique<ResultUI>();
    resultUI_->Initialize(resultData);

    resultModelEffect_ = std::make_unique<ResultModelEffect>();
    resultModelEffect_->Initialize(resultData);
}

void ResultEffectFlow::Update(float deltaTime)
{
    resultModelEffect_->Update(deltaTime);

    if(resultModelEffect_->IsAnimationEnd())
    // モデルエフェクトのアニメーションが終了してからUIを更新
        resultUI_->Update(deltaTime);
}

void ResultEffectFlow::Draw3D()
{
    resultModelEffect_->Draw();
}

void ResultEffectFlow::Draw2D()
{
    resultUI_->Draw();
}
