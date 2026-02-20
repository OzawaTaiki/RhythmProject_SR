#include "SpectrumRing.h"

#include <Features/AudioSpectrum/AudioSpectrum.h>
#include <Features/WaveformDisplay/WaveformAnalyzer.h>
#include <Math/MyLib.h>

using namespace Engine;

namespace
{
float innerRadius = 1.0f;
float outerRadius = 3.0f;

size_t numRings = 5;
Vector4 clearColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

int32_t fftsize = 1024;
int32_t barCount = 48;
float minHz = 20.0f;
float maxHz = 10000.0f;

}

void SpectrumRing::Initialize(std::shared_ptr<SoundInstance> musicInstance,size_t numring)
{
    musicInstance_ = musicInstance;
    numRings = numring;
    audioSpectrum_ = AudioSpectrum();

    audioSpectrum_.SetAudioData(musicInstance_->GetAudioData());
    audioSpectrum_.SetSampleRate(musicInstance_->GetSampleRate());

    CreateRings();
    CreateTextureGenerators();
}

void SpectrumRing::Update(float elapsedTime)
{
    if (!isInitTextures_)
    {
        auto spectrum = audioSpectrum_.GetSpectrumAtTime(0.0f);
        // 荳蠎ｦ縺吶∋縺ｦ縺ｮ繝・け繧ｹ繝√Ε繧堤函謌舌☆繧・
        // init縺ｧ縺ｯ縺ｧ縺阪↑縺・∩縺溘＞縺ｪ縺ｮ縺ｧ縺励°縺溘↑縺・
        for (auto& generator : textureGenerators_)
        {
            generator->Generate(spectrum, 0.0f, barCount);
        }
        isInitTextures_ = true;
    }

    // 繝・け繧ｹ繝√Ε縺ｮ逕滓・
    // 豈弱ヵ繝ｬ繝ｼ繝1縺､縺縺第眠縺励＞繝・・繧ｿ縺ｧ譖ｴ譁ｰ縺吶ｋ
    // 蜿､縺・ユ繧ｯ繧ｹ繝√Ε縺ｯ菴ｿ縺・∪繧上☆
    uint32_t index = cycleTextureIndices_.back();
    cycleTextureIndices_.pop_back();
    cycleTextureIndices_.push_front(index);
    
    auto spectrum = audioSpectrum_.GetSpectrumAtTime(elapsedTime);
    //float min, max;
    //WaveformAnalyzer::GetRawWaveformMaxMin(musicInstance_.get(), _elapsedTime, 5.0f, max, min);
    float scale =
        //(std::abs(max) + std::abs(min)) / 2.0f;
        WaveformAnalyzer::GetRMSAtTime(musicInstance_.get(),elapsedTime);
    textureGenerators_[index]->Generate(spectrum, scale , barCount);
    uint32_t nextIndex = cycleTextureIndices_.back();
    textureGenerators_[nextIndex]->ReserveClear();

    // 譽偵・菴咲ｽｮ縺ｯ螟峨ｏ繧峨↑縺・rot騾溷ｺｦ縺ｯ荳螳夐俣髫・
    // 1邂・園蠖薙◆繧贋ｸ画悽
    // 1縺ｯ譛譁ｰ 2・・縺ｯ驕主悉
    // 驕主悉縺ｯ蠕舌・↓遏ｭ縺上↑繧・
    // 驕主悉縺ｯ豈弱ヵ繝ｬ繝ｼ繝縺ｯ譖ｴ譁ｰ縺輔ｌ縺ｪ縺・
    // 縺倥ｃ縺ゅ←縺ｮ繧ｿ繧､繝溘Φ繧ｰ縺ｧ譖ｴ譁ｰ縺吶ｋ縺・
    // TODO : 繧上°繧峨ｓ
    //

     //TODO : 繝ｪ繝ｳ繧ｰ縺ｮ蝗櫁ｻ｢
    const float rotationSpeedPerFrame = std::numbers::pi_v<float> *2.0f / static_cast<float>(barCount * 3);
    rings_[0]->euler_.z += rotationSpeedPerFrame;
    for (size_t i = 1; i < rings_.size(); ++i)
    {
        rings_[i]->euler_.z = rings_[i - 1]->euler_.z + (rotationSpeedPerFrame * static_cast<float>(barCount * 3 / numRings));
    }

    for (auto& ring : rings_)
    {
        ring->Update();
    }
}

void SpectrumRing::Draw(Camera* camera)
{
    for (size_t i = 0; i < numRings; ++i)
    {
        uint32_t handleIndex = cycleTextureIndices_[i];
        rings_[i]->Draw(camera, textureHandles_[handleIndex], Vector4(1, 1, 1, 0.3f));
    }    rings_[numRings]->Draw(camera);
}

void SpectrumRing::CreateRings()
{
    float outerRadiusStep = (outerRadius - innerRadius) / static_cast<float>(numRings);

    for (size_t i = 0; i < numRings; ++i)
    {
        // 螟ｧ縺阪＞鬆・〒逕滓・縺励※縺・￥
        Ring priRing(innerRadius, outerRadius - outerRadiusStep * static_cast<float>(i));
        priRing.SetDivide(256); // 螟ｧ縺阪ａ縺ｫ縺励↑縺・→繝・け繧ｹ繝√Ε蟠ｩ繧後ｋ

        std::string name = "spectrumRing_" + std::to_string(i);
        auto model = std::make_unique<ObjectModel>(name);
        model->Initialize(priRing.Generate(name));
        model->GetMaterial()->SetEnableLighting(false);
        model->GetMaterial()->GetUVTransform().SetUScale(3.0f);
        rings_.emplace_back(std::move(model));
    }

    //  荳ｭ縺ｮ繝ｪ繝ｳ繧ｰ
    Ring innerRing(0.0f, innerRadius);
    innerRing.SetDivide(256);
    auto innerModel = std::make_unique<ObjectModel>("spectrumInnerRing");
    innerModel->Initialize(innerRing.Generate("spectrumInnerRing"));
    innerModel->GetMaterial()->SetEnableLighting(false);
    rings_.emplace_back(std::move(innerModel));
}

void SpectrumRing::CreateTextureGenerators()
{
    textureGenerators_.resize(numRings);

    for (auto& generator : textureGenerators_)
    {
        generator = std::make_unique<SpectrumTextureGenerator>();
        generator->Initialize(clearColor);
        generator->MakeLogRanges(
            musicInstance_->GetSampleRate(),
            minHz,
            maxHz,
            barCount,
            fftsize / 2,
            fftsize
        );

        textureHandles_.push_back(generator->GetTextureHandle());
    }

    for (uint32_t i = 0; i < numRings; ++i)
    {
        cycleTextureIndices_.push_back(i);// 4 3 2 1 0
    }

}
