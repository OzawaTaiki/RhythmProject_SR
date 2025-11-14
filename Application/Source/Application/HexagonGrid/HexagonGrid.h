#pragma once
#include <memory>
#include <Features/UI/UIGroup.h>
#include <Math/Rect/Rect.h>


class HexagonGrid
{
public:
    HexagonGrid() = default;
    ~HexagonGrid() = default;

    void Initialize(const Rect& area);
    void Update();
    void Draw();


private:

    void GetHexagonLocalVertices(std::vector<Vector2>& outVertices) const;

    void GenerateHexagonGrid(const Rect& area);

    Vector2 GetHexagonCenter(int32_t row, int32_t col) const;


    void ImGui();
private:
    float radius_ = 128.0f; // 外接円の半径
    float margin_ =  20.0f; // 六角形間の余白

    std::unique_ptr<UIGroup> uiGroup_;
};