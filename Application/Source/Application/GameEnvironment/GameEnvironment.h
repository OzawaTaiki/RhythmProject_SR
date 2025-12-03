#pragma once

#include <Features/Model/ObjectModel.h>
#include <Features/UVTransform/SpriteSheetAnimetion.h>
#include <System/Time/Stopwatch.h>
#include <Features/Event/EventListener.h>


#include <memory>


struct ColorChangeEvent : EventData
{
    ObjectModel* targets;
};

class Camera;

/// <summary>
/// 背景等のオブジェクトに関するクラス。
/// </summary>
class GameEnvironment : public iEventListener
{
public:

    GameEnvironment();
    ~GameEnvironment();

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    /// <param name="filePath">環境設定ファイルのパス（省略可）</param>
    void Initialize(const std::string& filePath = "Resources/Data/Game/Environment.json");

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    void Update(float deltaTime);

    /// <summary>
    /// 描画処理を行う。
    /// </summary>
    /// <param name="camera">描画用カメラ</param>
    void Draw(const Camera* camera);

    /// <summary>
    /// BPMを設定して環境に反映する。
    /// </summary>
    void SetBPM(float bpm);

    /// <summary>
    /// 指定レーンのスピーカーオブジェクトを取得する。
    /// </summary>
    ObjectModel* GetSpeaker(uint32_t laneIndex);

    void SetSpectrumTextureHandle(uint32_t handle) { spectrumTextureHandle_ = handle; }

    void OnEvent(const GameEvent& event) override;

private:
    /// <summary>
    /// シーンデータの読み込み（内部処理）。
    /// </summary>
    void Serialize(const std::string& filePath);

    /// <summary>
    /// スピーカーマップを構築する（内部処理）。
    /// </summary>
    void BuildSpeakerMap(const std::string& objName,ObjectModel* model, const std::string& filepath);

    void UpdateSpeakerAnimation(float deltaTime);

    // Wallの初期化
    void InitializeWall(ObjectModel* wallModel);

    void InitializeOverlayFloor();

    void CreateEmissivePSO();
private:
    std::vector<std::unique_ptr<ObjectModel>> environmentObjects_ = {};
    std::unique_ptr<ObjectModel> overFloor_ = nullptr;
    std::unique_ptr<ObjectModel> overlayFloor_ = nullptr;


    std::map<uint32_t, ObjectModel*> speakerMap_;

    std::unique_ptr<ObjectModel> screen_ = nullptr;
    uint32_t spectrumTextureHandle_ = 0;

    std::map<ObjectModel*, float> speakerColorTimers_; // スピーカーごとの色変化タイマー

    Stopwatch stopwatch_;
    int32_t currentAnimationStep_ = 0;

    float animationTimer_ = 0.0f;
    float animationInterval_ = 1.0f; // アニメーションのステップ間隔

    float timeScale_ = 1.0f; // アニメーションの時間スケール

    bool enableEmissive_ = true;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> emissivePso_ = nullptr;
};