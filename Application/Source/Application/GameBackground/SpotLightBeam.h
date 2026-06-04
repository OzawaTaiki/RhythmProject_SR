#pragma once

#include <Math/Vector/Vector3.h>
#include <Math/Vector/Vector4.h>

#include <d3d12.h>
#include <memory>
#include <string>

namespace Engine
{
class Camera;
class ObjectModel;
class SpotLightComponent;
}

/// <summary>
/// スポットライト1基分の「照明（SpotLightComponent）＋可視光線（円錐メッシュ）」を束ねるクラス。
/// ドロップ検出時に Burst() で一時的に輝度を跳ね上げる。
/// </summary>
class SpotLightBeam
{
public:
    SpotLightBeam() = default;
    ~SpotLightBeam() = default;

    /// <summary>
    /// 初期化。position から target へ向かう光線を生成する。
    /// </summary>
    void Initialize(const std::string& name,
                    const Engine::Vector3& position,
                    const Engine::Vector3& target,
                    const Engine::Vector4& color);

    /// <summary>毎フレーム更新（バースト減衰・ライト/光線への反映）。</summary>
    void Update(float deltaTime);

    /// <summary>可視光線を描画する（加算ブレンドの専用PSOを渡す）。</summary>
    void Draw(const Engine::Camera* camera, ID3D12PipelineState* beamPso);

    /// <summary>ドロップ検出時に呼ぶ。輝度を一時的に底上げする。</summary>
    void Burst(float amount);

    void ShiftHue(float deg) { hue_ += deg; }
    void SetEnergy(float energy01) { energyTarget_ = energy01; }
    void SetBaseline(float baseline01) { baseline_ = baseline01; }

    /// <summary>左右スイングを設定する。amplitudeDeg=振れ角(度)、speed=初期角速度、phaseOffset=開始位相(rad)。</summary>
    void SetSwing(float amplitudeDeg, float speed, float phaseOffset = 0.0f)
    {
        swingAmplitudeDeg_ = amplitudeDeg;
        swingSpeed_ = speed;
        swingPhaseOffset_ = phaseOffset;
    }

    /// <summary>BPMを設定。スイング速度を拍に同期させる。</summary>
    void SetBPM(float bpm);

    void SetThickness(float pulseAmount) { thicknessPulse_ = pulseAmount; }
    void SetDynamicColor(bool enable) { useDynamicColor_ = enable; }
    void SetHueCycle(float speed, float startHueDeg) { hueCycleSpeed_ = speed; hue_ = startHueDeg; }
    void SetDominantBand(float band01) { dominantBand_ = band01; }
    void SetComboRatio(float ratio01) { comboRatio_ = ratio01; }
    void SetColor(const Engine::Vector4& color) { color_ = color; }

private:
    std::shared_ptr<Engine::SpotLightComponent> light_;
    std::unique_ptr<Engine::ObjectModel> beam_;

    Engine::Vector3 position_{};
    Engine::Vector3 baseDirection_{ 0.0f, -1.0f, 0.0f }; // スイング前の基準向き
    Engine::Vector3 direction_{ 0.0f, -1.0f, 0.0f };     // スイング適用後の現在向き
    Engine::Vector4 color_{ 1.0f, 1.0f, 1.0f, 1.0f };

    // 輝度制御
    float baseIntensity_ = 0.3f;   // 通常時のライト輝度
    float beamBrightnessScale_ = 0.45f; // 可視光線の明るさ減衰（主張を抑える）
    float burstLevel_ = 0.0f;      // 0..1 ドロップで跳ね、減衰する
    float burstDecay_ = 2.0f;      // 1秒あたりの減衰量

    // 駆動値（A=音圧RMS / C=高域エネルギー）由来の明るさ
    float energyTarget_ = 0.0f;    // 毎フレーム外部から与えられる目標値
    float energyLevel_ = 0.0f;     // 平滑化された現在値
    float energyResponse_ = 8.0f;  // 追従の速さ（大きいほど機敏）

    float baseline_ = 0.0f;        // 常時の最低輝度（Aは正、Cは0）

    // 左右スイング（A用）
    float swingAmplitudeDeg_ = 0.0f; // 振れ角（度）。0でスイングなし
    float swingSpeed_ = 1.0f;        // 基本波の角速度（BPMから計算）
    float swingPhase_ = 0.0f;        // 基本波の内部位相
    float swingPhase2_ = 0.0f;       // 複合波（2本目）の位相
    float swingPhaseOffset_ = 0.0f;  // ビームごとの位相オフセット（初期化時に設定）

    // 太さ脈動
    float thicknessPulse_ = 0.0f;    // energyで太さが増える量（0で固定）
    Engine::Vector3 baseBeamScale_{ 1.0f, 1.0f, 1.0f }; // 初期スケール

    // 動的カラー
    bool  useDynamicColor_ = false;  // 動的カラーを使うか
    float hueCycleSpeed_ = 0.0f;     // 色相サイクル速度（度/秒）
    float hue_ = 0.0f;               // 時間サイクルの現在色相（度）
    float dominantBand_ = 0.0f;      // 支配帯域(0..1)。色相に加算寄与
    float comboRatio_ = 0.0f;        // コンボ比率(0..1)。彩度の上限を開放

    float length_ = 18.0f;         // 光線（円錐）の長さ
};
