#pragma once

#include <Features/PostEffects/Vignette.h>

#include <memory>

// ミス時のビネットエフェクト
/// <summary>
/// ミス時に画面にビネット（暗転）エフェクトを適用するクラス。
/// </summary>
class MissedVignette
{
public:
    /// <summary>
    /// デフォルトコンストラクタ
    /// </summary>
    MissedVignette() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~MissedVignette() = default;

    /// <summary>
    /// 初期化処理。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    /// <param name="_deltaTime">経過時間（秒）</param>
    void Update(float _deltaTime);

    /// <summary>
    /// 指定レンダーターゲットに対してビネットエフェクトを適用する。
    /// </summary>
    /// <param name="_input">入力レンダーターゲット名</param>
    /// <param name="_output">出力レンダーターゲット名</param>
    void ApplyEffect(const std::string& _input, const std::string& _output);

    /// <summary>
    /// エフェクトを発動させる。
    /// </summary>
    void Emit();
private:

    bool emit_ = false; // エフェクト発動フラグ

    float duration_ = 0.5f; // エフェクトの持続時間
    float elapsedTime_ = 0.0f; // 経過時間

    std::unique_ptr<Vignette> vignette_ = nullptr;
    VignetteData currentVignetteData_ = {};

    VignetteData startVignetteData_ = {};

};