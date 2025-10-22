#include "LaneEffect.h"

#include <Application/Lane/Lane.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Math/MyLib.h>

Vector4 LaneEffect::defoultColor_ = { 0.5f, 0.7f, 0.8f, 0.5f }; // デフォルトの色を設定
uint32_t LaneEffect::textureHandle_ = UINT_MAX; // テクスチャハンドルの初期化

void LaneEffect::Initialize(uint32_t _laneIndex, const std::string& _model)
{
    auto model = std::make_unique<ObjectModel>("laneEffectPlane");
    model->Initialize(_model);
    Vector3 laneCenter = { 0.0f, 0.0f, 0.0f }; // レーンの中心座標を設定
    laneCenter = Lane::GetLaneEndPosition(_laneIndex);
    laneCenter.y -= 0.001f; // zファイティング対策で少し下げる
    laneCenter.z += Lane::GetLaneLength();
    model->translate_ = laneCenter; // レーンの中心位置に移動

    laneModel_ = std::move(model);

    laneModel_->scale_ = Vector3(Lane::GetLaneWidth() - 0.1f, 1.0f, Lane::GetLaneLength()); // レーンの幅と長さを設定

    duration_ = 0.5f; // デフォルトの持続時間を設定
    timer_ = 0.0f; // タイマーをリセット
    isActive_ = false; // エフェクトをアクティブに設定
    color_ = { 0.5f, 0.5f, 1.0f, 1.0f }; // デフォルトの色を設定

    if (textureHandle_ == UINT_MAX)
    {// テクスチャがまだ読み込まれていない場合にのみ読み込む
        textureHandle_ = TextureManager::GetInstance()->Load("guradation.png");
    }

}

void LaneEffect::Update(float _deltaTime)
{
    if (!isActive_)
    {
        return; // エフェクトがアクティブでない場合は何もしない
    }

    timer_ += _deltaTime;
    if (timer_ > duration_)
    {
        timer_ = duration_; // リセット
        End();
    }

    // フェードアウト
    float progress = timer_ / duration_;
    color_.w = Lerp(defoultColor_.w, 0.0f, progress);

    laneModel_->Update();
}

void LaneEffect::Draw(const Camera* _camera)
{
    if (!isActive_)
    {
        return; // エフェクトがアクティブでない場合は描画しない
    }

    // laneModels_の各モデルを描画
    if (laneModel_)
    {
        laneModel_->Draw(_camera, textureHandle_, color_);
    }
}

void LaneEffect::Start()
{
    isActive_ = true;
    timer_ = 0.0f; // タイマーをリセット
}

void LaneEffect::End()
{
    isActive_ = false; // エフェクトを非アクティブに設定
    timer_ = 0.0f; // タイマーをリセット
}
