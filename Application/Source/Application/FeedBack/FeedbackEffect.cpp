#include "FeedbackEffect.h"

#include <Features/Camera/Camera/Camera.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <Application/Lane/Lane.h>

#include <Application/GameEnvironment/GameEnvironment.h>

void FeedbackEffect::Initialize(Camera* camera, int32_t laneCount, GameEnvironment* gameEnvironment)
{
    if (camera)
        camera_ = camera;
    gameEnvironment_ = gameEnvironment;

    judgeSound_ = std::make_unique<JudgeSound>();
    judgeSound_->Initialize();

    judgeEffect_ = std::make_unique<JudgeEffect>();
    judgeEffect_->Initialize();

    tapEffect_ = std::make_unique<TapEffect>();
    tapEffect_->Initialize();

    backgroundEffect_ = std::make_unique<BackgroundEffect>();
    backgroundEffect_->SetGameEnvironment(gameEnvironment_);

    noteHoldEffect_ = std::make_unique<NoteHoldEffect>();
    noteHoldEffect_->Initialize();

    speakerSeekEffect_ = std::make_unique<SpeakerSeekEffect>();
    speakerSeekEffect_->Initialize();

    laneEdgeEffects_ = std::make_unique<LaneEdgeEffects>();
    laneEdgeEffects_->Initialize();

    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        judgeTextPool_[i] = std::make_unique<JudgeText>();
        usedJudgeTexts_.set(i, false); // 初期化時は全て未使用
    }

    // ミス時のビネットエフェクトの初期化
    missedVignette_ = std::make_unique<MissedVignette>();
    missedVignette_->Initialize();


    for (int32_t i = 0; i < laneCount; ++i)
    {
        auto laneEffect = std::make_unique<LaneEffect>();
        laneEffect->Initialize(i, "pY1x1p01Plane");

        laneEffects_.push_back(std::move(laneEffect)); // レーンエフェクトを追加
    }
}


void FeedbackEffect::Update(float deltaTime, const std::vector<InputData>& inputData)
{
    //DebugWindow(); // デバッグウィンドウの更新

    if (judgeSound_)
        judgeSound_->CleanupStoppedVoices(); // 停止した音声をクリーンアップ

    if (speakerSeekEffect_)
        speakerSeekEffect_->Update(deltaTime);

    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        if (usedJudgeTexts_[i]) // 使用中のテキストのみ更新
        {
            judgeTextPool_[i]->Update(deltaTime);
            if (judgeTextPool_[i]->IsFinished()) // 終了したテキストは未使用に戻す
            {
                usedJudgeTexts_.set(i, false);
            }
        }
    }

    if (missedVignette_)
        missedVignette_->Update(deltaTime);

    for (const auto& input : inputData)
    {
        if (input.state == KeyState::Trigger)
            tapEffect_->Play(input.laneIndex);

        if (input.state == KeyState::Trigger || input.state == KeyState::Hold)
            laneEffects_[input.laneIndex]->Start();
    }

    for (auto& laneEffect : laneEffects_)
    {
        if (laneEffect)
        {
            laneEffect->Update(deltaTime);
        }
    }

    if (laneEdgeEffects_)
        laneEdgeEffects_->Update(deltaTime);
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

void FeedbackEffect::PlayJudgeEffect(int32_t laneIndex, JudgeType judgeType,int32_t combo)
{
    // 各エフェクトの再生

    int32_t comboLevel = comboThresholds_.GetComboLevel(combo);
    bool levelUp = (comboLevel != prevComboLevel_) && (comboLevel > prevComboLevel_);

    if (judgeSound_)
        judgeSound_->Play();

    if (judgeEffect_)
    {
        judgeEffect_->Play(laneIndex, comboLevel);
    }

    if (levelUp && laneEdgeEffects_)
    {
        laneEdgeEffects_->Emit();
    }

    PlaySpeakerSeekEffect(laneIndex, judgeType);
    if (backgroundEffect_)
        backgroundEffect_->PlaySpeakerEffect(laneIndex, speakerSeekEffect_->GetDuration());


    AllocateJudgeText(judgeType, laneIndex); // 判定テキストを割り当てる

    prevComboLevel_ = comboLevel;
}

void FeedbackEffect::PlayMissedEffect()
{
    if (missedVignette_)
    {
        missedVignette_->Emit(); // ミス時のビネットエフェクトを発動
    }
}

void FeedbackEffect::PlaySpeakerSeekEffect(int32_t laneIndex, [[maybe_unused]] JudgeType judgeType)
//判定によって変更したい
{
    static const Vector3 speakerOffset = Vector3(0, 1, 0);
    if (speakerSeekEffect_)
    {
        Vector3 speakerPos = gameEnvironment_->GetSpeaker(laneIndex)->GetWorldTransform()->GetWorldPosition();
        speakerSeekEffect_->Emit(speakerPos + speakerOffset, Lane::GetLaneEndPosition(laneIndex), kMaxSeekEffects_); // スピーカーに向かうエフェクトを発生
    }
}

void FeedbackEffect::PlayHoldEffect(int32_t laneIndex)
{
    if (noteHoldEffect_)
    {
        noteHoldEffect_->Play(laneIndex); // ホールドエフェクトを再生
    }
}

void FeedbackEffect::ApplyMissedVignetteEffect(const std::string& input, const std::string& output)
{
    if (missedVignette_)
    {
        missedVignette_->ApplyEffect(input, output); // ビネットエフェクトを適用
    }
}

void FeedbackEffect::InitComboThresholds(int32_t maxCombo)
{
    comboThresholds_.Initialize(maxCombo);
}

void FeedbackEffect::AllocateJudgeText(JudgeType judgeType, int32_t laneIndex)
{
    for (int32_t i = 0; i < judgeTextPool_.size(); ++i)
    {
        if (!usedJudgeTexts_[i]) // 未使用のテキストを探す
        {
            usedJudgeTexts_.set(i); // 使用中に設定
            judgeTextPool_[i]->Initialize(judgeType, laneIndex, camera_);
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
