#pragma once

#include <cstdint>

// 判定時にエフェクト
/// <summary>
/// 判定時に表示するパーティクル等のエフェクトを管理するクラス。
/// </summary>
class JudgeEffect
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    JudgeEffect() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~JudgeEffect() = default;

    /// <summary>
    /// エフェクトの初期化処理。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 指定レーンでエフェクトを再生する。
    /// </summary>
    /// <param name="laneIndex">レーンのインデックス</param>
    /// <param name="combo">コンボ数</param>
    void Play(int32_t laneIndex,int32_t combo);
};