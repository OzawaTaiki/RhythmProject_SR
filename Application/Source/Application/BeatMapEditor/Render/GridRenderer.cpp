#include "GridRenderer.h"

#include <Math/Vector/Vector4.h>
#include <Core/DXCommon/TextureManager/TextureManager.h>

namespace BME
{

namespace
{
const std::vector<Vector4> gridColors = {
    Vector4(1.0f  ,1.0f   ,1.0f,   1.0f), // 1
    Vector4(1.0f  ,1.0f   ,0.0f,   1.0f), // 2
    Vector4(1.0f  ,0.5f   ,0.0f,   1.0f), // 4
    Vector4(1.0f  ,0.0f   ,1.0f,   1.0f), // 8
    Vector4(0.0f  ,0.0f   ,1.0f,   1.0f)  // 16
};

}

void GridRenderer::Initialize(const EditorCoordinate* coordinate)
{
    lineDrawer_ = LineDrawer::GetInstance();
    // TODO: グリッドスプライト初期化
    InitLaneSprites(coordinate);
    InitPlayheadSprite(coordinate);

}

void GridRenderer::Draw(const EditorCoordinate* coordinate,
                        const State* state, float bpm, float currentTime)
{
    DrawLanes();
    DrawGridLines(coordinate,state, bpm);
    DrawJudgeLine(coordinate,currentTime);
}

void GridRenderer::DrawLanes()
{
    for (size_t i = 0; i < laneSprites_.size(); ++i)
    {
        laneSprites_[i]->Draw(); // レーンを描画
    }
}

void GridRenderer::DrawGridLines(const EditorCoordinate* coordinate, const State* state, float bpm)
{
    float snapInterval = state->GetSnapInterval();

    auto gridY = coordinate->GetGridLinesY(bpm, static_cast<int>(1.0f / snapInterval)); // グリッドラインのY座標を取得

    // グリッドラインはLineで描画
    float gridLeftX = coordinate->GetEditAreaX();
    float gridRightX = gridLeftX + coordinate->GetEditAreaWidth();

    const float top = coordinate->GetTopMargin();
    const float bottom = coordinate->GetEditAreaHeight() + top;

    for (auto& [y, n] : gridY)
    {
        if (y < top || y > bottom)
            continue;

        lineDrawer_->RegisterPoint(Vector2(gridLeftX, y), Vector2(gridRightX, y), gridColors[n]);
    }

}

void GridRenderer::DrawJudgeLine(const EditorCoordinate* _coordinate, float currentTime)
{
    // TODO: 判定ライン描画
    float playheadX = _coordinate->GetEditAreaX() - playheadSprite_->GetSize().x / 2.0f; // 再生ヘッドのX座標を設定
    float playheadY = _coordinate->TimeToScreenY(currentTime); // 再生ヘッドのY座標を設定
    playheadSprite_->translate_ = Vector2(playheadX, playheadY); // 再生ヘッドの位置を設定

    playheadSprite_->Draw();

    // ラインの描画
    float lineY = playheadY;
    float lineLeftX = _coordinate->GetEditAreaX();
    float lineRightX = lineLeftX + _coordinate->GetEditAreaWidth();
    lineDrawer_->RegisterPoint(Vector2(lineLeftX, lineY), Vector2(lineRightX, lineY), Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // 再生ヘッドのラインを描画
}

void GridRenderer::InitLaneSprites(const EditorCoordinate* coordinate)
{
    laneSprites_.clear();
    float laneWidth = coordinate->GetLaneWidth();
    for (int32_t i = 0; i < coordinate->GetLaneCount(); ++i)
    {
        auto laneSprite = std::make_unique<Sprite>("LaneSprite_" + std::to_string(i),false);

        laneSprite->Initialize();
        laneSprite->SetAnchor(Vector2(0.5f, 0.0f)); // レーンのアンカーを中央下に設定
        laneSprite->SetSize(Vector2(laneWidth, coordinate->GetEditAreaHeight())); // レーンのサイズを設定
        laneSprite->translate_ = Vector2(coordinate->GetLaneLeftX(i) + laneWidth / 2.0f, coordinate->GetBottomMargin()); // レーンの位置を設定
        laneSprite->SetColor(Vector4(0.3f, 0.3f, 0.3f, 1.0f));

        laneSprites_.push_back(std::move(laneSprite)); // スプライトをリストに格納
    }
}

void GridRenderer::InitPlayheadSprite(const EditorCoordinate* coordinate)
{// playheadのスプライトを初期化
    uint32_t triangleTextureHandle = TextureManager::GetInstance()->Load("triangle.png");

    playheadSprite_ = std::make_unique<Sprite>("PlayheadSprite");
    playheadSprite_->Initialize();
    playheadSprite_->SetTextureHandle(triangleTextureHandle); // テクスチャを設定
    playheadSprite_->SetAnchor(Vector2(0.5f, 0.5f)); // 再生ヘッドのアンカーを中央下に設定
    playheadSprite_->SetSize(Vector2(20.0f, 20.0f)); // 再生ヘッドのサイズを設定
    playheadSprite_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // 再生ヘッドの色を赤に設定
    playheadSprite_->rotate_ = 1.57f; // 再生ヘッドの回転を設定（PI/2ラジアン）

    float playheadsize = playheadSprite_->GetSize().x; // 再生ヘッドのサイズを取得
    Vector2 playheadPos = { coordinate->GetEditAreaX() - playheadsize / 2.0f, coordinate->TimeToScreenY(0) }; // 再生ヘッドの初期位置を設定
    playheadSprite_->translate_ = playheadPos;

    // ジャッジラインのスプライトを初期化
    judgeLineSprite_ = std::make_unique<Sprite>("JudgeLineSprite");
    judgeLineSprite_->Initialize();
    judgeLineSprite_->SetAnchor(Vector2(0.0f, 0.5f));// ジャッジラインのアンカーを左中央に設定
    float judgeLineXMargin = 30.0f; // ジャッジラインのXマージンを設定
    judgeLineSprite_->SetSize(Vector2(coordinate->GetEditAreaWidth() + judgeLineXMargin, 5.0f)); // ジャッジラインのサイズを設定
    judgeLineSprite_->translate_ = Vector2(coordinate->GetEditAreaX() - judgeLineXMargin / 2.0f, coordinate->TimeToScreenY(0)); // ジャッジラインの位置を設定
    judgeLineSprite_->SetColor(Vector4(0.8f, 0.2f, 0.2f, 1.0f)); // ジャッジラインの色を設定
}

void GridRenderer::Finalize()
{
    // TODO: 終了処理
}

} // namespace BME
