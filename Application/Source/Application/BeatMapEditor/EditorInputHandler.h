#pragma once

#include <Application/BeatMapEditor/EditorState.h>
#include <Application/BeatMapEditor/EditorCoordinate.h>
#include <cstdint>

class Input;
class CommandHistory;

namespace BME
{

class Document;
class AudioController;
class Document;

/// <summary>
/// エディターの入力処理クラス
/// </summary>
class InputHandler
{
public:
    InputHandler();
    ~InputHandler() = default;

    /// <summary>
    /// 入力処理（依存クラスをすべて引数で受け取る）
    /// </summary>
    void HandleInput(
        State* state,
        Document* document,
        AudioController* audioController,
        CommandHistory* commandHistory,
        EditorCoordinate* coordinate,
        float& currentTime
    );

private:
    // グローバル入力（全モード共通）
    void HandleGlobalInput(
        State* state,
        Document* document,
        AudioController* audioController,
        CommandHistory* commandHistory,
        float& currentTime
    );

    // モード別入力
    void HandleModeSpecificInput(
        State* state,
        Document* document,
        AudioController* audioController,
        CommandHistory* commandHistory,
        EditorCoordinate* coordinate
    );

    // 選択モード
    void HandleSelectMode(
        State* state,
        Document* document,
        CommandHistory* commandHistory,
        EditorCoordinate* coordinate
    );

    // 通常ノート配置モード
    void HandlePlaceNormalNoteMode(
        State* state,
        Document* document,
        CommandHistory* commandHistory,
        EditorCoordinate* coordinate
    );

    // ロングノート配置モード
    void HandlePlaceLongNoteMode(
        State* state,
        Document* document,
        CommandHistory* commandHistory,
        EditorCoordinate* coordinate
    );

    // 削除モード
    void HandleDeleteMode(
        Document* document,
        CommandHistory* commandHistory,
        EditorCoordinate* coordinate
    );

    // ライブマッピングモード
    void HandleLiveMappingMode(
        State* state,
        Document* document,
        CommandHistory* commandHistory,
        EditorCoordinate* coordinate,
        float currentTime
    );
    // BPM設定モード
    void HandleBPMSettingMode(
        State* state,
        AudioController* audioController
    );

    // マウスホイール入力処理
    void HandleMouseWheelInput(
        EditorCoordinate* coordinate,
        AudioController* audioController,
        float& currentTime
    );

    // ヘルパー関数
    /// <summary>
    /// 時間をグリッドにスナップ
    /// </summary>
    float SnapTimeToGrid(float time, const State* state, const BeatMapData& data) const;

    /// <summary>
    /// マウス位置から時間を取得
    /// </summary>
    float GetTimeFromMousePosition(const EditorCoordinate* coordinate) const;

    /// <summary>
    /// マウス位置からレーンインデックスを取得
    /// </summary>
    int32_t GetLaneIndexFromMousePosition(const EditorCoordinate* coordinate) const;

    /// <summary>
    /// マウス位置からノートインデックスを取得
    /// </summary>
    size_t GetNoteIndexFromMousePosition(const Document* document, const EditorCoordinate* coordinate) const;

private:
    Input* input_ = nullptr;

};

} // namespace BME
