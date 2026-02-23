#pragma once

#include <Math/Vector/Vector4.h>
#include <Application/Note/Judge/JudgeType.h>

namespace Judge {

struct ColorPair {
    Engine::Vector4 top;
    Engine::Vector4 bottom;
};

ColorPair GetColor(JudgeType type);

} // namespace Judge
