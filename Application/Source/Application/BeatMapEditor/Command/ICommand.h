#pragma once

/// <summary>
/// コマンドインターフェース。
/// </summary>
/// <remarks>
/// Command パターンで使用する基本インターフェース。Execute と Undo を実装する。
/// </remarks>
class ICommand 
{

public:
    // デストラクタ
    virtual ~ICommand() = default;

    /// <summary>
    /// コマンドを実行する（Redo としても機能することが期待される）。
    /// </summary>
    virtual void Execute() = 0;

    /// <summary>
    /// コマンドを元に戻す（Undo）。
    /// </summary>
    virtual void Undo() = 0;

};