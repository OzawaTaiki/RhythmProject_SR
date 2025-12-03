#pragma once

#include <Features/LineDrawer/LineDrawer.h>
#include <Features/Sprite/Sprite.h>

#include <Application/BeatMapEditor/EditorCoordinate.h>
#include <Application/BeatMapEditor/EditorState.h>


namespace BME
{

/// <summary>
/// グリッド・レーン描画クラス
/// </summary>
class GridRenderer
{
public:
    GridRenderer() = default;
    ~GridRenderer() = default;

    void Initialize(const EditorCoordinate* coordinate);
    void Draw(const EditorCoordinate* coordinate, const State* state, float bpm, float currentTime);
    void Finalize();

private:

    void DrawLanes();
    void DrawGridLines(const EditorCoordinate* _coordinate, const State* state, float _bpm);
    void DrawJudgeLine(const EditorCoordinate* _coordinate,float currentTime);

    void InitLaneSprites(const EditorCoordinate* coordinate);

    void InitPlayheadSprite(const EditorCoordinate* coordinate);
private:

    LineDrawer* lineDrawer_ = nullptr; // ライン描画クラスのポインタ

    std::vector<std::unique_ptr<Sprite>> laneSprites_;
    std::unique_ptr<Sprite> judgeLineSprite_;
    std::unique_ptr<Sprite> playheadSprite_;
};

} // namespace BME
