#pragma once
#include <Features/Event/EventData.h>

/// <summary>
/// ポーズメニューでのアクションを表す列挙型。
/// </summary>
enum class PauseActions
{
    None,
    Open,
    Close,

    Resume,
    Retry,
    ToTitle,

    Max
};

/// <summary>
/// ポーズアクションを表すイベントデータ。
/// </summary>
struct PauseActionData : EventData
{
    PauseActions pauseAction = PauseActions::None;

    PauseActionData(PauseActions _action) : pauseAction(_action) {}
};