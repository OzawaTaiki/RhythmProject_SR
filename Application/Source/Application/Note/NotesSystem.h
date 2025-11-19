#pragma once

// Engine
#include <System/Time/Stopwatch.h>

// Application
#include <Application/Lane/Lane.h>
#include <Application/Note/Note.h>
#include <Application/BeatMapLoader/BeatMapData.h>

#include <System/Audio/SoundInstance.h>
/// <summary>
/// ノーツシステムクラス。指定されたレーンに対してノーツの生成、更新、描画を行う。
/// </summary>
class NotesSystem
{
public:
    /// <summary>
    /// コンストラクタ。
    /// </summary>
    /// <param name="lane">操作対象のレーンポインタ</param>
    NotesSystem(Lane* lane);

    /// <summary>
    /// デストラクタ。
    /// </summary>
    ~NotesSystem();

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    /// <param name="noteSpeed">ノーツの速度</param>
    /// <param name="noteSize">ノーツの表示サイズ</param>
    /// <param name="startOffsetTime">再生開始のオフセット時間（秒）</param>
    void Initialize(float noteSpeed, float noteSize, float startOffsetTime = 2.0f);

    /// <summary>
    /// 毎フレームの更新処理を行う。
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間（秒）</param>
    void Update(float deltaTime);

    /// <summary>
    /// ノーツの描画を行う。
    /// </summary>
    /// <param name="camera">描画用カメラ</param>
    void DrawNotes(const Camera* camera);

    /// <summary>
    /// 譜面データを設定して内部的にノーツを生成する。
    /// </summary>
    /// <param name="beatMapData">譜面データ</param>
    void SetBeatMapDataAndCreateNotes(const BeatMapData& beatMapData);

    /// <summary>
    /// ノーツの移動速度を取得する。
    /// </summary>
    float GetNoteSpeed() const { return noteSpeed_; }

    /// <summary>
    /// ジャッジラインの位置を設定する。
    /// </summary>
    void SetJudgeLinePosition(float position) { judgeLinePosition_ = position; }

    /// <summary>
    /// miss判定の閾値を設定する。
    /// </summary>
    void SetMissJudgeThreshold(float threshold) { missJudgeThreshold_ = threshold; }

    /// <summary>
    /// 内部で使用するストップウォッチを設定する。
    /// </summary>
    void SetStopwatch(Stopwatch* stopwatch) { stopwatch_ = stopwatch; }

    /// <summary>
    /// ノーツデータを再読み込みする。
    /// </summary>
    void Reload();

    /// <summary>
    /// 再生中かどうかを設定する。
    /// </summary>
    void playing(bool playing) { playing_ = playing; }

    /// <summary>
    /// 自動プレイ設定を行う。
    /// </summary>
    void SetAutoPlay(bool autoPlay) { autoPlay_ = autoPlay; }

    /// <summary>
    /// 音楽のボイスインスタンスを設定する。
    /// </summary>
    void SetMusicVoiceInstance(std::shared_ptr<VoiceInstance> voiceInstance) { musicVoiceInstance_ = voiceInstance; }

    /// <summary>
    /// ノーツが再生成されたかを返す。
    /// </summary>
    bool IsReloaded();

    /// <summary>
    /// ノーツの処理を開始する。
    /// </summary>
    void Start() { isStarted_ = true; }

private:
    void CreateNormalNote(uint32_t laneIndex, float speed, float targetTime);
    void CreateLongNote(const NoteData& noteData);
    std::shared_ptr<Note> CreateNextNoteForLongNote(uint32_t laneIndex, float speed, float targetTime);
    void DebugWindow();

private:
    float noteSpeed_ = 0.0f;
    float noteSize_ = 0.0f;

    float judgeLinePosition_ = 0.0f;
    // miss判定で消えるまでの猶予標準
    float missJudgeThreshold_ = 3.0f;

    bool isStarted_ = false; // ゲーム開始オフセット時間を超えたかどうか
    float waitTimer_ = 0.0f; // ゲーム開始オフセット時間を超えるまでのタイマ
    float startOffsetTime_ = 0.0f; // ゲーム開始オフセット時間

    Lane* lane_ = nullptr;
    std::list<std::shared_ptr<Note>> notes_;

    bool autoPlay_ = false;

    BeatMapData beatMapData_;

    bool playing_ = true;

    Stopwatch* stopwatch_;

    bool isReloaded_ = false;

    // 再生している音楽データ
    std::shared_ptr<VoiceInstance> musicVoiceInstance_ = nullptr;
};
