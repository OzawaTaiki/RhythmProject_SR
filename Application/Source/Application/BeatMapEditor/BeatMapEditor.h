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
    void Finalize();

public:

    /// <summary>
    /// テストモードへ移行するフラグを取得する。
    /// </summary>
    bool ToTestMode() const { return toTest_; }

    /// <summary>
    /// 現在の譜面データを取得する。
    /// </summary>
    BeatMapData GetBeatMapData() const { return currentBeatMapData_; }

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

    // ========================================
    // 譜面データ
    // ========================================
    BeatMapData currentBeatMapData_;
    std::string currentFilePath_;
    bool isModified_ = false;

    // ========================================
    // 音楽・再生制御
    // ========================================
    std::shared_ptr<SoundInstance> musicSoundInstance_;
    std::shared_ptr<VoiceInstance> musicVoiceInstance_;
    std::shared_ptr<VoiceInstance> voiceInstanceForBPMSet_;
    float volume_ = 0.3f;
    float currentTime_ = 0.0f;
    bool isPlaying_ = false;

    // ========================================
    // エディター状態・設定
    // ========================================
    EditorMode currentEditorMode_ = EditorMode::Select;
    EditorMode preCurrentEditorMode_ = EditorMode::Select;
    bool gridSnapEnabled_ = true;
    float snapInterval_ = 1.0f / 4.0f;

    // ========================================
    // 選択・編集状態
    // ========================================
    std::vector<size_t> selectedNoteIndices_;
    bool isMovingSelectedNote_ = false;
    int32_t lastSelectedNoteIndex_ = -1;
    bool isRangeSelected_ = false;
    MoveState moveState_;
    ClipboardData clipboardData_;

    // ========================================
    // ロングノート編集状態
    // ========================================
    bool isCreatingLongNote_ = false;
    float longNoteStartTime_ = 0.0f;
    int32_t longNoteStartLane_ = 0;
    bool isSelectingHoldEnd_ = false;
    int32_t selectNoteIndex_ = -1;

    // ========================================
    // 範囲選択・ドラッグ
    // ========================================
    bool isDragging_ = false;
    Vector2 dragStartPosition_ = { 0.0f, 0.0f };
    Vector2 dragEndPosition_ = { 0.0f, 0.0f };

    // ========================================
    // 描画制御・インデックス管理
    // ========================================
    uint32_t noteIndex_ = 0;
    uint32_t holdNoteIndex_ = 0;
    std::vector<int32_t> drawNoteIndices_;

    // ========================================
    // ノート色設定
    // ========================================
    NoteColor normalNoteColor_;
    NoteColor longNoteColor_;

    // ========================================
    // プレビュー機能
    // ========================================
    std::unique_ptr<UISprite> previewNoteSprite_;
    std::unique_ptr<UISprite> previewBridgeSprite_;
    std::unique_ptr<UISprite> previewHoldEndSprite_;
    float previewAlpha_ = 0.5f;

    // ========================================
    // 描画用スプライト群
    // ========================================
    // ノート描画
    std::vector<std::unique_ptr<UISprite>> noteSprites_;
    std::vector<std::unique_ptr<UISprite>> noteBridges_;
    std::vector<std::unique_ptr<UISprite>> holdNoteEnd_;

    // レーン・UI描画
    std::vector<std::unique_ptr<UISprite>> laneSprites_;
    std::unique_ptr<UISprite> judgeLineSprite_;
    std::unique_ptr<UISprite> playheadSprite_;

    // Timeline
    std::map<std::string, std::unique_ptr<UISprite>> timelineSprites_;
    std::unique_ptr<UISprite> dummy_timeline_;
    float timelineStartPosition_ = 0.0f; // タイムラインの開始位置
    float timelineEndPosition_ = 0.0f; // タイムラインの終了位置
    float timelineWidth_ = 0.0f; // タイムラインの幅

    // 選択・判定用
    std::unique_ptr<UIBase> areaSelectionSprite_;
    std::unique_ptr<UIBase> dummy_editLaneArea_;
    std::unique_ptr<UIBase> dummy_window_;
    std::unique_ptr<UIBase> dummy_editArea_;


    // ========================================
    // 波形表示
    // ========================================
    WaveformDisplay waveformDisplay_;
    WaveformBounds waveformBounds_;
    std::unique_ptr<UISprite> waveformBackground_;

    // ========================================
    // 特殊機能
    // ========================================
    LiveMapping liveMapping_;
    TapBPMCounter tapBPMCounter_;


    bool toTest_ = false; // テストモードへ移行フラグ
    TextParam textParam_;
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