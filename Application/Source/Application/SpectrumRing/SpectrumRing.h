#pragma once

#include <Features/Model/ObjectModel.h>
#include <Features/Model/Primitive/Ring.h>
#include <Features/AudioSpectrum/SpectrumTextureGenerator.h>
#include <System/Audio/SoundInstance.h>
#include <System/Audio/VoiceInstance.h>



#include <vector>
#include <deque>
#include <memory>
#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <Application/BeatsManager/BeatManager.h>

// 前方宣言
class Camera;

class SpectrumRing
{
public:
    SpectrumRing() = default;
    ~SpectrumRing() = default;

    void Initialize(std::shared_ptr<SoundInstance> _musicInstance, size_t _numring);
    void Update(float _elapsedTime);
    void Draw(class Camera* camera);

    void SetBeatManager(BeatManager* _beatManager) { beatManager_ = _beatManager; }

private:

    void CreateRings();
    void CreateTextureGenerators();

    //void UpdateRings(std::shared_ptr<VoiceInstance> _voice);
    //void UpdateTextureGenerators(const std::vector<float>& _spectrumData, float _rms);


private:

    std::vector<std::unique_ptr<ObjectModel>> rings_;
    std::vector<std::unique_ptr<SpectrumTextureGenerator>> textureGenerators_;

    BeatManager* beatManager_;

    std::deque<uint32_t> cycleTextureIndices_;
    std::vector<uint32_t> textureHandles_;
    std::shared_ptr<SoundInstance> musicInstance_= nullptr;
    AudioSpectrum audioSpectrum_;
    bool isInitTextures_ = false;
};