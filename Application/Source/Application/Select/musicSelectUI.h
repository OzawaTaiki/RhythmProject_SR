#pragma once

#include <Features/UI/Element/UIButtonElement.h>
#include <Features/Animation/Sequence/AnimationSequence.h>

/// <summary>
/// 選曲UIクラス
/// </summary>
class MusicSelectUI
{
    // 画面外に置いてある大きな円の円周上になるような配置。
    // 円は画面下方向に配置
    // 北半球が出ているような状態になる
    // 画面下部にはスペクトラムはおき，上部には楽曲の情報を出そうと思う
public:
    MusicSelectUI() = default;
    ~MusicSelectUI() = default;
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();

private:

    /// <summary>
    /// Jsonバインダー初期化
    /// </summary>
    void InitJsonBinder();

    /// <summary>
    /// レイアウト更新
    /// </summary>
    void UpdateLayout();

    /// <summary>
    /// スケーリング更新
    /// </summary>
    void UpdateScaling();

    /// <summary>
    /// UIアイテム数が最低限になるように追加する
    /// </summary>
    void EnsureMinimumItems();

    /// <summary>
    /// 選択インデックスを範囲内に収める
    /// </summary>
    void ClampSelectedIndex();
private:

    // UI配置用の大きな円情報
    struct LayoutCircle
    {
        Vector2 center;
        float radius;

        float startAngle;
        float endAngle;
    };

    LayoutCircle layoutCircle_;

    // 円周上のUIアイテム情報
    struct UIItemWithAngle
    {
        std::unique_ptr<UIButtonElement> item;
        float angle = 0.0f;
    };

    std::vector<UIItemWithAngle> uiItems_;

    float marginAngle_ = 0.2f; // 円周上のUIアイテム間の角度の余白
    float scrollTime_ = 0.3f; // スクロールアニメーション時間

    Vector2 BaseUISize_ ={};

    int32_t selectedIndex_ = 0;

    //std::unique_ptr<AnimationSequence> openSequence_;

    std::unique_ptr<JsonBinder> jsonBinder_;
};
