#pragma once

#include <Features/WaveformDisplay/WaveformDisplay.h>
#include <Math/Matrix/Matrix4x4.h>
#include <Features/Sprite/Sprite.h>

namespace BME
{

class AudioController;
class EditorCoordinate;

/// <summary>
/// 音声波形描画クラス
/// </summary>
class WaveformRenderer
{
public:
    WaveformRenderer() = default;
    ~WaveformRenderer() = default;

    void Initialize( const Engine::Matrix4x4& _matVP);
    void Draw(const AudioController* _audioController, float _currentTime);
    void DrawBackSprite();
    void Finalize();

    void UpdateBounds();

private:
    Engine::WaveformDisplay waveformDisplay_;

    std::unique_ptr<Engine::Sprite> waveformBackground_;
};

} // namespace BME
