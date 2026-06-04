#pragma once

#include <Features/Model/ObjectModel.h>
#include <Features/UVTransform/SpriteSheetAnimetion.h>
#include <System/Time/Stopwatch.h>
#include <Features/Event/EventListener.h>


#include "SpectrumBar.h"
#include "SpectrumFloor.h"
#include "SpeakerSpectrumReaction.h"
#include "SpotLightBeam.h"
#include "PenlightCrowd.h"

#include <Application/FeedBack/ComboThresholds.h>

#include <Features/AudioSpectrum/OnsetDetector.h>

#include <memory>
#include <vector>


struct ColorChangeEvent : Engine::EventData
{
    Engine::ObjectModel* targets;
    float delayTime = 0.0f;
};

namespace Engine
{
class Camera;
}

/// <summary>
/// 背景等のオブジェクトに関するクラス。
/// </summary>
class GameBackground : public Engine::iEventListener
{
public:

    GameBackground();
    ~GameBackground();

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    /// <param name="filePath">環境設定ファイルのパス（省略可）</param>
    void Initialize(const std::string& filePath = "Resources/Data/Game/Environment.json");

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    void Update(float deltaTime, Engine::AudioSpectrum* audioSpectrum, Engine::SoundInstance* soundInstance, float duration);

    /// <summary>
    /// 描画処理を行う。
    /// </summary>
    /// <param name="camera">描画用カメラ</param>
    void Draw(const Engine::Camera* camera);

    /// <summary>
    /// BPMを設定して環境に反映する。
    /// </summary>
    void SetBPM(float bpm);

    /// <summary>
    /// コンボ数を設定する（ペンライト群の本数・彩度に反映）。
    /// </summary>
    void SetCombo(int32_t combo);

    /// <summary>
    /// コンボ閾値定義を設定する。SetComboでの比率計算に使用。
    /// </summary>
    void SetComboThresholds(const ComboThresholds* thresholds) { comboThresholds_ = thresholds; }

    /// <summary>
    /// 指定レーンのスピーカーオブジェクトを取得する。
    /// </summary>
    Engine::ObjectModel* GetSpeaker(uint32_t laneIndex);

    void OnEvent(const Engine::GameEvent& event) override;

private:
    /// <summary>
    /// シーンデータの読み込み（内部処理）。
    /// </summary>
    void Serialize(const std::string& filePath);

    /// <summary>
    /// スピーカーマップを構築する（内部処理）。
    /// </summary>
    void BuildSpeakerMap(const std::string& objName, Engine::ObjectModel* model, const std::string& filepath);

    void UpdateSpeakerAnimation(float deltaTime);

    // Wallの初期化
    void InitializeWall(Engine::ObjectModel* wallModel);

    void InitializeOverlayFloor();

    void CreateEmissivePSO();

    // ビーム（可視光線）用の加算PSOを生成する
    void CreateBeamPSO();

    // FFT連動演出（スピーカー反応・ドロップ検出・スポットライト）の初期化
    void InitializeFFTReaction();
private:
    std::unique_ptr<SpectrumFloor> spectrumFloor_ = nullptr; //
    std::vector<std::unique_ptr<Engine::ObjectModel>> backgroundObjects_ = {};
    std::unique_ptr<Engine::ObjectModel> overFloor_ = nullptr;
    std::unique_ptr<Engine::ObjectModel> overlayFloor_ = nullptr;
    std::vector<std::unique_ptr<SpectrumBar>> spectrumBars_;// スペクトラムバーの配列


    std::map<uint32_t, Engine::ObjectModel*> speakerMap_;

    std::map<Engine::ObjectModel*, float> speakerColorTimers_; // スピーカーごとの色変化タイマー

    Engine::Stopwatch stopwatch_;
    int32_t currentAnimationStep_ = 0;

    float animationTimer_ = 0.0f;
    float animationInterval_ = 1.0f; // アニメーションのステップ間隔

    float timeScale_ = 1.0f; // アニメーションの時間スケール

    bool enableEmissive_ = true;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> emissivePso_ = nullptr;

    // ---- FFT連動演出 ----
    std::vector<std::unique_ptr<SpeakerSpectrumReaction>> speakerReactions_;
    std::unique_ptr<OnsetDetector> onsetDetector_;
    std::vector<std::unique_ptr<SpotLightBeam>> ceilingBeams_; // 天井トラスのスポットライト
    std::vector<std::unique_ptr<SpotLightBeam>> backBeams_;    // ステージ奥の逆光
    std::unique_ptr<PenlightCrowd> penlightCrowd_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> beamPso_ = nullptr;

    // 天井ビームの感度・下限（ImGui調整用）
    float beamEnergyGain_ = 8.0f;
    float beamEnergyFloor_ = 0.3f;
    // 奥ビームの感度・下限（ImGui調整用）
    float backBeamGain_ = 12.0f;
    float backBeamFloor_ = 0.25f;
    float comboColorRatio_ = 0.0f;
    const ComboThresholds* comboThresholds_ = nullptr;
};
