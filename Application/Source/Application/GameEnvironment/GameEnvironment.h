#pragma once

#include <Features/Model/ObjectModel.h>
#include <Features/UVTransform/SpriteSheetAnimetion.h>


#include <memory>


class Camera;

// 背景等のオブジェクトに関するクラス
class GameEnvironment
{

public:

    GameEnvironment() = default;
    ~GameEnvironment() = default;

    // 初期化
    void Initialize(const std::string& _filePath = "Resources/Data/Game/Environment.json");
    // 更新
    void Update(float _deltaTime);
    // 描画
    void Draw(const Camera* _camera);
    // BPM設定
    void SetBPM(float _bpm);
    // スピーカーオブジェクトの取得
    ObjectModel* GetSpeaker(uint32_t _laneIndex);
    // アニメーション開始
    void StartAnimation();
private:
    // シリアライズ
    // Blenderで作成したシーンデータを読み込み
    void Serialize(const std::string& _filePath);
    // スピーカーマップの構築
    void BuildSpeakerMap(const std::string& _objName,ObjectModel* _model, const std::string& _filepath);

private:
    std::vector<std::unique_ptr<ObjectModel>> environmentObjects_ = {};
    std::unique_ptr<ObjectModel> overFloor_ = nullptr;

    std::map<uint32_t, ObjectModel*> speakerMap_;

    SpriteSheetAnimation spriteSheetAnimation_; // 連番画像アニメーション

    std::vector<Vector4> floorColors_ = {
        { 0.2f, 0.2f, 0.2f, 1.0f }, // UnderFloor
        { 0.3f, 0.3f, 0.3f, 1.0f }  // OverFloor
    };

    float timeScale_ = 1.0f; // アニメーションの時間スケール

};