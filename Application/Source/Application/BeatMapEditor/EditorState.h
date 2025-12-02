#pragma once
#include <cstdint>
#include <vector>
#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/BeatMapEditor/LiveMapping/LiveMapping.h>
#include <Application/BeatMapEditor/BPMCounter/TapBPMCounter.h>
#include <Math/Vector/Vector2.h>


namespace BME
{
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


struct MoveState
{
    bool isMoving = false; // 移動中フラグ
    std::vector<float> originalTimes; // 元の時間
    std::vector<size_t> movingIndices; // 移動対象のノートインデックス
    float startMouseTime = 0.0f; // 移動開始時のマウスtime
    float timeOffset = 0.0f;
};


struct ClipboardData
{
    std::vector<NoteData> notes; // コピーしたノートデータ
    float baseline = 0.0f; // コピーしたノートの基準時間

    void Clear()
    {
        notes.clear();
        baseline = 0.0f;
    };
    bool IsEmpty() const
    {
        return notes.empty();
    }
};

/// <summary>
/// エディタの状態を管理するクラス
/// </summary>
class State
{
public:

    State();
    ~State() = default;

    EditorMode GetCurrentMode() const { return currentMode_; }
    EditorMode GetPreviousMode() const { return previousMode_; }
    void SetCurrentMode(EditorMode mode);


    void SelectNote(uint32_t noteIndex, bool multiSelect);
    void ClearSelection();

    bool IsNoteSelected(uint32_t _noteIndex) const;

    const std::vector<size_t>& GetSelectedNotes() const { return selectedNoteIndices_; }
    bool IsMovingSelectedNote() const { return moveState_.isMoving; }

    void Copy(const std::vector<NoteData>& notes, float baseline);
    std::vector<NoteData> GetPasteNotes(float currentTime) const;
    bool IsClipboardEmpty() const { return clipboardData_.IsEmpty(); }

    void ChangeEditorMode(EditorMode _mode);

    // ロングノート作成状態
    void StartLongNoteCreation(uint32_t laneIndex, float startTime);
    void EndLongNoteCreation();
    bool IsCreatingLongNote() const { return isCreatingLongNote_; }
    float GetLongNoteStartTime() const { return longNoteStartTime_; }
    uint32_t GetLongNoteLaneIndex() const { return longNoteLaneIndex_; }

    // ノート移動状態
    void StartMove(const std::vector<NoteData>& notes, float mouseAtTime);
    bool IsMoving() const { return moveState_.isMoving; }
    void EndMove();
    MoveState& GetMoveState() { return moveState_; }

    // ドラッグ選択状態
    void StartDragSelection(const Vector2& start);
    void UpdateDragSelection(const Vector2& current);
    void EndDragSelection();
    bool IsDragging() const { return isDragging_; }
    void GetDragSelectionRect(Vector2& LT,Vector2& RB) const;

    // グリッドスナップ設定
    void SetGridSnapEnabled(bool enabled) { gridSnapEnabled_ = enabled; }
    bool IsGridSnapEnabled() const { return gridSnapEnabled_; }
    void SetSnapInterval(float interval) { snapInterval_ = interval; }
    float GetSnapInterval() const { return snapInterval_; }

    void SetToTestMode(bool toTest) { toTest_ = toTest; }
    bool IsToTestMode() const { return toTest_; }


    TapBPMCounter& GetTapBPMCounter() { return tapBPMCounter_; }
    LiveMapping& GetLiveMapping() { return liveMapping_; }

private:

    EditorMode currentMode_ = EditorMode::Select;
    EditorMode previousMode_ = EditorMode::Select;
    bool gridSnapEnabled_ = true;
    float snapInterval_ = 1.0f / 4.0f;

    std::vector<size_t> selectedNoteIndices_;
    bool isMovingSelectedNote_ = false;
    int32_t lastSelectedNoteIndex_ = -1;
    bool isRangeSelected_ = false;
    MoveState moveState_;
    ClipboardData clipboardData_;

    // ロングノート作成状態
    bool isCreatingLongNote_ = false;
    uint32_t longNoteLaneIndex_ = 0;
    float longNoteStartTime_ = 0.0f;

    // ドラッグ選択状態
    bool isDragging_ = false;
    Vector2 dragStartPosition_{ 0.0f, 0.0f };
    Vector2 dragCurrentPosition_{ 0.0f, 0.0f };


    LiveMapping liveMapping_;
    TapBPMCounter tapBPMCounter_;

    bool toTest_ = false;
};

}// namespace BME