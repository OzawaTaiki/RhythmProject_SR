#pragma once

#include <Features/UI/UISprite.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>
#include <Features/WaveformDisplay/WaveformDisplay.h>

#include <Features/TextRenderer/TextGenerator.h>


#include <Application/BeatMapEditor/EditorCoordinate.h>
#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/BeatsManager/BeatManager.h>
#include <Application/BeatMapEditor/Command/CommandHistory.h>


// 新アーキテクチャクラス
#include <Application/BeatMapEditor/BeatMapDocument.h>
#include <Application/BeatMapEditor/EditorState.h>
#include <Application/BeatMapEditor/AudioController.h>
#include <Application/BeatMapEditor/EditorInputHandler.h>
#include <Application/BeatMapEditor/BeatMapFileManager.h>
#include <Application/BeatMapEditor/Render/EditorRenderer.h>

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

// 前方宣言
class Input;
class LineDrawer;
class BeatMapLoader;

/// <summary>
/// ビートマップエディター
/// </summary>
class BeatMapEditor
{
private:
    // エディターモード
    enum class EditorMode
    {
        Select, // 選択
        PlaceNormalNote,// ノーマルノート配置
        PlaceLongNote, // ロングノート配置
        Delete,// ノート削除
        LiveMapping,// ライブマッピング
        BPMSetting,// BPM設定

        Count // モード数
    };
public:

    BeatMapEditor();
    ~BeatMapEditor();

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    /// <param name="_beatMapData">初期化時に読み込む譜面データ（省略可）</param>
    void Initialize(const BeatMapData& _beatMapData = {});

    /// <summary>
    /// 毎フレームの更新処理を行う。
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理を行う。
    /// </summary>
    void Draw();

    /// <summary>
    /// 終了処理を行う。
    /// </summary>
    //void Finalize();

public:

    /// <summary>
    /// テストモードへ移行するフラグを取得する。
    /// </summary>
    bool ToTestMode() const { return !document_->GetData().notes.empty() && state_->IsToTestMode(); }

    /// <summary>
    /// 現在の譜面データを取得する。
    /// </summary>
    BeatMapData GetBeatMapData() const { return document_->GetData(); }

    /// <summary>
    /// ロングノートの持続時間を設定
    /// </summary>
    /// <param name="_noteIndex"> ノートのインデックス</param>
    /// <param name="_newDuration"> 新しい持続時間</param>
    void SetNoteDuration(size_t _noteIndex, float _newDuration);
private:
private:
    struct NoteColor
    {
        Vector4 defaultColor;
        Vector4 hoverColor;
        Vector4 selectedColor;
    };

    struct MoveState
    {
        bool isMoving = false; // 移動中フラグ
        std::vector<float> originalTimes; // 元の時間
        std::vector<size_t> movingIndices; // 移動対象のノートインデックス

    };


    struct ClipboardData
    {
        std::vector<NoteData> notes; // コピーしたノートデータ
        float baseline = 0.0f; // コピーしたノートの基準時間

        void Clear() {
            notes.clear();
            baseline = 0.0f;
        };
        bool IsEmpty() const {
            return notes.empty();
        }
    };
private:
    // ========================================
    // システム依存関係
    // ========================================
    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    Camera for2dCamera_;
    BeatMapLoader* beatMapLoader_ = nullptr;
    std::unique_ptr<BeatManager> beatManager_ = nullptr;
    TextGenerator text_;
    bool enableBeats_ = false;

    // ========================================
    // コア機能
    // ========================================
    CommandHistory commandHistory_;
    BME::EditorCoordinate editorCoordinate_;

    // ========================================
    // 新アーキテクチャクラス（リファクタリング中）
    // ========================================
    std::unique_ptr<BME::Document> document_;
    std::unique_ptr<BME::State> state_;
    std::unique_ptr<BME::AudioController> audioController_;
    std::unique_ptr<BME::InputHandler> inputHandler_;
    std::unique_ptr<BME::EditorRenderer> renderer_;
    std::unique_ptr<BME::FileManager> fileManager_;

    float currentTime_ = 0.0f;
    BeatMapData currentBeatMapData_ = {};
};


/*

操作
    1~ モード選択 (選択 ノーマル配置 ロング配置

    削除は選択からDIK_del
    BPM設定とライブマッピングは何か別のキーを

    タイムラインのスクロールはマウスホイールで
    ズームはalt+スクロール

    画面左にはモードとかを
    画面右にノートのパラメータ表示


グリッドの色
    1/1 白
    1/2 緑
    1/4 青
    1/8 赤
    1/16 紫
*/