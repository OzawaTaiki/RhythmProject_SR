#pragma once

#include <System/Input/Input.h>


#include <cstdint>
#include <vector>

struct MappingData
{
    int32_t laneIndex; // レーンインデックス
    float targetTime; // ターゲット時間

    MappingData(int32_t laneIndex = -1, float targetTime = -1.0f)
        : laneIndex(laneIndex), targetTime(targetTime) {
    }

};

class LiveMapping
{
public:

    LiveMapping() = default;
    ~LiveMapping() = default;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(int32_t laneCount);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="elapsedTime">曲の経過時間</param>
    void Update(float elapsedTime);

    /// <summary>
    /// マッピングデータを取得
    /// </summary>
    /// <returns>マッピングデータのリスト</returns>
    std::vector<MappingData>& GetMappingData() { return mappingData_; }

    /// <summary>
    /// マッピングデータをリセット
    /// </summary>
    void ResetMappingData();

    /// <summary>
    /// レーンのキー入力バインディングを設定
    /// </summary>
    /// <param name="laneIndex">レーン番号</param>
    /// <param name="keyCode">キーコード (DIK_Aなど)</param>
    void SetLaneKeyBinding(int32_t laneIndex, uint8_t keyCode);

private:

    /// <summary>
    /// デフォルトのキー入力バインディングを設定
    /// </summary>
    void SetDefaultKeyBindings(int32_t laneIndex);

private:
    Input* input_ = nullptr; // 入力管理クラスへのポインタ

    std::vector<uint8_t> laneKeyBindings_; // レーンごとのキー入力バインディング

    std::vector<MappingData> mappingData_; // マッピングデータのリスト

};