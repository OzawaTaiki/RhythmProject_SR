#pragma once

#include <Features/Scene/Interface/ISceneTransition.h>
#include <Features/Sprite/Sprite.h>
#include <Math/Vector/Vector2.h>
#include <Features/Json/JsonBinder.h>

class SceneTransHex : public Engine::ISceneTransition
{
public:
    SceneTransHex();

    void Initialize() override;
    void Update() override;
    void Draw() override;

    void Start() override;
    void End() override;

    bool IsEnd() const override  { return isEnd_; }
    bool CanSwitch() const override{ return canSwitch_; }

    void ImGui();

private:



    void CreateHexagons(); // 六角形のグリッドを作成

    void InitJsonBinder(); // JSONバインダーの初期化

private:

    bool isEnd_ = false; // トランジションが終了したか
    bool canSwitch_ = false; // シーンを切り替え可能か
    bool playing_ = false; // トランジションが再生中か

    float duration_ = 1.0f; // 全体の処理時間
    float elapsedTime_ = 0.0f; // 経過時間

    // 六角形のサイズ
    float hexagonSize_ = 50.0f;
    // 六角形の間隔
    float spacing_ = 10.0f;

    // 全体を何分割するか フェーズ数
    int32_t phaseCount_ = 5;
    // フェーズ間の遅延時間
    int32_t totalHexagons_ = 0; // 六角形の総数
    int32_t gridWidth_ = 0; // グリッドの幅（六角形の数）
    int32_t gridHeight_ = 0; // グリッドの高さ（六角形の数）


    // 六角形のひとつのパラメータ構造体
    struct Hexagon
    {
        Engine::Vector2 position; // 六角形の位置

        float size; // サイズ
        float elapsedTime; // アニメーションの経過時間
        float delay; // アニメーション開始の遅延時間

        std::unique_ptr<Engine::Sprite> sprite; // 六角形のスプライト
    };

    std::vector<std::unique_ptr<Hexagon>> hexagons_; // 六角形のリスト

    std::unique_ptr<Engine::JsonBinder> jsonBinder_; // JSONバインダー
};