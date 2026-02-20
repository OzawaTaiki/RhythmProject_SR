#pragma once

#include <System/Input/Input.h>
#include <System/Time/Stopwatch.h>
#include <System/Time/Time_MT.h>

#include <deque>

#include <vector>

/// <summary>
/// タップ入力からBPMを計測するためのユーティリティクラス。
/// </summary>
/// <remarks>
/// TapBPMCounterはユーザーのタップ（ここではスペースキー）を検出し、
/// タップ間の時間間隔を記録して平均インターバルからBPMを算出します。
/// 内部でストップウォッチを用いて各タップの経過時間を取得し、
/// 最新のN回分（デフォルトでは10回）を保持して移動平均を取ります。
/// </remarks>
/// <example>
/// TapBPMCounter counter;
/// counter.Initialize();
/// // 毎フレーム
/// counter.Update();
/// float bpm = counter.GetBPM();
/// </example>
class TapBPMCounter
{
public:
    TapBPMCounter() = default;
    ~TapBPMCounter() = default;

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    /// <remarks>
    /// 入力管理クラスのポインタを取得し、ストップウォッチと内部記録を初期化します。
    /// ゲーム開始時や計測を始める前に一度呼び出してください。
    /// </remarks>
    void Initialize();

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    /// <remarks>
    /// スペースキーのトリガーを検出すると、その時点の経過時間を記録して
    /// ストップウォッチをリセットします。内部で保存する最大サンプル数は
    /// 10件で、古い値は破棄されます。
    /// </remarks>
    void Update();

    /// <summary>
    /// 計測データをリセットする。
    /// </summary>
    /// <remarks>
    /// ストップウォッチと記録されているタップ時間をクリアし、合計時間を0に戻します。
    /// </remarks>
    void Reset();

    /// <summary>
    /// 現在のBPMを取得する。
    /// </summary>
    /// <returns>計算されたBPM（小数）。サンプル数が2未満、または平均時間が0以下の場合は0.0を返します。</returns>
    float GetBPM() const;

private:

    Engine::Input* input_ = nullptr; // 入力管理クラスへのポインタ

    Engine::Stopwatch stopwatch_; // タップの時間を計測するストップウォッチ

    std::deque<float> tapTimes_= {}; // タップした時間を記録するベクター
    float totalTime_ = 0.0f; // タップの合計時間

};