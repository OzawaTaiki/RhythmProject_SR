#pragma once

#include <System/Audio/AudioEffectBase.h>
#include <atomic>


struct __declspec(uuid("87654321-4321-4321-4321-CBA987654321")) BitcrusherEffectTag {};

class BitcrusherEffect : public Engine::AudioEffectBase
{
public:

    static HRESULT Create(IUnknown** effect);

    STDMETHOD_(void, Process)(
        UINT32 inputCount,
        const XAPO_PROCESS_BUFFER_PARAMETERS* input,
        UINT32 outputCount,
        XAPO_PROCESS_BUFFER_PARAMETERS* output,
        BOOL isEnabled) override;

    /// <summary>
    /// ビット深度を設定する。
    /// normalizedは0.0fから1.0fの範囲で、ビット深度は2ビットから16ビット相当になるようにマッピングされる。
    /// </summary>
    /// <param name="normalized">正規化ビット深度</param>
    void SetBitDepth(float normalized);

    /// <summary>
    /// ドライ/ウェット比を設定する。
    /// 0.0fで完全にドライ、1.0fで完全にウェットになる。
    /// </summary>
    /// <param name="dryWet"></param>
    void SetDryWet(float dryWet) { dryWet_.store(dryWet); }

private:

    BitcrusherEffect(const XAPO_REGISTRATION_PROPERTIES* pRegProps) : AudioEffectBase(pRegProps) {}
    ~BitcrusherEffect() override =default;


private:

    std::atomic<float> step_{ 1.f / 256.0f }; // = 1 / 2^bitDepth
    std::atomic<float> dryWet_{ 0.0f };      // 初期は無効
};