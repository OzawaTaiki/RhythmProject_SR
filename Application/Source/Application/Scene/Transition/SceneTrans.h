#pragma once

#include <Features/Scene/Interface/ISceneTransition.h>
#include <Features/UI/Element/UITextElement.h>

#include <Features/Sprite/Sprite.h>

// シーン遷移クラス フェードイン・アウト
/// <summary>
/// フェードイン・フェードアウトによるシーン遷移を実装するクラス。
/// </summary>
class SceneTrans : public Engine::ISceneTransition
{
public:
    SceneTrans() = default;
    ~SceneTrans() override = default;

    void Initialize() override;
    void Update() override;
    void Draw() override;

    bool IsEnd() const override { return isEnd_; };
    bool CanSwitch() const override { return canSwitch_; };

    void Start() override;
    void End() override;

private:

    float duration_     = 1.0f; // トランジションの持続時間
    float elapsedTime_  = 0.0f; // 経過時間
    float alpha_        = 0.0f; // トランジションのアルファ値
    float fadeSpeed_    = 1.0f; // フェード速度

    std::unique_ptr<Engine::Sprite> transitionSprite_   = nullptr; // トランジション用のスプライト
    std::unique_ptr<Engine::UITextElement> loadingText_ = nullptr;

    bool isEnd_     = false; // トランジションが終了したか
    bool canSwitch_ = false; // シーンを切り替え可能か
    bool playing_   = false; // トランジションが再生中か

};