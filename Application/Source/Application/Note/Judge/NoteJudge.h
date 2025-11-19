#pragma once

// Engine
#include <Features/Json/JsonBinder.h>

// Application
#include <Application/Note/Judge/JudgeType.h>

// STL
#include <memory>

class Note;
// ノート判定クラス
class NoteJudge
{
public:
    NoteJudge();
    ~NoteJudge();

    // 初期化
    void Initialize();
    // 判定ラインの描画
    void DrawJudgeLine();

    /// <summary>
    /// ノーツの判定を行う
    /// </summary>
    /// <param name="note">判定するノート</param>
    /// <param name="elapsedTime">楽曲の経過時間</param>
    JudgeType ProcessNoteJudge(Note* note, float elapsedTime);

    // ノーツの速度を設定
    void SetSpeed(float speed) { speed_ = speed; }

    // 各判定の閾値を取得
    float GetGoodJudgeThreshold() { return timingThresholds_[JudgeType::Good]; }
    float GetMissJudgeThreshold() { return timingThresholds_[JudgeType::Miss]; }

    // === デバッグ用 === //
    // 判定ラインを描画するか
    void SetIsDrawLine(bool isDraw) { isDrawLine = isDraw; }
    // ノーツの位置を設定
    void SetPosition(float position) { position_ = position; }
    // レーンの総幅を設定
    void SetLaneTotalWidth(float width) { laneTotalWidth_ = width; }

private:
    // JSONバインダーの初期化
    void InitializeJsonBinder();

private:

    std::unique_ptr<JsonBinder> jsonBinder_; // JSONバインダー

    // 判定 閾値
    std::map<JudgeType, float> timingThresholds_;

    // ノーツの速度
    float speed_ = 0.0f;

    /// debug用
    // 判定ラインの位置
    float position_ = 0.0f;
    // レーンの総幅
    float laneTotalWidth_ = 0.0f;
    // 判定ラインを描画するか
    bool isDrawLine = true;

};
