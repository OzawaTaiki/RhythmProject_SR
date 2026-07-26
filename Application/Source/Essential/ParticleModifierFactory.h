#pragma once

#include <Features/Effect/Modifier/IPaticleMoifierFactory.h>

/// <summary>
/// 名前に対応するアプリケーション固有のパーティクルモディファイアを生成するファクトリクラス。
/// </summary>
class ParticleModifierFactory : public Engine::IParticleMoifierFactory
{

public:
    ParticleModifierFactory();
    ~ParticleModifierFactory() override = default;

    std::unique_ptr<Engine::ParticleModifier> CreateModifier(const std::string& _name) override;

private:
    std::map<std::string, std::function<std::unique_ptr<Engine::ParticleModifier>()>> modifierCreators_;
};
