#pragma once
#include <Features/Sprite/Sprite.h>
#include <Application/BeatMapEditor/AutoChartGenerator/autoChartGenerator.h>

class BeatManager;

namespace BME
{

class State;
class Document;
class AudioController;
class FileManager;
class EditorCoordinate;

/// <summary>
/// エディターUIコントローラークラス（ImGuiパネル等の描画と制御）
/// </summary>
class EditorUIController
{
public:
    EditorUIController() = default;
    ~EditorUIController() = default;

    void Initialize();
    void ProcessUI(State* state,
                   Document* document,
                   AudioController* audioController,
                   FileManager* fileManager,
                   BeatManager* beatManager,
                   EditorCoordinate* coordinate,
                   AutoChartGenerator::GenerateRequest& autoGenerateRequest,
                   bool isGenerating,
                   float progress
    );
    void Draw(const State* state) const;
    void Finalize();


private:
#ifdef _DEBUG
    void DrawLeftPanel(State* state, Document* document, AudioController* audioController, BeatManager* beatManager, EditorCoordinate* coordinate_);
    void DrawRightPanel(State* state,
                        Document* document,
                        AudioController* audioController,
                        FileManager* fileManager,
                        AutoChartGenerator::GenerateRequest& autoGenerateRequest,
                        bool isGenerating,
                        float progress
    );
#endif // _DEBUG
    void UpdateDraggingArea(const State* state);
    void DrawDraggingArea(const State* state) const;

private:
    std::unique_ptr<Engine::Sprite> draggingAreaSprite_;

};

} // namespace BME
