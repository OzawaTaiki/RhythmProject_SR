#pragma once

#include <Math/Vector/Vector4.h>
#include <Application/Note/Judge/JudgeType.h>

namespace Judge {

/// <summary>
/// ColorPairを表す構造体。
/// </summary>
struct ColorPair {
    Engine::Vector4 top;
    Engine::Vector4 bottom;
};

ColorPair GetColor(JudgeType type);

} // namespace Judge
