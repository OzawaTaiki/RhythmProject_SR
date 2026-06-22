#pragma once

#include <Math/Vector/Vector2.h>
#include <Math/Vector/Vector3.h>
#include <Math/Vector/Vector4.h>

#include <memory>
#include <vector>
#include <algorithm>

namespace Engine
{
class Camera;
class AudioSpectrum;
class InstancedObjectModel;
}

/// <summary>
/// 観客のペンライト群。インスタンシングで大量描画する。
/// ・揺れ：ベーススイング × 低〜中域FFTエネルギーで振幅（個体ごとに位相をずらす）
/// ・色＆本数：コンボ数で本数が増え、カラフルになる
/// </summary>
class PenlightCrowd
{
public:
    PenlightCrowd() = default;
    ~PenlightCrowd() = default;

    /// <summary>
    /// 初期化。areaCenter を中心に cols×rows 本を格子配置する。
    /// </summary>
    void Initialize(const Engine::Vector3& areaCenter,
                    const Engine::Vector2& areaSize,
                    int32_t cols, int32_t rows);

    /// <summary>毎フレーム更新。audioSpectrum から低〜中域エネルギーを取って揺らす。</summary>
    void Update(float deltaTime, Engine::AudioSpectrum* audioSpectrum);

    void Draw(const Engine::Camera* camera);

    /// <summary>コンボ比率(0..1)を設定。表示本数とカラフルさが増す。GameBackgroundから渡される。</summary>
    void SetComboRatio(float ratio) { comboRatio_ = std::clamp(ratio, 0.0f, 1.0f); }

private:
    struct Penlight
    {
        Engine::Vector3 basePos{};      // 根元位置
        float phase = 0.0f;             // スイング位相オフセット（群衆のバラつき）
        float hue = 0.0f;               // ランダム段階で使う色相(0..1)
        float stage1Threshold = 0.0f;   // この比率を超えると白→ライトブルーへ
        float stage2Threshold = 0.0f;   // この比率を超えるとランダム色へ
        float visibility = 0.0f;        // 表示フェード(0=非表示 〜 1=表示)。増減時に補間する
    };

    // コンボ比率(0..1)とペンライト個体から最終色を決める
    Engine::Vector4 ResolveColor(const Penlight& p, float comboRatio, float brightness) const;

    // 配置エリアのデバッグ枠線を描く（_DEBUGのみ）
    void DrawDebugArea();

    // 低〜中域エネルギー(0..1)を取得する
    float ComputeEnergy(Engine::AudioSpectrum* audioSpectrum);

private:
    std::unique_ptr<Engine::InstancedObjectModel> model_;
    std::vector<Penlight> lights_;

    int32_t maxCount_ = 0;       // 全本数（格子の総数）
    float comboRatio_ = 0.0f;    // コンボ比率(0..1)。ComboThresholds経由で外部から渡される
    int32_t visibleCount_ = 0;   // コンボから決まる表示本数

    // 揺れ
    float swingTime_ = 0.0f;          // 全体の進行時間
    float swingSpeed_ = 3.0f;         // スイングの速さ
    float swingBaseDeg_ = 8.0f;       // 最低振れ角（音が無くても少し揺れる）
    float swingEnergyDeg_ = 22.0f;    // エネルギーで増える振れ角
    float energyLevel_ = 0.0f;        // 平滑化したエネルギー
    float energyResponse_ = 6.0f;     // 追従の速さ

    // 表示フェード
    float fadeSpeed_ = 3.0f;          // 増減フェードの速さ（1秒あたり）

    // 発光
    float glowBase_ = 0.4f;           // 最低の明るさ
    float glowEnergy_ = 0.6f;         // エネルギーで増える明るさ

    // 帯域（低〜中域）
    Engine::Vector2 hzRange_{ 60.0f, 2000.0f };
    size_t beginIndex_ = 0;
    size_t endIndex_ = 0;
    std::vector<float> spectrumData_;

    float penlightLength_ = 1.2f;     // ペンライトの長さ

    Engine::Vector3 areaCenter_ = {};
    Engine::Vector3 areaSize_ = {};
};
