#pragma once

#include <Features/Effect/Particle/Particle.h>
#include <Features/Effect/Emitter/ParticleEmitter.h>

#include <vector>
#include <string>
#include <cstdint>

/// <summary>
/// トリガー（タップ）時に複数のパーティクルやエフェクトを発生させるユーティリティクラス。
/// </summary>
class TriggerEffects
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    TriggerEffects() {};

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~TriggerEffects() {};

    /// <summary>
    /// エフェクトシステム全体の初期化を行う。
    /// </summary>
    static void Initialize();

    /// <summary>
    /// 指定位置に中心の円形エフェクトを発生させる。
    /// </summary>
    /// <param name="pos">発生位置（ワールド座標）</param>
    static void EmitCenterCircles(const Engine::Vector3& pos);

    /// <summary>
    /// 指定位置の周囲に散開するパーティクルを発生させる。
    /// </summary>
    /// <param name="pos">発生位置（ワールド座標）</param>
    /// <param name="combo">コンボ数（エフェクトの強さに影響）</param>
    static void EmitSurroundingParticles(const Engine::Vector3& pos, int32_t comboLevel);

private:

    static uint32_t countPerEmit_;  // 一度に発生するパーティクルの数
    static float    baseSize_;      // 基本のサイズ
    static float    centerSize_;    // 中心のサイズ

    static Engine::Vector4 commonColor_; // 共通の色

    static uint32_t textureHandle_;     // テクスチャハンドル
    static uint32_t gradationTexture_;  // グラデーション用のテクスチャ


    static Engine::ParticleEmitter cubePop_;            // キューブを出すエミッタ
    static Engine::ParticleEmitter risingParticles_;    // 上昇するパーティクル
    static Engine::ParticleEmitter lightPillarEmitter_; // 光の柱を出すエミッタ
    static Engine::ParticleEmitter triangleEmitter_;    // 三角形を出すエミッタ

};
