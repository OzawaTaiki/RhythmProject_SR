#include "ParticleModifierFactory.h"

#include <Features/Effect/Modifier/Preset/RotationBasedMovementModifier.h>
#include <Features/Effect/Modifier/Preset/AlphaOverLifetime.h>
#include <Features/Effect/Modifier/Preset/DecelerationModifier.h>

#include <Application/Effects/TapEffects/HitparticleModifier.h>
#include <Application/Effects/TapEffects/LightPillarModifier.h>

#include <Application/Effects/Speaker/SpeakerEffectModifier.h>

#include <Application/FeedBack/LaneEdgePillar/LaneEdgeEffect.h>


ParticleModifierFactory::ParticleModifierFactory()
{
    // モディファイアの登録
    modifierCreators_["DecelerationModifier"]       = []() { return std::make_unique<DecelerationModifier>(); };
    modifierCreators_["AlphaOverLifetime"]          = []() { return std::make_unique<AlphaOverLifetime>(); };
    modifierCreators_["HitParticleModifier"]        = []() { return std::make_unique<HitParticleModifier>(); };
    modifierCreators_["HitCircleParticleModifier"]  = []() { return std::make_unique<HitCircleParticleModifier>(); };
    modifierCreators_["RotationBasedMovementModifier"] = []() { return std::make_unique<RotationBasedMovementModifier>(); };
    modifierCreators_["SpeakerRingModifier"]        = []() { return std::make_unique<SpeakerRingModifier>(); };
    modifierCreators_["SpeakerParticleModifier"]    = []() { return std::make_unique<SpeakerParticleModifier>(); };
    modifierCreators_["LaneEdgePillarModifier"] = []() { return std::make_unique<LaneEdgePillarModifier>(); };
    modifierCreators_["LaneEdgeParticleModifier"] = []() { return std::make_unique<LaneEdgeParticleModifier>(); };
}

std::unique_ptr<ParticleModifier> ParticleModifierFactory::CreateModifier(const std::string& _name)
{
    auto it = modifierCreators_.find(_name);
    if (it != modifierCreators_.end())
    {
        return it->second();
    }

    assert("モディファイアが見つかりませんでした。");

    return nullptr;
}
