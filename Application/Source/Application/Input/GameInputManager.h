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
    /// <param name="_input">Input のインスタンス</param>
    void Initialize(Input* _input);

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    void Update();

    /// <summary>
    /// 入力データを取得する。
    /// </summary>
    /// <returns>入力データのベクター</returns>
    const std::vector<InputDate>& GetInputData() const { return inputData_; }

    /// <summary>
    /// キーバインドの設定（マップで一括設定）。
    /// </summary>
    /// <param name="_keyBindings">キーとレーンのバインディングのマップ</param>
    void SetKeyBinding(std::map<int8_t, int32_t> _keyBindings) { keyBindings_ = _keyBindings; }

    /// <summary>
    /// 個別のキーとレーンのバインディングを設定する。
    /// </summary>
    /// <param name="_key">キーコード (DIK_A など)</param>
    /// <param name="_lane">レーン番号</param>
    void SetKeyBinding(int8_t _key, int32_t _lane) { keyBindings_[_key] = _lane; }

    /// <summary>
    /// 音楽のボイスインスタンスを設定する。
    /// </summary>
    /// <param name="_voiceInstance">音声インスタンスの共有ポインタ</param>
    void SetMusicVoiceInstance(std::shared_ptr<VoiceInstance> _voiceInstance) { musicVoiceInstance_ = _voiceInstance; }

    /// <summary>
    /// ゲーム音楽の情報を設定する。
    /// </summary>
    void SetGameMusic(const GameMusic* _gameMusic) { gameMusic_ = _gameMusic; }

private:
    /// <summary>
    /// デフォルトのキー設定を行う。
    /// </summary>
    void SetDefaultKeyBindings();

private:
    Input* input_; // Inputのインスタンス

    std::vector<InputDate> inputData_; // キーの状態を保持するベクター

    std::map<int8_t, int32_t> keyBindings_; // キーのバインディング

    std::weak_ptr<VoiceInstance> musicVoiceInstance_; // 音楽の音声インスタンス

    const GameMusic* gameMusic_ = nullptr; // ゲーム音楽のインスタンス
};