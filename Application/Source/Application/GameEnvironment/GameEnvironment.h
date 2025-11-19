#pragma once

#include <Features/Model/ObjectModel.h>
#include <Features/UVTransform/SpriteSheetAnimetion.h>


#include <memory>


class Camera;

/// <summary>
/// 背景等のオブジェクトに関するクラス。
/// </summary>
class GameEnvironment
{
public:

    GameEnvironment() = default;
    ~GameEnvironment() = default;

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

    /// <summary>
    /// アニメーションを開始する。
    /// </summary>
    void StartAnimation();

    void SetSpectrumTextureHandle(uint32_t handle) { spectrumTextureHandle_ = handle; }

private:
    /// <summary>
    /// シーンデータの読み込み（内部処理）。
    /// </summary>
    void Serialize(const std::string& filePath);

    /// <summary>
    /// スピーカーマップを構築する（内部処理）。
    /// </summary>
    void BuildSpeakerMap(const std::string& objName,ObjectModel* model, const std::string& filepath);
private:
    std::vector<std::unique_ptr<ObjectModel>> environmentObjects_ = {};
    std::unique_ptr<ObjectModel> overFloor_ = nullptr;


    std::map<uint32_t, ObjectModel*> speakerMap_;

    std::unique_ptr<ObjectModel> screen_ = nullptr;
    uint32_t spectrumTextureHandle_ = 0;

    SpriteSheetAnimation spriteSheetAnimation_; // 連番画像アニメーション

    std::vector<Vector4> floorColors_ = {
        { 0.2f, 0.2f, 0.2f, 1.0f }, // UnderFloor
        { 0.3f, 0.3f, 0.3f, 1.0f }  // OverFloor
    };

    float timeScale_ = 1.0f; // アニメーションの時間スケール

};