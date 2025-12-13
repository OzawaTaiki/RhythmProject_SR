#pragma once

#include <Features/Effect/Particle/Particle.h>
#include <Math/Vector/Vector3.h>
#include <vector>
#include <memory>

/// <summary>
/// 楕円軌道情報
/// </summary>
struct EllipticalPath
{
    Vector3 startPos;
    Vector3 targetPos;
    float time;              // 現在の進行度（0.0～1.0）
    float duration;          // 到達までの時間
    float verticalBulge;     // 縦方向の膨らみ
    float horizontalBulge;   // 横方向の膨らみ
    Vector3 worldUp;         // ワールドの上方向

    EllipticalPath()
        : startPos(0, 0, 0)
        , targetPos(0, 0, 0)
        , time(0.0f)
        , duration(1.0f)
        , verticalBulge(1.0f)
        , horizontalBulge(0.0f)
        , worldUp(0, 1, 0)
    {
    }
};

/// <summary>
/// スピーカーに向かって楕円軌道で移動するパーティクルエフェクト
/// </summary>
class SpeakerSeekEffect
{
public:
    SpeakerSeekEffect() = default;
    ~SpeakerSeekEffect() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新処理（毎フレーム呼ぶ）
    /// </summary>
    /// <param name="deltaTime">経過時間</param>
    void Update(float deltaTime);

    /// <summary>
    /// パーティクルを発生させる
    /// </summary>
    /// <param name="startPos">開始位置</param>
    /// <param name="targetPos">目標位置（スピーカーの位置）</param>
    /// <param name="count">発生させるパーティクル数</param>
    void Emit(const Vector3& startPos, const Vector3& targetPos, uint32_t count = 1);

    float GetDuration() const;

    /// <summary>
    /// 全パーティクルをクリア
    /// </summary>
    void Clear();

    // 楕円軌道のパラメータ設定
    void SetWorldUp(const Vector3& up) { worldUp_ = up; }

private:
    /// <summary>
    /// 楕円軌道上の位置を計算
    /// </summary>
    Vector3 CalculateEllipticalPosition(const EllipticalPath& path) const;

    std::vector<Particle*> particles_;  // 生ポインタで保持（所有権はParticleSystemにある）
    std::vector<EllipticalPath> paths_;  // 各パーティクルの楕円軌道情報

    // デフォルト設定
    Vector3 worldUp_ = Vector3(0, 1, 0);  // ワールドの上方向

    uint32_t textureHandle_ = 0;
};