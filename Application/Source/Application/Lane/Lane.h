#pragma once

#include <Application/Note/Note.h>
#include <Application/BeatMapLoader/BeatMapData.h>

#include <memory>
#include <list>

class Camera;

// 単一のレーン
class Lane
{
public:

    Lane() = default;
    ~Lane();

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="noteDataList">ノーツのデータ</param>
    /// <param name="laneIndex">レーンインデックス</param>
    /// <param name="judgeLine">判定ラインの座標</param>
    void Initialize(const std::list<NoteData>& noteDataList, int32_t laneIndex, float judgeLine, float  speed, float startOffsetTime);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="elapseTime">経過時間</param>
    /// <param name="speed">ノーツの移動速度</param>
    void Update(float elapseTime, float speed);

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="camera">カメラ</param>
    /// <param name="laneColor">レーンの色</param>
    void Draw(const Camera* camera, const Vector4& laneColor = Vector4(0.0f, 0.0f, 0.0f, 0.8f)) const;


    /// <summary>
    /// 一番手前のノーツを取得
    /// </summary>
    /// <returns>一番手前のノーツへのポインタ</returns>
    Note* GetFirstNote() const;

    /// <summary>
    /// 画面外のノーツを削除する
    /// </summary>
    /// <param name="noteDeletePos">ノーツを削除する位置</param>
    /// /// <returns>削除したノーツの数</returns>
    int32_t DeleteNotesOutOfScreen(float noteDeletePos);

    /// <summary>
    /// ホールド開始
    /// </summary>
    void StartHold();

    /// <summary>
    /// ホールド終了
    /// </summary>
    void EndHold();

    bool IsHolding() const { return isHolding_; }


public: // 静的メンバ関数

    static void SetTotalWidth(float width) { totalWidth_ = width; }
    static float GetTotalWidth() { return totalWidth_; }

    static void SetLaneWidth(float width) { laneWidth_ = width; }
    static float GetLaneWidth() { return laneWidth_; }

    static void SetLaneLength(float length) { laneLength_ = length; }
    static float GetLaneLength() { return laneLength_; }

    static void SetLaneCount(int32_t count) { laneCount_ = count; }
    static int32_t GetLaneCount() { return laneCount_; }

    static Vector3 GetLaneEndPosition(int32_t laneIndex, float judgeLine = 0.0f);

private: // 内部処理用関数たち

    /// <summary>
    /// ノーツを生成する
    /// </summary>
    void CreateNotes(const std::list<NoteData>& noteDataList, int32_t laneIndex, float judgeLine, float  speed, float startOffsetTime);

    void CreateLaneModel();
private:

    std::unique_ptr<ObjectModel> laneModel_ = nullptr; // レーンのモデル

    std::list<std::shared_ptr<Note>> notes_; // レーンにあるノーツ


    Vector3 startPosition_; // レーンの開始位置 手前
    Vector3 endPosition_; // レーンの終了位置 奥

    bool isHolding_ = false; // ホールド中かどうか
private:
    static float laneLength_; // レーンの長さ
    static float totalWidth_; // レーンの合計幅
    static int32_t laneCount_; // レーンの数
    static float laneWidth_; // レーンの幅
};