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

    void Initialize(const EditorCoordinate* _coordinate, const Matrix4x4& _matVP);
    void Draw(const AudioController* _audioController, float _currentTime);
    void DrawBackSprite();
    void Finalize();

    void SetViewProjection(const Matrix4x4& _matVP);
    void UpdateBounds(const EditorCoordinate* _coordinate);

private:
    WaveformDisplay waveformDisplay_;

    std::unique_ptr<Sprite> waveformBackground_;
};

} // namespace BME
