#pragma once

#include <Features/Scene/Interface/BaseScene.h>
#include <Features/Camera/Camera/Camera.h>
#include <Features/Camera/DebugCamera/DebugCamera.h>
#include <System/Input/Input.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <System/Time/Stopwatch.h>
#include <Features/Effect/Manager/ParticleSystem.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>
#include <Features/Sprite/Sprite.h>
#include <System/Audio/AudioSystem.h>
#include <Features/Event/EventListener.h>

#include <Features/PostEffects/BoxFilter.h>
#include <Features/PostEffects/Vignette.h>
#include <Features/PostEffects/DepthBasedOutLine.h>
#include <Features/PostEffects/Bloom.h>

// Application
#include <Application/Core/GameCore.h>
#include <Application/Input/GameInputManager.h>
#include <Application/FeedBack/FeedbackEffect.h>
#include <Application/GameEnvironment/GameEnvironment.h>
#include <Application/GameMusic/GameMusic.h>
#include <Application/GameUI/GameUI.h>

#include <Application/BeatsManager/BeatManager.h>
#include <Application/BeatMapLoader/BeatMapLoader.h>
#include <Application/PauseMenu/PauseMenu.h>
#include <Application/Setting/SettingMenu.h>
#include <Features/AudioSpectrum/SpectrumTextureGenerator.h>
#include <Features/AudioSpectrum/AudioSpectrum.h>

#include <thread>

enum class GameMode
{
    Normal,
    EditorTest
};
// TODO : だいぶ肥大化しているので分割することを検討する
// ゲームシーン
/// <summary>
/// ゲームのメインプレイシーンを表すクラス。ゲームの初期化、更新、描画、イベント処理を行う。
/// </summary>
class GameScene : public BaseScene, public iEventListener
{
public:
    /// <summary>
    /// コンストラクタ。
    /// </summary>
    GameScene();

    /// <summary>
    /// デストラクタ。
    /// </summary>
    ~GameScene() override;

    /// <summary>
    /// シーンの初期化処理。
    /// </summary>
    /// <param name="sceneData">シーンに渡す初期データ</param>
    void Initialize(SceneData* sceneData) override;

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理。
    /// </summary>
    void Draw() override;

    /// <summary>
    /// シャドウ用の描画処理。
    /// </summary>
    void DrawShadow() override;

private:

    /// <summary>
    /// 譜面データの読み込みを待機する
    /// </summary>
    /// <param name="_filePath">譜面データのファイルパス</param>
    /// <returns> 読み込み完了 or 読み込み済みなら true </returns>
    bool IsCompleteLoadBeatMap();

    /// <summary>
    /// ゲーム開始オフセット処理の更新
    /// </summary>
    void UpdateGameStartOffset();


    // 曲の再生が終わったか
    bool IsMusicEnd() const;
    // リトライ処理
    void Retry();
    //  タイトルへ戻る処理
    void ToTitle();
    // イベント受信処理
    void OnEvent(const GameEvent& event) override;

    void Load(const std::string& beforeScene,const std::string &filepth,const BeatMapData& data);
private:

    // シーン関連
    Camera SceneCamera_ = {};
    Camera camera2d_ = {};
    DebugCamera debugCamera_ = {};
    bool enableDebugCamera_ = false;

    LineDrawer* lineDrawer_ = nullptr;
    Input* input_ = nullptr;
    ParticleSystem* particleSystem_ = nullptr;

    std::shared_ptr<LightGroup> lightGroup_ = nullptr;

    // Application

    std::unique_ptr<GameCore> gameCore_ = nullptr;                  // ゲームの核となる部分
    std::unique_ptr<GameInputManager> gameInputManager_ = nullptr;  // ゲームの入力管理
    std::unique_ptr<FeedbackEffect> feedbackEffect_ = nullptr;      // フィードバックエフェクト
    std::unique_ptr<GameEnvironment> gameEnvironment_ = nullptr; // ゲーム環境のオブジェクト配置
    std::unique_ptr<GameUI> gameUI_ = nullptr; // ゲームUI

    std::unique_ptr<PauseMenu> pauseMenu_ = nullptr; // 一時停止メニュー
    std::unique_ptr<SettingMenu> settingMenu_ = nullptr; // 設定メニュー

    BeatMapLoader* beatMapLoader_ = nullptr;
    BeatMapData currentBeatMapData_ = {}; // 現在の譜面データ

    std::unique_ptr<BeatManager> beatManager_ = nullptr;
    std::future<bool> beatMapLoadFuture_ = {};

    std::unique_ptr<SpectrumTextureGenerator> spectrumTextureGenerator_ = nullptr;
    AudioSpectrum audioSpectrum_;


    bool isBeatMapLoaded_ = false;

    bool isWatingForStart_ = false;
    float gameStartOffset_ = 3.0f;
    float waitTimer_ = 0.0f;

    bool isMusicPlaying_ = false; // 音楽が再生中かどうか

    std::unique_ptr<GameMusic> gameMusic_ = nullptr; // 音楽の管理


    GameMode gameMode_ = GameMode::Normal;

    std::unique_ptr<DepthBasedOutLine> depthBasedOutLine_ = nullptr;
    DepthBasedOutLineData depthBasedOutLineData_ = {};

    std::unique_ptr<Bloom> bloom_ = nullptr;
    BloomConstantBufferData bloomData_ = {};
    BloomBlurConstantBufferData bloomBlurData_ = {};

    void ImGui();
    // 楽曲終了後遷移するか
    bool isTransitionToResultScene_ = false;
    bool noteUpdateEnabled_ = true; // ノートの更新を有効にするかどうか


    std::thread loadingThread_; std::atomic<bool> isLoadComplete_;
};