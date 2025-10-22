#pragma once

#include <Math/Vector/Vector3.h>

#include <Features/Model/ObjectModel.h>
#include <Features/Model/Primitive/Plane.h>

#include <vector>

// レーンエフェクトクラス
class LaneEffect
{
public:

    LaneEffect() = default;
    ~LaneEffect() = default;

    // 初期化
    void Initialize(uint32_t _laneIndex, const std::string&  _model);
    // 更新
    void Update(float _deltaTime);
    // 描画
    void Draw(const Camera* _camera);
    // エフェクト開始
    void Start();
    // エフェクト終了
    void End();


    // == アクセッサ ==
    void SetDuration(float _duration) { duration_ = _duration; }
    float GetDuration() const { return duration_; }

    void SetColor(const Vector4& _color) { color_ = _color; }
    float GetTimer() const { return timer_; }

private:

    float duration_ = 0.0f; // エフェクトの持続時間
    float timer_    = 0.0f; // エフェクトのタイマー

    bool isActive_ = true; // エフェクトがアクティブかどうか

    Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f }; // エフェクトの色

    std::unique_ptr<ObjectModel> laneModel_; // レーンエフェクトのモデル

    static Vector4  defoultColor_;  // デフォルトの色
    static uint32_t textureHandle_; // テクスチャハンドル

};