#pragma once

#include <cstdint>

// タップエフェクトクラス
/// <summary>
/// タップ時に発生する視覚エフェクトを管理するクラス。
/// </summary>
class TapEffect
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    TapEffect() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~TapEffect() = default;

    /// <summary>
    /// エフェクトの初期化を行う。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 指定レーンでエフェクトを再生する。
    /// </summary>
    /// <param name="laneIndex">レーンインデックス</param>
    void Play(int32_t laneIndex);
};