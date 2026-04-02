#pragma once

#include <Features/Model/InstancedObjectModel.h>
#include <Features/Camera/Camera/Camera.h>
#include <memory>


class NoteDrawer
{
public:

    NoteDrawer() = default;
    ~NoteDrawer() = default;

    void Initialize(uint32_t maxNotes = Engine::InstancedObjectModel::kDefaultMaxInstances, uint32_t maxBridges = Engine::InstancedObjectModel::kDefaultMaxInstances / 2);

    void AddNote(const Engine::Matrix4x4& world, const Engine::Vector4& color);
    void AddBridge(const Engine::Matrix4x4& world, const Engine::Vector4& color);

    void Draw(const Engine::Camera* camera);
    void Clear();
private:

    std::unique_ptr<Engine::InstancedObjectModel> noteModels_;
    std::unique_ptr<Engine::InstancedObjectModel> bridgeModels_;

};