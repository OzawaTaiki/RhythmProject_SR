#pragma once

// Engine
#include <Features/LineDrawer/LineDrawer.h>
#include <Math/Vector/Vector4.h>

// 判定ラインクラス
class JudgeLine
{
public:
    JudgeLine();
    ~JudgeLine();

    // 初期化
    void Initialize();
    // 描画
    void Draw();

    // 位置の取得
    float GetPosition() const { return position_; }

private:
    // ラインの計算
    void CalculateLine();
    // デバッグウィンドウ
    void DebugWindow();

private:

    float position_ = -0.0f; // ラインのz位置

    Vector4 color_ = { 0,1,1,1 }; // ラインの色

    LineDrawer* lineDrawer_ = nullptr; // ラインドロワーのポインタ

    std::vector<Vector3> linePoints_;  // ラインの頂点リスト
    bool dirty_ = true;                // ラインの再計算が必要かどうか

};