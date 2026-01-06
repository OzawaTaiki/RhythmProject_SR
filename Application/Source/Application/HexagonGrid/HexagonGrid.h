#pragma once
#include <memory>
#include <Features/UI/ELement/UIButtonElement.h>
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
    float radius_ = 96.0f; // 外接円の半径
    float margin_ =  5.0f; // 六角形間の余白

    Vector2 moveOffset_;
    std::vector<Vector2> initialPositions_; // 各六角形の初期位置

    std::vector<std::unique_ptr<UIButtonElement>> elements_;
};