#pragma once

#include <Math/Vector/Vector3.h>

#include <Features/Model/ObjectModel.h>
#include <Features/Model/Primitive/Plane.h>

#include <vector>

// レーンエフェクトクラス
/// <summary>
/// 個別レーンに適用される視覚エフェクトを管理するクラス。
/// </summary>
class LaneEffect
{
public:

    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    LaneEffect() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~LaneEffect() = default;

    /// <summary>
    /// 初期化処理。
    /// </summary>
    /// <param name="_laneIndex">レーンのインデックス</param>
    /// <param name="_model">使用するモデル名</param>
    void Initialize(uint32_t _laneIndex, const std::string&  _model);

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    /// <param name="_deltaTime">経過時間（秒）</param>
    void Update(float _deltaTime);

    /// <summary>
    /// 描画処理。
    /// </summary>
    /// <param name="_camera">描画に使用するカメラ</param>
    void Draw(const Camera* _camera);

    /// <summary>
    /// エフェクトを開始する。
    /// </summary>
    void Start();

    /// <summary>
    /// エフェクトを終了する。
    /// </summary>
    void End();


    // == アクセッサ ==

    /// <summary>
    /// エフェクト持続時間を設定する。
    /// </summary>
    void SetDuration(float _duration) { duration_ = _duration; }

    /// <summary>
    /// 現在設定されている持続時間を取得する。
    /// </summary>
    float GetDuration() const { return duration_; }

    /// <summary>
    /// エフェクトの色を設定する。
    /// </summary>
    void SetColor(const Vector4& _color) { color_ = _color; }

    /// <summary>
    /// 現在のタイマー値を取得する。
    /// </summary>
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