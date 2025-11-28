#pragma once
#include <Features/Sprite/Sprite.h>

class BeatManager;

namespace BME
{

class State;
class Document;
class AudioController;
class FileManager;
class EditorCoordinate;

/// <summary>
/// UI描画クラス（ImGuiパネル等）
/// </summary>
class UIRenderer
{
public:
    UIRenderer() = default;
    ~UIRenderer() = default;

    void Initialize();
    void Draw(State* state, 
              Document* document, 
              AudioController* audioController, 
              FileManager* fileManager, 
              BeatManager* beatManager,
              EditorCoordinate* coordinate);
    void Finalize();

private:
    void DrawLeftPanel(State* state, Document* document, AudioController* audioController, BeatManager* beatManager, EditorCoordinate* coordinate_);
    void DrawRightPanel(State* state,Document* document, AudioController* audioController, FileManager* fileManager);
    void DrawDraggingArea(State* state);

private:
    std::unique_ptr<Sprite> draggingAreaSprite_;


};

} // namespace BME
