#include "Resources/Shader/Object3d.hlsli"

cbuffer gMaterial : register(b1)
{
    float4x4 unTransform;
    float4 deffuseColor;
    float shininess;
    int enableLighting;
    int hasTexture;
    float envScale;
    int enableEnviroment;
    float3 pad;
};

cbuffer gColor : register(b2)
{
    float4 materialColor;
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput _input)
{
    PixelShaderOutput output;

    // Cylinderは上側texcoord.y=0(先端)・下側texcoord.y=1(根元)
    // 根元を明るく、先端を透明にフェードする
    float v = _input.texcoord.y;
    float fade = saturate(v); // 0(先端) → 1(根元)

    float4 col = deffuseColor * materialColor;

    // 加算ブレンド前提：rgb強度で見せる
    output.color.rgb = col.rgb * fade;
    output.color.a = col.a * fade;

    return output;
}
