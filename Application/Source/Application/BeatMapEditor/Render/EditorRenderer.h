#pragma once

#include <Application/BeatMapEditor/Render/NoteRenderer.h>
#include <Application/BeatMapEditor/Render/GridRenderer.h>
#include <Application/BeatMapEditor/Render/WaveformRenderer.h>
#include <Application/BeatMapEditor/Render/TimelineRenderer.h>
#include <Application/BeatMapEditor/Render/UIRenderer.h>

class BeatManager;

namespace BME
{

class State;
class Document;
class FileManager;
class AudioController;
class EditorCoordinate;

/// <summary>
/// エディターレンダリングのファサードクラス
/// </summary>
class EditorRenderer
{
public:
    EditorRenderer() = default;
    ~EditorRenderer() = default;

    void Initialize(EditorCoordinate* _coordinate, const Matrix4x4& _matVP);
    void Draw(
        State* _state,
        Document* _document,
        FileManager* _fileManager,
        AudioController* _audioController,
        EditorCoordinate* _coordinate,
        BeatManager* beatManager,
        float _currentTime
    );
    void Finalize();

private:
    NoteRenderer noteRenderer_;
    GridRenderer gridRenderer_;
    WaveformRenderer waveformRenderer_;
    TimelineRenderer timelineRenderer_;
    UIRenderer uiRenderer_;
};

} // namespace BME
