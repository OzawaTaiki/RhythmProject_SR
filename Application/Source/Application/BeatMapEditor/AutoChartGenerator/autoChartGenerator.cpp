#include "autoChartGenerator.h"

#include <Features/AudioSpectrum/AudioSpectrum.h>

#include <Debug/Debug.h>

#include <algorithm>
#include <numeric>
#include <chrono>

std::vector<NoteData> BME::AutoChartGenerator::Generate(Engine::AudioSpectrum* spectrum, float duration, float bpm, float offset, const Settings& settings)
{
    struct BandDef
    {
        int32_t laneIndex;
        float minHz;
        float maxHz;
    };

    // TODO : 周波数帯域の定義を外部から受け取れるようにする
    BandDef bands[] = {
        { 0,    20,   150 }, // kick
        { 1,   150,   500 }, // bass
        { 2,   500,  2000 }, // mid
        { 3,  2000, 20000 }  // hi-hat
    };
    //BandDef bands[] = {
    //    { 0,    20,   3000},
    //};

    Engine::Debug::Log("Begin auto-chart generation\n");
    Engine::Debug::Log("dutaion: " + std::to_string(duration) + " sec\n");
    Engine::Debug::Log(std::format("FFT Window Size:2^{}({})\n ", settings.windowN, 1 << settings.windowN));
    auto beginTime = std::chrono::high_resolution_clock::now();

    std::vector<NoteData> result;
    int32_t lane;
    lane = 0;// 無参照 回避用
    for (auto& band : bands)
    {
        // 帯域毎のフラックスを計算 TODO：hopSec
        FluxSeries flux = ComputeFlux(spectrum, duration, band.minHz, band.maxHz, 0.01f);
        SmoothFlux(flux, 5); // 窓幅5フレーム（±2フレーム）

        // フラックスのピークから音セット時刻を検出
        std::vector<float> onsets = PickOnsets(flux, settings.sensitivity, settings.minNoteGap);

        float windowOffset = (1 << settings.windowN) / (2.0f * spectrum->GetSampleRate());
        for (float& t : onsets)
            t -= windowOffset;
        for (float time : onsets)
        {
            // 音セット時刻を最近傍のグリッドにスナップ
            if (settings.snapToGrid)
            {
                time = SnapToGrid(time, bpm, offset, settings.gridDivision);
            }
            result.push_back({
                band.laneIndex , // レーンインデックス
                time,           // タイミング
                "normal", 0.0f  // 自動生成はタイミングのみで、ノーツの種類やホールド時間は固定
                             });
            //result.push_back({
            //    lane++ % 4 , // レーンインデックス
            //    time,           // タイミング
            //    "normal", 0.0f  // 自動生成はタイミングのみで、ノーツの種類やホールド時間は固定
            //                 });
        }
    }

    // 時間で昇順ソート
    std::sort(result.begin(), result.end(), [](const NoteData& a, const NoteData& b)
              {
                  return a.targetTime < b.targetTime;
              });

    auto endTime = std::chrono::high_resolution_clock::now();
    Engine::Debug::Log(std::format("Auto-chart generation complete: {} notes generated in {} ms\n",
                                   result.size(),
                                   std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count()));

    return result;
}

BME::AutoChartGenerator::FluxSeries BME::AutoChartGenerator::ComputeFlux(Engine::AudioSpectrum* spectrum, float duration, float minHz, float maxHz, float hopSec)
{
    FluxSeries series;
    std::vector<float> prev;

    for (float t = 0.0f; t < duration; t += hopSec)
    {
        // 時刻tのスペクトルを取得
        spectrum->GetSpectrumAtTime(t);
        // 帯域内の振幅リストを取得
        std::vector<float> curr = spectrum->GetAmplitudesInRange(minHz, maxHz);

        // 初回は比較対象がないのでスキップ
        if (prev.empty())
        {
            prev = curr;
            continue;
        }

        // 前フレームとの差分を合計してフラックスを計算
        float fluxVal = 0.0f;
        for (size_t i = 0; i < curr.size(); ++i)
        {
            float diff = curr[i] - prev[i];
            if (diff > 0) // 増加分のみをカウント
                fluxVal += diff;
        }

        series.times.push_back(t);
        series.flux.push_back(fluxVal);

        prev = curr;
    }

    return series;
}

std::vector<float> BME::AutoChartGenerator::PickOnsets(const FluxSeries& fluxSeries, float sensitivity, float minGap)
{
    // 閾値計算のために flux 全体の統計量を求める
    std::vector<float> sorted = fluxSeries.flux;
    std::sort(sorted.begin(), sorted.end());

    /// 閾値 = 中央値 + (中央値 - 平均) * sensitivity
    //// 中央値
    //float median = sorted[sorted.size() / 2];
    //// stddev計算のための平均
    //float mean = std::accumulate(sorted.begin(), sorted.end(), 0.0f) / sorted.size();
    //// fluxのばらつき
    //float stddev = std::sqrt(std::accumulate(sorted.begin(), sorted.end(), 0.0f, [mean](float acc, float val)
    //                                         {
    //                                             return acc + (val - mean) * (val - mean);
    //                                         }) / sorted.size());

    float k = 2.0f - sensitivity * 1.5f; // sensitivityが0のときk=2、1のときk=0.5
    //float threshold = median + k * stddev;
    float globalMin = sorted[sorted.size() / 2]; // グローバル中央値の50%


    std::vector<float> onsets;
    float lastTime = -9999.0f; // 最後に追加したノーツの時間（初期値は十分に小さい値）
    const size_t windowSize = 50;
    for (size_t i = 1; i < fluxSeries.flux.size() - 1; ++i)
    {
        size_t begin = (i >= windowSize) ? i - windowSize : 0;
        size_t end = std::min(i + windowSize, fluxSeries.flux.size() - 1);

        std::vector<float> localFlux(fluxSeries.flux.begin() + begin, fluxSeries.flux.begin() + end);
        std::sort(localFlux.begin(), localFlux.end());
        float localMidian = localFlux[localFlux.size() / 2];
        float localMean = std::accumulate(localFlux.begin(), localFlux.end(), 0.0f) / localFlux.size();
        float localStddev = std::sqrt(std::accumulate(localFlux.begin(), localFlux.end(), 0.0f, [localMean](float acc, float val)
                                                      {
                                                          return acc + (val - localMean) * (val - localMean);
                                                      }) / localFlux.size());

        // ソート済みなので localFlux.back() がローカル最大値
        float localPeak  = localFlux.back();

        // グローバルの75パーセンタイルを「意味のある音」の基準にする
        float globalFloor = sorted[sorted.size() * 3 / 4];

        // ローカル最大値がその基準を下回る区間 = 無音とみなしてスキップ
        if (localPeak < globalFloor)
            continue;

        float localThreshold = localMidian + k * localStddev;
        float threshold = std::max(localThreshold, globalMin);

        bool isLocalMax = fluxSeries.flux[i] > fluxSeries.flux[i - 1]
            && fluxSeries.flux[i] > fluxSeries.flux[i + 1];
        bool overThreshold = fluxSeries.flux[i] > threshold;

        if (isLocalMax && overThreshold)
        {
            float time = fluxSeries.times[i];
            if (time - lastTime >= minGap)
            {
                onsets.push_back(time);
                lastTime = time;
            }
        }
    }



    return onsets;
}

void BME::AutoChartGenerator::SmoothFlux(FluxSeries& series, int windowSize)
{
    int half = windowSize / 2;
    std::vector<float> smoothed(series.flux.size());

    for (size_t i = 0; i < series.flux.size(); ++i)
    {
        float sum = 0.0f;
        int count = 0;

        // 前後 half フレームの平均を取る
        for (int j = -half; j <= half; ++j)
        {
            int idx = static_cast<int>(i) + j;
            if (idx >= 0 && idx < static_cast<int>(series.flux.size()))
            {
                sum += series.flux[idx];
                ++count;
            }
        }
        smoothed[i] = sum / count;
    }

    series.flux = smoothed;
}


float BME::AutoChartGenerator::SnapToGrid(float time, float bpm, float offset, int gridDivision)
{
    float gridInterval = 60.0f / bpm / static_cast<float>(gridDivision); // グリッドの間隔（秒）

    float elapsed = time - offset; // オフセットを考慮した経過時間

    float snapped = std::round(elapsed / gridInterval) * gridInterval + offset; // 最近傍のグリッドにスナップ

    return snapped;
}
