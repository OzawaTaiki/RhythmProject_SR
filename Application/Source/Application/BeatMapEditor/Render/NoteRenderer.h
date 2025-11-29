#pragma once

#include <Application/BeatMapLoader/BeatMapData.h>
#include <Application/BeatMapEditor/EditorCoordinate.h>

#include <Features/Sprite/Sprite.h>

#include <vector>
#include <cstdint>

namespace BME
{

class State;

/// <summary>
/// ノート描画クラス
/// </summary>
class NoteRenderer
{
public:
    NoteRenderer() = default;
    ~NoteRenderer() = default;

    void Initialize();
    void Draw(const BeatMapData& _data, const State* _state, const EditorCoordinate* _coordinate, float _currentTime);
    void DrawPreview(const State* _state, const EditorCoordinate* _coordinate);
    void Finalize();

private:
    void DrawNote(const NoteData& _note, const EditorCoordinate* _coordinate, bool _isSelected, size_t& _noteIndex, size_t& _holdNoteIndex);

    void InitNoteSprites();

    void InitNoteBridges();

    void InitHoldNoteEndSprites();


private:

    struct NoteColor
    {
        Vector4 defaultColor;
        Vector4 hoverColor;
        Vector4 selectedColor;
    };


private:
    std::vector<std::unique_ptr<Sprite>> noteSprites_;
    std::vector<std::unique_ptr<Sprite>> noteBridges_;
    std::vector<std::unique_ptr<Sprite>> holdNoteEnd_;

    // プレビュー用スプライト
    std::unique_ptr<Sprite> previewNoteSprite_;
    std::unique_ptr<Sprite> previewBridgeSprite_;
    std::unique_ptr<Sprite> previewHoldEndSprite_;

    NoteColor normalNoteColor_;
    NoteColor holdNoteColor_;

    float previewAlpha_ = 0.5f;
};

} // namespace BME
