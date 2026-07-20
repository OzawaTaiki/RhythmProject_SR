#pragma once

#include <Framework/Framework.h>

#include <Application/Setting/Setting.h>

/// <summary>
/// アプリケーション固有のフレームワーク実装クラス。
/// Engine::Framework を継承し、Initialize/Update/Draw/Finalize をオーバーライドしてゲーム全体のループを管理する。
/// </summary>
class SampleFramework : public Engine::Framework
{
public:
    SampleFramework() = default;
    ~SampleFramework() = default;

    void Initialize(const std::wstring& _winTitle) override;
    void Update() override;
    void Draw() override;

    void Finalize() override;

private:

    void GenerateModels();

};
