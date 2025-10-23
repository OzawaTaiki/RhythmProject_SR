#pragma once

//#include <Features/UI/UISprite.h>
#include <Features/TextRenderer/TextGenerator.h>
#include <Features/Json/JsonBinder.h>

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
    /// <param name="_combo">現在のコンボ数</param>
    void Update(int32_t _combo);

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

    int32_t comboValue_ = 0; // コンボ値

    std::unique_ptr<JsonBinder> jsonBinder_ = nullptr; // JSONバインダー
};