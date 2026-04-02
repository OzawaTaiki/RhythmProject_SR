
#include "Resources/shader/Complex.hlsli"

cbuffer Constants : register(b0)
{
    uint fftSize;           // FFTのサイズ (2^n)
    uint cyrrentStage;      // 現在のステージ (0 から bits-1)
    uint isInverse;         // 逆変換フラグ     TODO : 実装
    uint padding;
}

StructuredBuffer<uint> bitReversalIndex : register(t0); // ビット反転インデックステーブル

RWStructuredBuffer<Complex> input : register(u0); // 入力データ
RWStructuredBuffer<Complex> output : register(u1); // 出力データ
//ピンポンバッファ

static const float PI = 3.14159265358979323846f;

[numthreads(256,1,1)]
void ButterflyCS(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    if (index >= fftSize)
    {
        return;
    }

    uint butterfylSize = 1u << (cyrrentStage + 1);   // 2^(s + 1)
    uint butterfylHalf = butterfylSize >> 1; // 2^s

    // このスレッドが処理するバタフライのグループとオフセット
    uint groupIndex = index / butterfylHalf;
    uint groupOffset = index % butterfylHalf;
    uint baseIndex = groupIndex * butterfylSize + groupOffset;

    // 演算のペアインデックス
    uint idx0 = baseIndex;
    uint idx1 = baseIndex + butterfylHalf;

    // 回転因子の計算
    float angle = -2.0f * PI * float(groupOffset) / float(butterfylSize);
    Complex w = { cos(angle), sin(angle) };

    Complex t = w.Mul(input[bitReversalIndex[idx1]]);
    Complex u = input[bitReversalIndex[idx0]];

    output[idx0] = u.Add(t);
    output[idx1] = u.Sub(t);
}
