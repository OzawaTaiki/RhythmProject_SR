#include "BeatMapDocument.h"

#include <algorithm>
#include <cmath>

namespace BME {

// ========================================
// ノート操作
// ========================================

void Document::PlaceNote(const NoteData& note)
{
    data_.notes.push_back(note);
    isModified_ = true;
}

NoteData Document::DeleteNote(size_t noteIndex)
{
    if (noteIndex >= data_.notes.size())
    {
        return NoteData{}; // 無効なインデックスの場合は空のNoteDataを返す
    }

    // ノートを削除
    NoteData deletedNote = data_.notes[noteIndex];
    data_.notes.erase(data_.notes.begin() + noteIndex);
    isModified_ = true;

    return deletedNote;
}

size_t Document::InsertNote(const NoteData& note)
{
    data_.notes.push_back(note);
    SortNotesByTime(); // ノートを時間順にソート
    isModified_ = true;

    return FindInsertIndex(note); // 挿入したノートのインデックスを返す
}

void Document::SetNoteTime(size_t noteIndex, float newTime)
{
    if (noteIndex >= data_.notes.size())
    {
        return; // 無効なインデックスの場合は何もしない
    }
    data_.notes[noteIndex].targetTime = newTime; // ノートの時間を更新
    isModified_ = true; // 譜面が変更されたフラグを立てる
}

int32_t Document::FindNoteAtTime(int32_t laneIndex, float time) const
{
    for (size_t i = 0; i < data_.notes.size(); ++i)
    {
        const NoteData& note = data_.notes[i];
        if (note.laneIndex == laneIndex &&
            std::abs(note.targetTime - time) < 0.001f) // 許容範囲を設定
        {
            return static_cast<int32_t>(i); // ノートのインデックスを返す
        }
    }
    return static_cast<int32_t>(-1); // 見つからなかった場合は無効なインデックスを返す
}

const NoteData& Document::GetNoteAt(size_t index) const
{
    static NoteData emptyNote;
    if (index >= data_.notes.size())
    {
        return emptyNote; // 無効なインデックスの場合は空のNoteDataを返す
    }
    return data_.notes[index]; // 指定されたインデックスのノートを返す
}

void Document::SortNotesByTime()
{
    std::sort(data_.notes.begin(), data_.notes.end(),
        [](const NoteData& a, const NoteData& b) {
            return a.targetTime < b.targetTime;
        });
    isModified_ = true;
}

// ========================================
// Private ヘルパー
// ========================================

size_t Document::FindInsertIndex(const NoteData& note) const
{
    for (size_t i = 0; i < data_.notes.size(); ++i)
    {
        const NoteData& existingNote = data_.notes[i];
        if (std::abs(existingNote.targetTime - note.targetTime) < 0.001f &&
            existingNote.laneIndex == note.laneIndex &&
            existingNote.noteType == note.noteType)
        {
            return i;
        }
    }
    return SIZE_MAX; // 見つからなかった場合は最大値を返す
}

} // namespace BME