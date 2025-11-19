#pragma once

#include <Math/Vector/Vector3.h>
#include <Math/Vector/Vector4.h>
#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/TextRenderer/TextGenerator.h>

#include <Application/Note/Judge/JudgeType.h>

#include <string>

class Camera; // 前方宣言

// 判定テキスト表示クラス
/// <summary>
/// 判定時に表示するテキスト（例：Perfect、Great）を管理するクラス。
/// </summary>
class JudgeText
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    JudgeText();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~JudgeText() = default;

    /// <summary>
    /// 初期化処理。
    /// </summary>
    /// <param name="judgeType">判定タイプ</param>
    /// <param name="laneIndex">レーンインデックス</param>
    void Initialize(JudgeType judgeType, int32_t laneIndex, const Camera* camera);

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    /// <param name="deltaTime">経過時間（秒）</param>
    void Update(float deltaTime);

    /// <summary>
    /// 描画処理。
    /// </summary>
    void Draw();


    /// <summary>
    /// 描画が完了したかどうかを判定する。
    /// </summary>
    /// <returns>true: 表示完了、false: 表示中</returns>
    bool IsFinished() const { return timer_ >= displayDuration_; }

private:

    /// <summary>
    /// テキストのアニメーション処理（内部）
    /// </summary>
    void AnimateText();

private:

    /// <summary>
    /// 判定タイプに対応するテキストを返す（内部ユーティリティ）。
    /// </summary>
    /// <param name="judgeType">判定タイプ</param>
    /// <returns>判定用のテキスト</returns>
    static std::wstring GetJudgeText(JudgeType judgeType);

    /// <summary>
    /// 判定タイプに応じたテキストの色を決定する（内部ユーティリティ）。
    /// </summary>
    /// <param name="judgeType">判定タイプ</param>
    /// <param name="topColor">上端の色（出力）</param>
    /// <param name="bottomColor">下端の色（出力）</param>
    static void GetJudgeTextColor(JudgeType judgeType, Vector4& topColor, Vector4& bottomColor);

    // テキストパラメータの更新（内部）
    void UpdateTextParam();

    static float displayYOffset_; // Y軸のオフセット
private:

    TextGenerator text_;

    JudgeType judgeType_; // 判定タイプ

    TextParam textParam_; // テキストパラメータ

    Vector4 topColor_       = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 上端点の色
    Vector4 bottomColor_    = Vector4(0.5f, 0.5f, 0.5f, 1.0f); // 下端点の色

    Vector2 scale_      = { 1.0f, 1.0f };   // テキストのスケール
    Vector2 movement_   = { 0.0f, 0.0f };   // テキストの移動量
    float   alpha_      = 1.0f;             // テキストのアルファ値

    std::wstring judgeText_; // 判定テキスト

    Vector2 position_       = {};   // テキストの表示位置
    float timer_            = 0.0f; // 表示時間のタイマー
    float displayDuration_  = 1.0f; // 表示時間

    std::unique_ptr<AnimationSequence> animationSequence_; // アニメーション制御クラス
};