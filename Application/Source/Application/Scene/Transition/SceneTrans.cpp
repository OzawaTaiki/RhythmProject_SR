#include "SceneTrans.h"

#include <System/Time/Time.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Core/WinApp/WinApp.h>
#include <Framework/LayerSystem/LayerSystem.h>

void SceneTrans::Initialize()
{
    duration_ = 1.0f; // トランジションの持続時間
    elapsedTime_ = 0.0f; // 経過時間
    alpha_ = 0.0f; // トランジションのアルファ値
    isEnd_ = false; // トランジションが終了したか
    canSwitch_ = false; // シーンを切り替え可能か
    playing_ = false; // トランジションが再生中か

    uint32_t handle = TextureManager::GetInstance()->Load("white.png");
    // トランジション用のスプライトを初期化
    transitionSprite_ = Sprite::Create("TransitionSprite", handle);
    transitionSprite_->translate_ = WinApp::kWindowSize_ * 0.5f; // 画面中央に配置
    transitionSprite_->SetSize(WinApp::kWindowSize_); // 画面全体を覆うサイズに設定
    transitionSprite_->SetColor(Vector4(0, 0, 0, 1)); // 初期は黒色

    LayerSystem::CreateLayer("SceneTransition", 100000); // トランジション用のレイヤーを作成
}

void SceneTrans::Update()
{
    if (!playing_) return; // トランジションが再生中でない場合は何もしない

    if (!canSwitch_)
    {
        elapsedTime_ += Time::GetDeltaTime<float>(); // 経過時間を更新
        if (elapsedTime_ >= duration_)
        {
            elapsedTime_ = duration_; // 経過時間が持続時間を超えたら持続時間に設定
            canSwitch_ = true; // シーンを切り替え可能にする
        }
    }
    else
    {
        elapsedTime_ -= Time::GetDeltaTime<float>(); // 経過時間を逆に更新
        if (elapsedTime_ <= 0.0f)
        {
            elapsedTime_ = 0.0f; // 経過時間が0以下になったら0に設定
            isEnd_ = true; // トランジションが終了したとマーク
            playing_ = false; // トランジションの再生を停止
        }
    }

    // アルファ値の計算
    alpha_ = elapsedTime_ / duration_;

    // トランジションスプライトのアルファ値を更新
    if (transitionSprite_)
    {
        transitionSprite_->SetColor(Vector4(0, 0, 0, alpha_)); // 黒色のアルファ値を更新
        transitionSprite_->Update();
    }
}

void SceneTrans::Draw()
{
    if (!playing_)
        return; // トランジションが再生中でない場合は何もしない

    if (transitionSprite_)
    {
        LayerSystem::SetLayer("SceneTransition");
        Sprite::PreDraw();
        transitionSprite_->Draw(); // トランジションスプライトを描画
    }
}

void SceneTrans::Start()
{
    duration_ = 0.5f; // トランジションの持続時間
    isEnd_ = false; // トランジションが終了したか
    canSwitch_ = false; // シーンを切り替え可能か

    playing_ = true; // トランジションが再生中か
}

void SceneTrans::End()
{
}
