#include "BitcrusherEffect.h"

#include <cmath>

namespace
{
const XAPO_REGISTRATION_PROPERTIES kRegProps = {
    __uuidof(BitcrusherEffectTag),
    L"BitcrusherEffect",
    L"Engine",
    1,0,
    XAPO_FLAG_INPLACE_SUPPORTED | XAPO_FLAG_INPLACE_REQUIRED,
    1,1,1,1
};
}

HRESULT BitcrusherEffect::Create(IUnknown** effect)
{
    if (!effect)
        return E_POINTER;
    auto* p = new (std::nothrow) BitcrusherEffect(&kRegProps);
    if (!p)
        return E_OUTOFMEMORY;

    HRESULT hr = p->Initialize(&kRegProps, sizeof(kRegProps));
    if (FAILED(hr))
    {
        delete p;
        return hr;
    }

    *effect = static_cast<IUnknown*>(static_cast<CXAPOBase*>(p));
    (*effect)->AddRef();

    return S_OK;

}

STDMETHODIMP_(void __stdcall) BitcrusherEffect::Process(UINT32 inputCount, const XAPO_PROCESS_BUFFER_PARAMETERS* input, UINT32 outputCount, XAPO_PROCESS_BUFFER_PARAMETERS* output, BOOL isEnabled)
{
    (void)inputCount;
    (void)outputCount;

    if (!input || !output)
        return;

    if (!isEnabled )
    {
        PassThrough(input, output);
        return;
    }

    const float* src = static_cast<const float*>(input[0].pBuffer);
    float* dst = static_cast<float*>(output[0].pBuffer);
    float step = step_.load();
    float dryWet = dryWet_.load();

    float hold = 0.0f; // 前回のサンプル値を保持する変数

    // ビットクラッシュ処理
    int iStep = 1 + static_cast<int>(31.0f * dryWet);

    for (uint32_t i = 0; i < input[0].ValidFrameCount * channels_; ++i)
    {
        if (i % iStep == 0)
        {
            hold = src[i];
        }

        dst[i] = std::floor(hold * step * 0.5f) / step;
    }
}

void BitcrusherEffect::SetBitDepth(float normalized)
{
    float newStep = 1.0f + static_cast<float>(normalized) * 23.0f;
    step_.store(newStep);
}
