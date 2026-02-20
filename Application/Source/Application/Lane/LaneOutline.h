#pragma once

#include <Features/PostEffects/DepthBasedOutLine.h>
#include <Math/Color/Color.h>
#include <memory>
#include <Features/Json/JsonBinder.h>

namespace Engine { class Camera; }
class ComboThresholds;

class LaneOutline
{
public:
    LaneOutline() = default;
    ~LaneOutline() = default;
    void Initialize(Engine::Camera* camera);
    void Update(int32_t combo);
    void Apply(const std::string& input, const std::string& output);

    void SetComboThresholds(const ComboThresholds* comboThresholds) { comboThresholds_ = comboThresholds; }
private:
    std::unique_ptr<Engine::DepthBasedOutLine> depthBasedOutLine_ = nullptr;
    Engine::DepthBasedOutLineData depthBasedOutLineData_ = {};

    Engine::Vector2 intensityRange_ = { 0.7f, 1.0f };
    std::vector<Engine::HSVA> edgeColors_={};

    std::unique_ptr<Engine::JsonBinder> jsonBinder_ = nullptr;

    const ComboThresholds* comboThresholds_;
};