#include "NoteRenderer.h"
#include <Application/BeatMapEditor/EditorState.h>
#include <System/Input/Input.h>

using namespace Engine;


namespace BME
{
namespace
{
const Vector2 kNoteSize = {50.0f, 25.0f};
const Vector2 kBridgeSize = {40.0f, 25.0f};
const size_t kMaxNoteCount = 1 << 6; // 最大ノート数を設定 一旦64個とする
}

void NoteRenderer::Initialize()
{
    InitNoteSprites();
    InitNoteBridges();
    InitHoldNoteEndSprites();

    normalNoteColor_.defaultColor = Vector4(0.31f, 0.76f, 0.97f, 1.0f); // デフォルトのノート色
    normalNoteColor_.hoverColor = Vector4(0.98f, 0.83f, 0.51f, 1.0f); // ホバー時の色
    normalNoteColor_.selectedColor = Vector4(0.97f, 0.98f, 0.01f, 1.0f); // 選択時の色

    holdNoteColor_.defaultColor = Vector4(0.40f, 0.73f, 0.42f, 1.0f); // デフォルトのロングノート色
    holdNoteColor_.hoverColor = Vector4(0.98f, 0.83f, 0.51f, 1.0f); // ホバー時の色
    holdNoteColor_.selectedColor = Vector4(0.97f, 0.98f, 0.01f, 1.0f); // 選択時の色

    // プレビュー用スプライト初期化
    previewNoteSprite_ = std::make_unique<Sprite>("PreviewNoteSprite", false);
    previewNoteSprite_->Initialize();
    previewNoteSprite_->SetAnchor({0.5f, 0.5f});
    previewNoteSprite_->SetSize(kNoteSize);

    previewBridgeSprite_ = std::make_unique<Sprite>("PreviewBridgeSprite", false);
    previewBridgeSprite_->Initialize();
    previewBridgeSprite_->SetAnchor({0.5f, 1.0f});
    previewBridgeSprite_->SetSize(kBridgeSize);

    previewHoldEndSprite_ = std::make_unique<Sprite>("PreviewHoldEndSprite", false);
    previewHoldEndSprite_->Initialize();
    previewHoldEndSprite_->SetAnchor({0.5f, 0.5f});
    previewHoldEndSprite_->SetSize(kNoteSize);
}

void NoteRenderer::Draw(const BeatMapData& _data, const State* _state, const EditorCoordinate* _coordinate)
{
    size_t noteIndex = 0; // ノートスプライトのインデックス
    size_t holdNoteIndex = 0; // ロングノートスプライトのインデックス

    float startTime, endTime;
    _coordinate->GetVisibleTimeRange(startTime, endTime);

    // 可視範囲内のノートのみ描画
    for (size_t i = 0; i < _data.notes.size(); ++i)
    {
        const NoteData& note = _data.notes[i];

        // 可視範囲チェック
        if ((note.targetTime < startTime || note.targetTime > endTime) &&
            (note.targetTime + note.holdDuration < startTime || note.targetTime + note.holdDuration > endTime))
        {
            if (note.noteType == "normal")
                continue;

            // ロングノートの場合、ブリッジが画面内にあるかチェック
            float visibleTimeRange = endTime - startTime;
            if (visibleTimeRange > note.holdDuration)
            {
                float dir1 = note.targetTime - startTime;
                float dir2 = note.targetTime + note.holdDuration - startTime;

                // 同じ方向ならブリッジが画面内に入らない
                if (dir1 * dir2 > 0.0f)
                    continue;
            }
        }

        // スプライト数の上限チェック
        if (noteIndex >= noteSprites_.size())
            break;

        bool isSelected = _state->IsNoteSelected(static_cast<uint32_t>(i));
        DrawNote(note, _coordinate, isSelected, noteIndex, holdNoteIndex);
    }
}

void NoteRenderer::DrawNote(const NoteData& _note, const EditorCoordinate* _coordinate, bool _isSelected, size_t& _noteIndex, size_t& _holdNoteIndex)
{
    if (_noteIndex >= noteSprites_.size())
        return;

    float noteX = _coordinate->LaneToScreenX(_note.laneIndex);
    float noteY = _coordinate->TimeToScreenY(_note.targetTime);

    // 色の選択
    bool isLongNote = (_note.noteType == "long" || _note.noteType == "hold");
    const NoteColor& noteColor = isLongNote ? holdNoteColor_ : normalNoteColor_;
    Vector4 color = _isSelected ? noteColor.selectedColor : noteColor.defaultColor;

    // ロングノート（hold/long）の描画
    if (isLongNote && _note.holdDuration > 0.0f)
    {
        if (_holdNoteIndex < noteBridges_.size())
        {
            // ブリッジの描画
            noteBridges_[_holdNoteIndex]->translate_ = {noteX, noteY};
            noteBridges_[_holdNoteIndex]->SetColor(color);
            noteBridges_[_holdNoteIndex]->SetSize({40.0f, _coordinate->GetPixelsPerSecond() * _note.holdDuration});
            noteBridges_[_holdNoteIndex]->Update();
            noteBridges_[_holdNoteIndex]->Draw();

            // 終端ノートの描画
            holdNoteEnd_[_holdNoteIndex]->translate_ = {noteX, noteY - _coordinate->GetPixelsPerSecond() * _note.holdDuration};
            holdNoteEnd_[_holdNoteIndex]->SetColor(color);
            holdNoteEnd_[_holdNoteIndex]->Update();
            holdNoteEnd_[_holdNoteIndex]->Draw();

            ++_holdNoteIndex;
        }
    }

    // 開始ノートの描画
    noteSprites_[_noteIndex]->translate_ = {noteX, noteY};
    noteSprites_[_noteIndex]->SetColor(color);
    noteSprites_[_noteIndex]->Update();
    noteSprites_[_noteIndex]->Draw();

    ++_noteIndex;
}

void NoteRenderer::InitNoteSprites()
{
    noteSprites_.clear();
    noteSprites_.resize(kMaxNoteCount); // ノートスプライトのリストを予約
    for (size_t i = 0; i < kMaxNoteCount; ++i)
    {
        auto sprite = std::make_unique<Sprite>("NoteSprite_" + std::to_string(i), false);
        sprite->Initialize();
        sprite->SetAnchor({0.5f, 0.5f}); // ノートのアンカーを中央に設定
        sprite->SetSize(kNoteSize); // ノートのサイズを設定

        noteSprites_[i] = std::move(sprite); // スプライトをリストに格納
    }
}

void NoteRenderer::InitNoteBridges()
{
    noteBridges_.clear(); // ノートとスプライトのブリッジをクリア
    noteBridges_.reserve(kMaxNoteCount / 2); // ブリッジのリストを予約
    for (size_t i = 0; i < kMaxNoteCount / 2; ++i)
    {
        auto bridgeSprite = std::make_unique<Sprite>("NoteBridgeSprite_" + std::to_string(i), false);
        bridgeSprite->Initialize();
        bridgeSprite->SetAnchor({0.5f, 1.0f});// ブリッジのアンカーを中央上に設定
        bridgeSprite->SetSize(kBridgeSize); // ブリッジのサイズを設定
        noteBridges_.push_back(std::move(bridgeSprite)); // ブリッジをリストに格納
    }

}

void NoteRenderer::InitHoldNoteEndSprites()
{
    holdNoteEnd_.clear(); // ロングノートの終端スプライトをクリア
    holdNoteEnd_.reserve(kMaxNoteCount / 2); // 終端スプライトのリストを予約
    for (size_t i = 0; i < kMaxNoteCount / 2; ++i)
    {
        auto longNoteEndSprite = std::make_unique<Sprite>("LongNoteEndSprite_" + std::to_string(i), false);
        longNoteEndSprite->Initialize();
        longNoteEndSprite->SetAnchor({0.5f, 0.5f}); // ロングノートの終端のアンカーを中央に設定
        longNoteEndSprite->SetSize(kNoteSize); // ロングノートの終端のサイズを設定
        holdNoteEnd_.push_back(std::move(longNoteEndSprite)); // 終端スプライトをリストに格納
    }
}

void NoteRenderer::DrawPreview(const State* state, const EditorCoordinate* coordinate)
{
    if (!state || !coordinate)
        return;

    EditorMode mode = state->GetCurrentMode();
    if (mode != EditorMode::PlaceNormalNote && mode != EditorMode::PlaceLongNote)
        return;

    // Input取得（TODO: 依存注入に変更を検討）
    Input* input = Input::GetInstance();
    Vector2 mousePos = input->GetMousePosition();

    // レーン内かチェック
    int32_t laneIndex = coordinate->ScreenXToLane(mousePos.x);
    if (laneIndex < 0 || laneIndex >= static_cast<int32_t>(coordinate->GetLaneCount()))
        return;

    float previewX = coordinate->LaneToScreenX(laneIndex);
    float previewY = mousePos.y;

    // 色設定
    const NoteColor& noteColor = (mode == EditorMode::PlaceNormalNote) ? normalNoteColor_ : holdNoteColor_;
    Vector4 color = noteColor.defaultColor;
    color.w = previewAlpha_;

    // 通常ノート配置モード
    if (mode == EditorMode::PlaceNormalNote)
    {
        previewNoteSprite_->translate_ = {previewX, previewY};
        previewNoteSprite_->SetColor(color);
        previewNoteSprite_->Update();
        previewNoteSprite_->Draw();
    }
    // ロングノート配置モード
    else if (mode == EditorMode::PlaceLongNote)
    {
        if (state->IsCreatingLongNote())
        {
            // ロングノート作成中：開始位置と終了位置を描画
            float startTime = state->GetLongNoteStartTime();
            uint32_t startLane = state->GetLongNoteLaneIndex();
            float posX = coordinate->LaneToScreenX(startLane);
            float posY = coordinate->TimeToScreenY(startTime);

            // 開始ノート
            previewNoteSprite_->translate_ = {posX, posY};
            previewNoteSprite_->SetColor(color);
            previewNoteSprite_->Update();
            previewNoteSprite_->Draw();

            // 持続時間を計算
            float holdDuration = coordinate->ScreenYToTime(mousePos.y) - coordinate->ScreenYToTime(posY);
            if (holdDuration > 0.0f)
            {
                // 終端ノート
                previewHoldEndSprite_->translate_ = {posX, previewY};
                previewHoldEndSprite_->SetColor(color);
                previewHoldEndSprite_->Update();
                previewHoldEndSprite_->Draw();

                // ブリッジ
                previewBridgeSprite_->translate_ = {posX, posY};
                previewBridgeSprite_->SetColor(color);
                previewBridgeSprite_->SetSize({40.0f, holdDuration * coordinate->GetPixelsPerSecond()});
                previewBridgeSprite_->Update();
                previewBridgeSprite_->Draw();
            }
        }
        else
        {
            // ロングノート未作成：マウス位置にプレビュー表示
            previewNoteSprite_->translate_ = {previewX, previewY};
            previewNoteSprite_->SetColor(color);
            previewNoteSprite_->Update();
            previewNoteSprite_->Draw();
        }
    }
}

void NoteRenderer::Finalize()
{
    // TODO: 終了処理
}

} // namespace BME
