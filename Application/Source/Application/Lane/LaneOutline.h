#pragma once

#include <Features/PostEffects/DepthBasedOutLine.h>
#include <Math/Color/Color.h>
#include <memory>
#include <Features/Json/JsonBinder.h>

class ComboThresholds;

class LaneOutline
{
public:
    LaneOutline() = default;
    ~LaneOutline() = default;
    void Initialize(Camera* camera);
    void Update(int32_t combo);
    void Apply(const std::string& input, const std::string& output);

    void SetComboThresholds(const ComboThresholds* comboThresholds) { comboThresholds_ = comboThresholds; }
private:
    std::unique_ptr<DepthBasedOutLine> depthBasedOutLine_ = nullptr;
    DepthBasedOutLineData depthBasedOutLineData_ = {};

    Vector2 intensityRange_ = { 0.7f, 1.0f };
    std::vector<HSVA> edgeColors_={};

    std::unique_ptr<JsonBinder> jsonBinder_ = nullptr;

    const ComboThresholds* comboThresholds_;
};