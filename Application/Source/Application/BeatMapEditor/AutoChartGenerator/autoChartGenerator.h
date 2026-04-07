#pragma once

#include <Application/BeatMapLoader/BeatMapData.h>

namespace Engine { class AudioSpectrum; }

namespace BME
{

/// <summary>
/// FFTスペクトル解析を用いて、譜面のたたき台をじどうせいせいするクラス
/// </summary>
class AutoChartGenerator
{
public:
    // 生成用パラメータ
    struct Settings
    {
        float sensitivity = 0.5f;// 0 ~ 1 (高いほどノーツ多め)
        float minNoteGap = 0.1f; // 同レーン内の最小間隔（秒）
        bool snapToGrid = true; // グリッドにスナップするか
        int gridDivision = 4; // グリッドの分割数（1/N）
        int windowN = 10; // FFTのウィンドウサイズ = 2^windowN

        bool useGpuFFT = true;
    };


    struct GenerateRequest
    {
        bool isRequested = false;
        Settings settings;
    };
public:
    AutoChartGenerator() = default;
    ~AutoChartGenerator() = default;

    /// <summary>
    /// 譜面を自動生成する
    /// </summary>
    /// <param name="spectrum">FFTスペクトル解析の結果</param>
    /// <param name="duration">曲の長さ（秒）</param>
    /// <param name="bpm">曲のBPM</param>
    /// <param name="offset">曲の開始オフセット（秒）</param>
    /// <param name="settings">生成の設定</param>
    /// <returns>生成されたノーツのリスト</returns>
    std::vector<NoteData> Generate(
        Engine::AudioSpectrum* spectrum,
        float duration,
        float bpm,
        float offset,
        const Settings& settings
    );


private:
    // 音量のピークを検出してノーツを生成するための構造体
    struct FluxSeries
    {
        std::vector<float> times; // ピークの時間
        std::vector<float> flux; // ピークの強さ
    };
private:

    /// <summary>
    /// FFTスペクトル解析の結果から、音量のピークを検出するためのフラックスを計算する
    /// </summary>
    /// <param name="spectrum">FFTスペクトル解析の結果</param>
    /// <param name="duration">曲の長さ（秒）</param>
    /// <param name="minHz">検出する周波数の下限</param>
    /// <param name="maxHz">検出する周波数の上限</param>
    /// <param name="hopSec">FFTのホップサイズ（秒）</param>
    /// <returns>フラックスの時系列データ</returns>
    FluxSeries ComputeFlux(Engine::AudioSpectrum* spectrum, float duration, float minHz, float maxHz, float hopSec);

    /// <summary>
    /// フラックスのピークを検出して、ノーツの時間を選び出す
    /// </summary>
    /// <param name="fluxSeries">フラックスの時系列データ</param>
    /// <param name="sensitivity">ピーク検出の感度（0 ~ 1）</param>
    /// <param name="minGap">同レーン内の最小間隔（秒）</param>
    /// <returns>ノーツの時間のリスト</returns>
    std::vector<float> PickOnsets(const FluxSeries& fluxSeries, float sensitivity, float minGap);

    void SmoothFlux(FluxSeries& series, int windowSize);

    /// <summary>
    /// ノーツの時間をグリッドにスナップする
    /// </summary>
    /// <param name="time">ノーツの時間（秒）</param>
    /// <param name="bpm">曲のBPM</param>
    /// <param name="offset">曲の開始オフセット（秒）</param>
    /// <param name="gridDivision">グリッドの分割数(1/N)</param>
    /// <returns>グリッドにスナップされたノーツの時間（秒）</returns>
    float SnapToGrid(float time, float bpm, float offset, int gridDivision);
};
}
