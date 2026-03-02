#pragma once

#include <Features/UI/Element/UIButtonElement.h>
#include <Features/UI/Element/UIImageElement.h>
#include <Features/Animation/Sequence/AnimationSequence.h>
#include <Features/Event/EventData.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>


// イベント発行時のデータ構造体
struct MusicSelectUIEventData : Engine::EventData
{
    std::string selectedFilePath; // 選択されたファイルパス
};

/// <summary>
/// 選曲UIクラス
/// </summary>
class MusicSelectUI
{
    // 画面外に置いてある大きな円の円周上になるような配置。
    // 円は画面下方向に配置
    // 北半球が出ているような状態になる
    // 画面下部にはスペクトラムはおき，上部には楽曲の情報を出そうと思う
public:
    MusicSelectUI() = default;
    ~MusicSelectUI() = default;
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(std::shared_ptr<Engine::VoiceInstance> voiceInstance);
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update(float deltaTime);
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();

    float GetSelectedMusicElapsedTime() const;
private:

    /// <summary>
    /// 読み込み完了したデータでアイテムを初期化
    /// <summary>
    void InitializeItemsFromData();


    /// <summary>
    /// Jsonバインダー初期化
    /// </summary>
    void InitJsonBinder();

    /// <summary>
    /// レイアウト更新
    /// </summary>
    void UpdateLayout(float deltaTime);

    /// <summary>
    /// スケーリング更新
    /// </summary>
    void UpdateScaling();

    /// <summary>
    /// アイテムカラー更新（選択中はブルーにハイライト）
    /// </summary>
    void UpdateItemColors();

    /// <summary>
    /// UIアイテム数が最低限になるように追加する
    /// </summary>
    void EnsureMinimumItems();

    /// <summary>
    /// 選択インデックスを範囲内に収める
    /// </summary>
    void ClampSelectedIndex();

    /// <summary>
    /// 選択移動処理
    /// </summary>
    void MoveSelection();

    /// <summary>
    /// アイテムにフォーカスが入ったときの処理
    /// </summary>
    void OnItemFocusEnter();

    /// <summary>
    /// アイテムが選択されたときの処理
    /// </summary>
    void OnItemSelected();

    /// <summary>
    /// 選択された楽曲を再生する
    /// </summary>
    void PlaySelectedMusic();

private:

    std::unique_ptr<Engine::UIImageElement> backImage_;
    std::vector<std::unique_ptr<Engine::UIButtonElement>> uiItems_;

    float marginBetweenItems_ = 150.0f; // アイテム間のマージン
    float layoutAngle_ = 0.0f; // レイアウトの角度
    Engine::Vector2 layoutNormalizedDirection_ = { 0.0f, -1.0f }; // レイアウトの正規化された方向ベクトル
    Engine::Vector2 layoutCenter_ = { 960.0f, 540.0f }; // レイアウトの中心位置

    Engine::Vector2 BaseUISize_ ={};
    float selectedScale_ = 1.0f;  // 選択中アイテムの拡大率
    float normalScale_   = 0.8f;  // 非選択アイテムの縮小率

    float bgPaddingX_ = 40.0f;   // 背景の左右余白
    float bgPaddingY_ = 40.0f;   // 背景の上下余白

    int32_t selectedIndex_ = 0;
    int32_t musicListSize_ = 0;

    // 読み込みが終わり、初期化が完了したか
    bool isInitialized_ = false;

    int32_t scrollDirection_ = 0; // スクロール方向 -1:左 1:右 0:停止
    float scrollElapsedTime_ = 0.0f; // スクロールアニメーション経過時間
    float scrollDuration_ = 0.3f; // スクロールアニメーション時間



    std::shared_ptr<Engine::SoundInstance> bgmSoundInstance_;
    std::shared_ptr<Engine::VoiceInstance> voiceInstance_;

    std::unique_ptr<Engine::AnimationSequence> entranceSequence_; // titleからの遷移時のアニメーションシーケンス
    bool isEntranceAnimPlaying_ = true;
    float entranceAnimTime_ = 0.0f;

    std::unique_ptr<Engine::JsonBinder> jsonBinder_;
};
