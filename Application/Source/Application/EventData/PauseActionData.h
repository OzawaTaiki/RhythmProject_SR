#pragma once
#include <Features/Event/EventData.h>

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

struct PauseActionData : EventData
{
    PauseActions pauseAction = PauseActions::None;

    PauseActionData(PauseActions _action) : pauseAction(_action) {}
};