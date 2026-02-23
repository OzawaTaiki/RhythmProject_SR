#include "JudgeColor.h"
#include <Math/MyLib.h>

using namespace Engine;

Judge::ColorPair Judge::GetColor(JudgeType type)
{
    switch (type)
    {
    case JudgeType::Perfect:
        // ピンクから水色
        return { ColorCodeToVector4(0xff66b8ff), Engine::Vector4(0.0f, 1.0f, 1.0f, 1.0f) };

    case JudgeType::Good:
        // 緑から薄い緑
        return { Engine::Vector4(0.0f, 1.0f, 0.0f, 1.0f), Engine::Vector4(0.5f, 1.0f, 0.5f, 1.0f) };

    case JudgeType::Bad:
        // 暗青系から水色系
        return { ColorCodeToVector4(0x3838d0ff), ColorCodeToVector4(0x3ea5ffff) };

    case JudgeType::Miss:
        // グレー
        return { Engine::Vector4(0.5f, 0.5f, 0.5f, 1.0f), Engine::Vector4(0.5f, 0.5f, 0.5f, 1.0f) };

    case JudgeType::None:
    case JudgeType::MAX:
    default:
        return { Engine::Vector4(1.0f, 1.0f, 1.0f, 1.0f), Engine::Vector4(1.0f, 1.0f, 1.0f, 1.0f) };
    }
}
