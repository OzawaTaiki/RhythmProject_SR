#pragma once
#include <System/Audio/VoiceInstance.h>
#include <System/Input/Input.h>

#include <Application/Input/InputData.h>
#include <Application/GameMusic/GameMusic.h>

#include <map>
#include <cstdint>
#include <memory>
#include <stdexcept>

/// <summary>
/// アプリ用の入力管理クラス。
/// </summary>
class GameInputManager
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    GameInputManager() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GameInputManager() = default;

    /// <summary>
    /// 初期化処理を行う。
    /// </summary>
    /// <param name="input">Input のインスタンス</param>
    void Initialize(Input* input);

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    void Update();

    /// <summary>
    /// 入力データを取得する。
    /// </summary>
    /// <returns>入力データのベクター</returns>
    const std::vector<InputData>& GetInputData() const { return inputData_; }

    /// <summary>
    /// キーバインドの設定（マップで一括設定）。
    /// </summary>
    /// <param name="keyBindings">キーとレーンのバインディングのマップ</param>
    void SetKeyBinding(std::map<int32_t,uint8_t> keyBindings);

    /// <summary>
    /// 個別のキーとレーンのバインディングを設定する。
    /// </summary>
    /// <param name="key">キーコード (DIK_A など)</param>
    /// <param name="lane">レーン番号</param>
    void SetKeyBinding(uint8_t key, int32_t lane);

    /// <summary>
    /// 音楽のボイスインスタンスを設定する。
    /// </summary>
    /// <param name="voiceInstance">音声インスタンスの共有ポインタ</param>
    void SetMusicVoiceInstance(std::shared_ptr<VoiceInstance> voiceInstance) { musicVoiceInstance_ = voiceInstance; }

    /// <summary>
    /// ゲーム音楽の情報を設定する。
    /// </summary>
    void SetGameMusic(const GameMusic* gameMusic) { gameMusic_ = gameMusic; }

    /// <summary>
    /// キーのバインディングを取得する。
    /// </summary>
    const std::map<int32_t, uint8_t>& GetKeyBinds()const { return keyBindings_; }

private:
    /// <summary>
    /// デフォルトのキー設定を行う。
    /// </summary>
    void SetDefaultKeyBindings();

private:
    Input* input_; // Inputのインスタンス

    std::vector<InputData> inputData_; // キーの状態を保持するベクター

    std::map<int32_t, uint8_t> keyBindings_; // キーのバインディング

    std::weak_ptr<VoiceInstance> musicVoiceInstance_; // 音楽の音声インスタンス

    const GameMusic* gameMusic_ = nullptr; // ゲーム音楽のインスタンス
};