#include "FeedbackEffect.h"

#include <Features/Camera/Camera/Camera.h>
#include <Framework/LayerSystem/LayerSystem.h>

#include <Application/GameEnvironment/GameEnvironment.h>

void FeedbackEffect::Initialize(Camera* _camera, int32_t _laneCount, GameEnvironment* _gameEnvironment)
{
    if (_camera)
        camera_ = _camera;

    judgeSound_ = std::make_unique<JudgeSound>();
    judgeSound_->Initialize();

    judgeEffect_ = std::make_unique<JudgeEffect>();
    judgeEffect_->Initialize();

    tapEffect_ = std::make_unique<TapEffect>();
    tapEffect_->Initialize();

    backgroundEffect_ = std::make_unique<BackgroundEffect>();
    backgroundEffect_->SetGameEnvironment(_gameEnvironment);

    noteHoldEffect_ = std::make_unique<NoteHoldEffect>();
    noteHoldEffect_->Initialize();

    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        judgeTextPool_[i] = std::make_unique<JudgeText>();
        usedJudgeTexts_.set(i, false); // 初期化時は全て未使用
    }

    // ミス時のビネットエフェクトの初期化
    missedVignette_ = std::make_unique<MissedVignette>();
    missedVignette_->Initialize();


    for (int32_t i = 0; i < _laneCount; ++i)
    {
        auto laneEffect = std::make_unique<LaneEffect>();
        laneEffect->Initialize(i, "pY1x1p01Plane");

        laneEffects_.push_back(std::move(laneEffect)); // レーンエフェクトを追加
    }

}


void FeedbackEffect::Update(float _deltaTime, const std::vector<InputDate>& _inputData)
{
    //DebugWindoow(); // デバッグウィンドウの更新

    if (judgeSound_)
        judgeSound_->CleanupStoppedVoices(); // 停止した音声をクリーンアップ

    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        if (usedJudgeTexts_[i]) // 使用中のテキストのみ更新
        {
            judgeTextPool_[i]->Update(_deltaTime);
            if (judgeTextPool_[i]->IsFinished()) // 終了したテキストは未使用に戻す
            {
                usedJudgeTexts_.set(i, false);
            }
        }
    }

    if (missedVignette_)
        missedVignette_->Update(_deltaTime);

    for (const auto& input : _inputData)
    {
        if (input.state == KeyState::trigger)
            tapEffect_->Play(input.laneIndex);

        if (input.state == KeyState::trigger || input.state == KeyState::Hold)
            laneEffects_[input.laneIndex]->Start();
    }

    for (auto& laneEffect : laneEffects_)
    {
        if (laneEffect)
        {
            laneEffect->Update(_deltaTime);
        }
    }

}

void FeedbackEffect::Draw() 
{
    LayerSystem::SetLayer("GameCore");
    for (const auto& laneEffect : laneEffects_)
    {
        if (laneEffect)
        {
            laneEffect->Draw(camera_); // レーンエフェクトの描画
        }
    }

    LayerSystem::SetLayer("FeedbackEffect");
    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        if (usedJudgeTexts_[i]) // 使用中のテキストのみ描画
        {
            judgeTextPool_[i]->Draw();
        }
    }
}

void FeedbackEffect::PlayJudgeEffect(int32_t _laneIndex, JudgeType _judgeType)
{

    if (judgeSound_)
        judgeSound_->Play();

    if (judgeEffect_)
        judgeEffect_->Play(_laneIndex);

    if (backgroundEffect_)
        backgroundEffect_->PlaySpeakerEffect(_laneIndex);


    AllocateJudgeText(_judgeType, _laneIndex); // 判定テキストを割り当てる
}

void FeedbackEffect::PlayMissedEffect()
{
    if (missedVignette_)
    {
        missedVignette_->Emit(); // ミス時のビネットエフェクトを発動
    }
}

void FeedbackEffect::PlayHoldEffect(int32_t _laneIndex)
{
    if (noteHoldEffect_)
    {
        noteHoldEffect_->Play(_laneIndex); // ホールドエフェクトを再生
    }
    // TODO : 音
}

void FeedbackEffect::ApplyMissedVignetteEffect(const std::string& _input, const std::string& _output)
{
    if (missedVignette_)
    {
        missedVignette_->ApplyEffect(_input, _output); // ビネットエフェクトを適用
    }
}

void FeedbackEffect::AllocateJudgeText(JudgeType _judgeType, int32_t _laneIndex)
{
    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        if (!usedJudgeTexts_[i]) // 未使用のテキストを探す
        {
            usedJudgeTexts_.set(i); // 使用中に設定
            judgeTextPool_[i]->Initialize(_judgeType, _laneIndex, camera_);
            return; // 割り当て完了
        }
    }

}

void FeedbackEffect::DebugWindoow()
{
#ifdef _DEBUG

    ImGui::Begin("FeedbackEffect Debug");
    {
        ImGui::SeparatorText("JudgeText");
        if (ImGui::Button("Perfect"))
            AllocateJudgeText(JudgeType::Perfect, 0);
        ImGui::SameLine();
        if (ImGui::Button("Good"))
            AllocateJudgeText(JudgeType::Good, 1);
        ImGui::SameLine();
        if (ImGui::Button("Miss"))
            AllocateJudgeText(JudgeType::Miss, 2);

    }
    ImGui::End();

#endif
}
