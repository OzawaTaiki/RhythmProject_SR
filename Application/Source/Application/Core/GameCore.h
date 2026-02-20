#pragma once

// Engine
#include <System/Audio/VoiceInstance.h>

// Application
#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/Core/ScoreCalc/ScoreCalculator.h>
#include <Application/GameMusic/GameMusic.h>
#include <Application/Input/InputData.h>
#include <Application/Lane/Lane.h>
#include <Application/Note/Judge/JudgeResult.h>
#include <Application/Note/Judge/NoteJudge.h>
#include <Application/Result/RankCalculator.h>


// STL
#include <functional>

// 前方宣言
namespace Engine { class Camera; }

// ゲームの核となる部分
class GameCore
{
public:
    // コンストラクタ
    GameCore(int32_t laneCount = 4);
    // デストラクタ
    ~GameCore();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(float noteSpeed, float musicLatencyMs = 0.0f,
                    float beginOffset = 2.0f);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    /// <param name="inputData">入力データ</param>
    void Update(float deltaTime, const std::vector<InputData>& inputData);

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="camera">カメラ</param>
    void Draw(const Engine::Camera* camera,
              const std::map<int32_t, uint8_t>& keyBindings);

    /// <summary>
    /// 譜面データを受け取り、ノーツを生成する
    /// </summary>
    /// <param name="beatMapData">譜面データ</param>
    void GenerateNotes(const BeatMapData& beatMapData);

    /// <summary>
    /// 音楽の音声インスタンスを設定する
    /// </summary>
    /// <param name="voiceInstance">音声インスタンスを</param>
    void SetMusicVoiceInstance(std::shared_ptr<Engine::VoiceInstance> voiceInstance)
    {
        musicVoiceInstance_ = voiceInstance;
    }

    /// <summary>
    /// GameMusicを設定する
    /// </summary>
    /// <param name="gameMusic">GameMusicのインスタンス</param>
    void SetGameMusic(const GameMusic* gameMusic) { gamemusic_ = gameMusic; }

    /// <summary>
    /// 開始
    /// </summary>
    void Start() { isWaitingForStart_ = false; }

    /// <summary>
    /// リスタート
    /// </summary>
    void Restart();

    /// <summary>
    /// 判定時のコールバック関数を設定する
    /// </summary>
    /// <param name="callback">コールバック関数</param>
    void SetJudgeCallback(
        const std::function<void(int32_t, JudgeType, int32_t)>& callback)
    {
        onJudgeCallback_ = callback;
    }

    /// <summary>
    /// ミス時のコールバック関数を設定する
    /// </summary>
    /// <param name="callback">コールバック関数</param>
    void SetMissCallback(const std::function<void(void)>& callback)
    {
        onMissCallback_ = callback;
    }

    /// <summary>
    /// ホールド時のコールバック関数を設定する
    /// </summary>
    /// <param name="callback">コールバック関数</param>
    void SetHoldCallback(const std::function<void(int32_t)>& callback)
    {
        onHoldCallback_ = callback;
    }

    /// <summary>
    /// ノーツの移動速度を設定する
    /// </summary>
    /// <param name="speed">ノーツの移動速度</param>
    void SetNoteSpeed(float speed)
    {
        noteSpeed_ = speed;
        noteJudge_->SetSpeed(noteSpeed_);
    }

    /// <summary>
    /// 音楽の遅延を設定する
    /// </summary>
    /// <param name="latencyMs">遅延時間（ミリ秒）</param>
    void SetMusicLatency(float latencyMs) { musicLatencyMs_ = latencyMs; }

    /// <summary>
    /// 最大コンボ数を取得する
    /// </summary>
    /// <returns>最大コンボ数</returns>
    int32_t GetMaxCombo() const { return maxCombo_; }

    /// <summary>
    /// 現在のコンボ数を取得する
    /// </summary>
    /// <returns>現在のコンボ数</returns>
    int32_t GetCombo() const { return combo_; }

    /// <summary>
    /// レーンの数を取得する
    /// </summary>
    /// <returns></returns>
    int32_t GetLaneCount() const { return static_cast<int32_t>(lanes_.size()); }

    /// <summary>
    /// 判定結果を取得する
    /// </summary>
    /// <returns>判定結果のマップ</returns>
    std::map<JudgeType, int32_t> GetJudgeResult() const
    {
        return judgeResult_->GetJudgeResult();
    }

    /// <summary>
    /// スコアを取得する
    /// </summary>
    /// <returns> 計算済みのスコア</returns>
    int32_t GetScore() const { return scoreCalculator_.GetDisplayScore(); }

    Rank GetRank() const;

private:
    // 譜面データを解析する
    void ParseBeatMapData(const BeatMapData& beatMapData);
    // ノーツを作成する
    void CreateBeatMapNotes();

    // ノーツの判定
    void JudgeNotes(const std::vector<InputData>& inputData);
    void JudgeNotesInAutoMode(float elapsedTime);
    // 通常ノーツの判定
    JudgeType ProcessNormalNote(Note* note, const InputData& inputData);
    // ホールドノーツの判定
    JudgeType ProcessHoldNote(Note* note, const InputData& inputData, Lane* lane);
    // ホールドエンドノーツの判定
    JudgeType ProcessHoldEndNote(Note* note, const InputData& inputData,
                                 Lane* lane);
    // コンボの更新
    void UpdateCombo(JudgeType result);
    // 判定結果の記録
    void RecordJudgeResult(JudgeType result, Note* note);

private:
    // lane
    float noteSpeed_ = 30.0f;                  // ノーツの移動速度
    int32_t laneCount_ = 4;                    // レーンの数
    std::vector<std::unique_ptr<Lane>> lanes_; // レーンのリスト
    std::vector<std::list<NoteData>> notesPerLane_; // レーンごとのノートデータリスト

    // judge
    std::unique_ptr<NoteJudge> noteJudge_;     // ノーツの判定を行うクラス
    std::unique_ptr<JudgeResult> judgeResult_; // 判定結果を保持するクラス

    // コンボ
    int32_t combo_ = 0;    // 現在のコンボ数
    int32_t maxCombo_ = 0; // 最大コンボ数

    // スコア計算
    ScoreCalculator scoreCalculator_; // スコア計算クラス

    //-------------------------
    // コールバック関連

    // 判定時のコールバック関数
    std::function<void(int32_t, JudgeType, int32_t)> onJudgeCallback_;
    // ミス時のコールバック関数
    std::function<void(void)> onMissCallback_;
    // ホールド時のコールバック関数
    std::function<void(int32_t)> onHoldCallback_;

    float noteDeletePosition_ = -10.0f; // ノーツを削除する位置

    /// 開始前オフセット関連
    float beginOffset_ = 2.0f;      // ゲーム開始オフセット時間
    float waitTimer_ = 0.0f;        // 開始前オフセット待機タイマー
    bool isWaitingForStart_ = true; // 開始前オフセット待機中かどうか

    float musicLatencyMs_ = 0.0f; // 音楽の遅延

    // ホールド状態
    // HoldState holdState_ = {};

    std::weak_ptr<Engine::VoiceInstance>
        musicVoiceInstance_; // 音楽の音声インスタンス 弱参照

    const GameMusic* gamemusic_ = nullptr; // 音楽の管理
};