#pragma once

#include <Features/UI/Element/UIElement.h>

class UISpriteRenderComponent;

enum class VertexType
{
    LeftTop,
    RightTop,
    RightBottom,
    LeftBottom,
    Max
};
struct OutlineTracer
{
    float progress;
    float totalEdgeLength;

    std::vector<Engine::Vector2> vertices;
};

class FocusFrame
{
public:
    FocusFrame() = default;
    ~FocusFrame() = default;

    void Initialize();
    void Update(float deltaTime);
    void Draw();

    void ChangeTarget(Engine::UIElement* target);

    void RegisterTargetName(const Engine::UIElement* target);

private:
    void ClearProgress();
    void InitEdgesParams(size_t number, OutlineTracer& edge);
    Engine::Vector2 CalcPointOnEdge(const OutlineTracer& edge, float distanceAlongEdge);
    Engine::Vector2 CalcTangentOnEdge(const OutlineTracer& edge, float distanceAlongEdge);
    Engine::Vector2 CalcPerpendicularOnEdge(const OutlineTracer& edge, float distanceAlongEdge);
    VertexType GetVertexTypeFromProgress(float t, int32_t index);
    bool IsCrossingVertex(const OutlineTracer& edge, float progress0, float progress1);
    Engine::Vector2 GetCrossedVertex(const OutlineTracer& edge, float progress0, float progress1);
    void UpdateTrackerLine(const OutlineTracer& tracer, const Engine::Vector2& targetPos,
                           float progress0, float progress1,
                           UISpriteRenderComponent* spriteComponent0, UISpriteRenderComponent* spriteComponent1, float width);
private:

    Engine::UIElement* targetElement_ = nullptr;


    std::map<std::string, size_t> edgeNameToIndexMap_;
    std::array<std::unique_ptr<Engine::UIElement>, 4> focusFrames_;
    std::vector<OutlineTracer> tracers_;

    // 一周にかかる時間
    float cycleTime_ = 3.0f;
    float currentTime_ = 0.0f;
    float trailLength_ = 0.2f;

    std::unique_ptr<Engine::JsonBinder> jsonBinder_;
};
