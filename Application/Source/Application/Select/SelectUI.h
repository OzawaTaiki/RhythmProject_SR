#pragma once

#include "musicSelectUI.h"

/// <summary>
/// 選曲画面のUIクラス
/// </summary>
// 選曲UI，設定ボタン，楽曲スピードのUIなどを管理
class SelectUI
{
public:
    SelectUI() = default;
    ~SelectUI() = default;

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

    std::unique_ptr<MusicSelectUI> musicSelectUI_;


};

