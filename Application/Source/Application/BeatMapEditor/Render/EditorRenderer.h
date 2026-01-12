#pragma once

#include <Application/BeatMapEditor/Render/NoteRenderer.h>
#include <Application/BeatMapEditor/Render/GridRenderer.h>
#include <Application/BeatMapEditor/Render/WaveformRenderer.h>
#include <Application/BeatMapEditor/Render/TimelineRenderer.h>
#include <Application/BeatMapEditor/Render/EditorUIController.h>

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

    void Initialize(EditorCoordinate* _coordinate,State* _state ,const Matrix4x4& _matVP);
    void Update(
        State* _state,
        Document* _document,
        FileManager* _fileManager,
        AudioController* _audioController,
        EditorCoordinate* _coordinate,
        BeatManager* beatManager,
        float& _currentTime
    );
    void Draw(
        State* _state,
        Document* _document,
        AudioController* _audioController,
        EditorCoordinate* _coordinate,
        float _currentTime
    ) const;
    void Finalize();

private:
    mutable NoteRenderer noteRenderer_;
    mutable GridRenderer gridRenderer_;
    mutable WaveformRenderer waveformRenderer_;
    mutable TimelineRenderer timelineRenderer_;
    mutable EditorUIController uiController_;
};

} // namespace BME
