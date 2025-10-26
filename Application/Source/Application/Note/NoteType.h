#pragma once

// ノーツの種類
/// <summary>
/// ノーツの種類を表す列挙型。
/// </summary>
enum class NoteType
{
    Normal,
    Hold,
    HoldEnd,

    MAX
};