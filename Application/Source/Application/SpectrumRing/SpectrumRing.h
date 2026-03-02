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
namespace Engine { class Camera; }

class SpectrumRing
{
public:
    SpectrumRing() = default;
    ~SpectrumRing() = default;

    void Initialize(std::shared_ptr<Engine::SoundInstance> musicInstance, size_t numring);
    void Update(float elapsedTime);
    void Draw(Engine::Camera* camera);

    void SetBeatManager(BeatManager* beatManager) { beatManager_ = beatManager; }

    void SetMusicInstance(std::shared_ptr<Engine::SoundInstance> musicInstance);

private:

    void CreateRings();
    void CreateTextureGenerators();

    //void UpdateRings(std::shared_ptr<VoiceInstance> _voice);
    //void UpdateTextureGenerators(const std::vector<float>& _spectrumData, float _rms);


private:

    std::vector<std::unique_ptr<Engine::ObjectModel>> rings_;
    std::vector<std::unique_ptr<Engine::SpectrumTextureGenerator>> textureGenerators_;

    BeatManager* beatManager_;

    std::deque<uint32_t> cycleTextureIndices_;
    std::vector<uint32_t> textureHandles_;
    std::shared_ptr<Engine::SoundInstance> musicInstance_= nullptr;
    Engine::AudioSpectrum audioSpectrum_;
    bool isInitTextures_ = false;
};