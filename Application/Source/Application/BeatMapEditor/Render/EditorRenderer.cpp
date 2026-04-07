#include "EditorRenderer.h"

#include <Application/BeatMapEditor/EditorState.h>
#include <Application/BeatMapEditor/BeatMapDocument.h>
#include <Application/BeatMapEditor/AudioController.h>
#include <Application/BeatMapEditor/EditorCoordinate.h>
#include <Application/BeatMapEditor/BeatMapFileManager.h>
#include <Application/BeatsManager/BeatManager.h>
#include <Framework/LayerSystem/LayerSystem.h>

using namespace Engine;


namespace BME {

void EditorRenderer::Initialize(EditorCoordinate* _coordinate, State* _state,const Matrix4x4& _matVP)
{
    noteRenderer_.Initialize();
    gridRenderer_.Initialize(_coordinate);
    waveformRenderer_.Initialize(_matVP);
    timelineRenderer_.Initialize([_state]() { _state->SetToTestMode(true); });
    uiController_.Initialize();

    LayerSystem::CreateLayer("main", 0);
    LayerSystem::CreateLayer("mid", 100);
    LayerSystem::CreateLayer("top", 2000);
}

void EditorRenderer::Update(
    State* _state,
    Document* _document,
    FileManager* _fileManager,
    AudioController* _audioController,
    EditorCoordinate* _coordinate,
    BeatManager* beatManager,
    float& _currentTime,
    AutoChartGenerator::GenerateRequest& autoGenerateRequest
    )
{
    if (!_state || !_document || !_audioController || !_coordinate)
        return;

    // タイムライン処理
    timelineRenderer_.ProcessTimeline(_audioController, _currentTime);

    // UI処理
    uiController_.ProcessUI(_state,
                            _document,
                            _audioController,
                            _fileManager,
                            beatManager,
                            _coordinate,
                            autoGenerateRequest);
}

void EditorRenderer::Draw(
    State* _state,
    Document* _document,
    AudioController* _audioController,
    EditorCoordinate* _coordinate,
    float _currentTime
    ) const
{
    if (!_state || !_document || !_audioController || !_coordinate)
        return;

    const BeatMapData& data = _document->GetData();

    // lane
    // notes previwe
    // grid
    // playhead selectarea
    // wave timeline
    // UI

    LayerSystem::SetLayer("main");
    {
        // グリッド・レーン描画
        gridRenderer_.Draw(_coordinate, _state, data.bpm, _currentTime);

        // ノート描画
        noteRenderer_.Draw(data, _state, _coordinate);

        // プレビューノート描画
        noteRenderer_.DrawPreview(_state, _coordinate);
    }

    LayerSystem::SetLayer("mid");
    {
        // 波形背景
        waveformRenderer_.DrawBackSprite();
    }

    LayerSystem::SetLayer("top");
    // 波形描画
    {
        waveformRenderer_.Draw(_audioController, _currentTime);

        // UI描画
        uiController_.Draw(_state);
    }
}

void EditorRenderer::Finalize()
{
    noteRenderer_.Finalize();
    gridRenderer_.Finalize();
    waveformRenderer_.Finalize();
    timelineRenderer_.Finalize();
    uiController_.Finalize();
}

} // namespace BME
