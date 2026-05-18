#pragma once


#include <Application/FeedBack/JudgeEffect/JudgeEffect.h>
#include <Application/FeedBack/JudgeText/JudgeText.h>
#include <Application/FeedBack/MissedVignette/MissedVignette.h>
#include <Application/FeedBack/LaneEffect/LaneEffect.h>
#include <Application/FeedBack/TapEffect/TapEffect.h>
#include <Application/GameEnvironment/BackgroundEffect.h>
#include <Application/FeedBack/HoldingEffect/NoteHoldEffect.h>
#include <APplication/FeedBack/SpeakerSeekEffect.h>
#include <Application/FeedBack/LaneEdgePillar/LaneEdgeEffect.h>
#include <Application/FeedBack/ComboThresholds.h>

#include <Application/Input/InputData.h>

#include <memory>
#include <cstdint>
#include <vector>
#include <list>
#include <bitset>

// 前方宣言
namespace Engine { class Camera; }
class GameBackground;

/// <summary>
/// フィードバック（判定エフェクト、音、ビネットなど）を管理するクラス。
/// </summary>
class FeedbackEffect
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    FeedbackEffect() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~FeedbackEffect() = default;

    /// <summary>
    /// 初期化を行う。
    /// </summary>
    /// <param name="camera">座標変換や表示に用いるカメラ</param>
    /// <param name="laneCount">レーン数</param>
    /// <param name="gameBackground">ゲーム背景オブジェクト</param>
    void Initialize(Engine::Camera* camera, int32_t laneCount, GameBackground* gameBackground);

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間（秒）</param>
    /// <param name="inputData">入力データの配列</param>
    void Update(float deltaTime, const std::vector<InputData>& inputData);

    /// <summary>
    /// 描画処理。
    /// </summary>
    void Draw();

    /// <summary>
    /// ジャッジエフェクトを再生する（コールバック向け）。
    /// </summary>
    /// <param name="laneIndex">対象レーンのインデックス</param>
    /// <param name="judgeType">判定種類</param>
    void PlayJudgeEffect(int32_t laneIndex, JudgeType judgeType,int32_t combo);

    /// <summary>
    /// ミス時のエフェクトを再生する。
    /// </summary>
    void PlayMissedEffect();

    void PlaySpeakerSeekEffect(int32_t laneIndex, JudgeType judgeType);

    /// <summary>
    /// ホールドエフェクトを再生する。
    /// </summary>
    /// <param name="laneIndex">対象レーンのインデックス</param>
    void PlayHoldEffect(int32_t laneIndex);

    /// <summary>
    /// ミス時のビネットエフェクトを適用する（レンダーターゲット指定）。
    /// </summary>
    /// <param name="input">入力レンダーターゲット名</param>
    /// <param name="output">出力レンダーターゲット名</param>
    void ApplyMissedVignetteEffect(const std::string& input, const std::string& output);

    const ComboThresholds* GetComboThresholds() { return &comboThresholds_; }

    /// <summary>
    /// コンボ閾値を初期化する。
    /// </summary>
    /// <param name="maxCombo">最大コンボ数</param>
    void InitComboThresholds(int32_t maxCombo);// 非同期ロードを考慮して後から初期化する


    /// <summary>
    /// デバッグ用ウィンドウを表示する。
    /// </summary>
    void DebugWindoow();

private:

    /// <summary>
    /// 判定テキストを割り当てる内部ユーティリティ。
    /// </summary>
    /// <param name="judgeType">判定タイプ</param>
    /// <param name="laneIndex">レーンインデックス</param>
    void AllocateJudgeText(JudgeType judgeType, int32_t laneIndex);


private:

    /// <summary>
    /// 判定パーティクル類
    /// </summary>
    std::unique_ptr<JudgeEffect> judgeEffect_;
    std::unique_ptr<TapEffect> tapEffect_; // タップエフェクト

    std::unique_ptr<BackgroundEffect> backgroundEffect_; // 背景エフェクト
    std::unique_ptr<NoteHoldEffect> noteHoldEffect_; // ホールドエフェクト

    /// <summary>
    /// 判定テキスト用のプール
    /// </summary>
    static const int32_t kMaxJudgeTexts_ = 10; // 最大の判定テキスト数
    std::array<std::unique_ptr<JudgeText>, kMaxJudgeTexts_> judgeTextPool_;
    std::bitset<kMaxJudgeTexts_> usedJudgeTexts_; // 使用中のテキストを管理するビットセット

    const int32_t kMaxSeekEffects_ = 16; // 最大のスピーカーシークエフェクト数
    std::unique_ptr<SpeakerSeekEffect> speakerSeekEffect_; // スピーカーに向かうエフェクト

    /// <summary>
    /// ミス時に表示するビネットエフェクト
    /// </summary>
    std::unique_ptr<MissedVignette> missedVignette_; // ミス時のビネットエフェクト

    /// <summary>
    /// 各レーンの個別エフェクト
    /// </summary>
    std::vector<std::unique_ptr<LaneEffect>> laneEffects_; // レーンごとのエフェクト

    std::unique_ptr<LaneEdgeEffects> laneEdgeEffects_; // レーンエッジエフェクト

    ComboThresholds comboThresholds_; // コンボ閾値管理クラス
    int32_t prevComboLevel_ = 0; // 前回のコンボレベル

    // 座標変換用カメラ
    Engine::Camera* camera_ = nullptr; // カメラへのポインタ
    GameBackground* gameBackground_ = nullptr; // ゲーム環境へのポインタ
};