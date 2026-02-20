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
    void Draw(const BeatMapData& _data, const State* _state, const EditorCoordinate* _coordinate);
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
        Engine::Vector4 defaultColor;
        Engine::Vector4 hoverColor;
        Engine::Vector4 selectedColor;
    };


private:
    std::vector<std::unique_ptr<Engine::Sprite>> noteSprites_;
    std::vector<std::unique_ptr<Engine::Sprite>> noteBridges_;
    std::vector<std::unique_ptr<Engine::Sprite>> holdNoteEnd_;

    // プレビュー用スプライト
    std::unique_ptr<Engine::Sprite> previewNoteSprite_;
    std::unique_ptr<Engine::Sprite> previewBridgeSprite_;
    std::unique_ptr<Engine::Sprite> previewHoldEndSprite_;

    NoteColor normalNoteColor_;
    NoteColor holdNoteColor_;

    float previewAlpha_ = 0.5f;
};

} // namespace BME
