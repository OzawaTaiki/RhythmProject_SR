#pragma once

// Engine
#include <Math/Vector/Vector2.h>
#include <Features/UI/Element/UIImageElement.h>
#include <Features/UI/Element/UIButtonElement.h>
#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/Json/JsonBinder.h>
#include <Features/TextRenderer/TextGenerator.h>

// Application
#include <Application/Result/ResultData.h>

// STL
#include <cstdint>

// リザルト画面のUIクラス
/// <summary>
/// リザルト画面の UI を担当するクラス。スコア表示やボタン等の描画・更新を行う。
/// </summary>
class ResultUI
{
public:
    ResultUI() = default;
    ~ResultUI() = default;

    // 初期化
    void Initialize(ResultData resultData);
    // 更新
    void Update(float deltaTime);
    // 描画
    void Draw();


private:

    // UIの種類
    enum class UIs
    {
        MainBackground, // メイン背景

        ScoreBackground, // スコア背景

        ToTitleButton, // タイトルへ戻るボタン
        ReplayButton, // リプレイボタン

    };
    // テキストの種類
    enum class TextType
    {
        Title, // タイトル

        Score_text, // スコア
        Score_value, // スコアの値

        Judge_perfect_text, // パーフェクトのテキスト
        Judge_perfect_value, // パーフェクトの値

        Judge_good_text, // グッドのテキスト
        Judge_good_value, // グッドの値

        Judge_bad_text, // バッドのテキスト
        Judge_bad_value, // バッドの値

        Judge_miss_text, // ミスのテキスト
        Judge_miss_value, // ミスの値

        Combo_text, // コンボのテキスト
        Combo_value, // コンボの値

        Count // テキストの数
    };

private:

    // UIグループの初期化
    void InitUIGroup();
    // テキストパラメータの初期化
    void InitTextParams();

    void UpdateTextParams(float deltaTime);
    void UpdateUIs(float deltaTime);

    // テキストタイプからキー文字列を取得
    std::string GetKeyString(TextType textType) const;
    // テキストタイプからラベルを取得
    std::wstring GetTextLabel(TextType textType) const;
    // テキストタイプからジャッジタイプを取得
    JudgeType GetJudgeTypeFromTextType(TextType textType) const;
    // ジャッジタイプからテキストタイプを取得
    TextType GetTextTypeFromJudgeType(JudgeType judgeType) const;

    enum class EventType
    {
        ToTitle,
        Retry,
    };
    void DispatchEvent(EventType eventType);
private:

    std::string musicTitle_ = ""; // 楽曲のタイトル
    // アニメーション用の値
    struct AnimationValue
    {
        Vector2 position = { 0, 0 }; // 座標
        Vector2 movement = { 0, 0 }; // 座標
        Vector2 scale = { 1, 1 }; // スケール
        float alpha = 1.0f; // アルファ値

        float timer = 0.0f;
        float delay = 0.0f; // アニメーションの遅延時間
    };

    struct CounterValue
    {
        int32_t value = 0; // データ保持用
        int32_t currentValue = 0; // 現在の表示値
        float animationTimer = 0.0f; // アニメーションタイマー
    };

    float animationDuration_ = 0.25f;
    std::unique_ptr<AnimationSequence> animationSequence_ = nullptr; // アニメーションシーケンス

    std::unique_ptr<UIElement> UIElement_ = nullptr; // UIグループ
    std::vector<UIElement*> buttons_;
    std::unique_ptr<AnimationSequence> animForUI_ = nullptr; //UI用


    // textParamの拡張
    struct ExtendedTextParam
    {
        std::wstring label; // ラベル
        TextParam textParam; // テキストパラメータ
        AnimationValue animationValue = {}; // アニメーション用の値
        std::optional<CounterValue> counterValue = std::nullopt; // カウンター用の値（オプション）
    };

    std::map<TextType, ExtendedTextParam> textParams_; // テキストパラメータのマップ

    std::unique_ptr<JsonBinder> jsonBinder_ = nullptr; // JSONバインダー

    bool transitionToTitle_ = false; // タイトルへ遷移するかどうか
    bool replay_ = false; // リプレイするかどうか

    TextGenerator text_; // テキストジェネレータ

    bool isDraw_ = false;
};