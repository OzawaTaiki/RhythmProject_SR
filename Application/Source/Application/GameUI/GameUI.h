#pragma once

//#include <Features/UI/UISprite.h>
#include <Features/TextRenderer/TextGenerator.h>
#include <Features/Json/JsonBinder.h>
#include <Features/Animation/Sequence/AnimationSequence.h>

/// <summary>
/// ゲームUIクラス。
/// </summary>
class GameUI
{
public:
    GameUI() = default;
    ~GameUI() = default;

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    /// <param name="combo">現在のコンボ数</param>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(int32_t combo, float deltaTime);

    /// <summary>
    /// 描画処理を行う。
    /// </summary>
    void Draw();

private:
    /// <summary>
    /// デバッグ用の ImGui 表示。
    /// </summary>
    void ImGui();

private:
    TextGenerator textGenerator_; // テキスト生成用のインスタンス

    TextParam comboValueParam_; // コンボ値のテキストパラメータ
    TextParam comboTextParam_; // コンボのテキストパラメータ

    std::unique_ptr<AnimationSequence> comboValueAnimation_ = nullptr; // コンボ値のアニメーションシーケンス

    int32_t comboValue_ = 0; // コンボ値

    std::unique_ptr<JsonBinder> jsonBinder_ = nullptr; // JSONバインダー
};