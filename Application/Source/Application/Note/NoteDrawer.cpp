#include "NoteDrawer.h"

void NoteDrawer::Initialize(uint32_t maxNotes, uint32_t maxBridges)
{
    noteModels_ = std::make_unique<Engine::InstancedObjectModel>();
    noteModels_->Initialize("cube/cube.obj", maxNotes);

    bridgeModels_ = std::make_unique<Engine::InstancedObjectModel>();
    bridgeModels_->Initialize("pY1x1p01Plane", maxBridges);
}

void NoteDrawer::AddNote(const Engine::Matrix4x4& world, const Engine::Vector4& color)
{
    noteModels_->AddInstance(world, color);
}

void NoteDrawer::AddBridge(const Engine::Matrix4x4& world, const Engine::Vector4& color)
{
    bridgeModels_->AddInstance(world, color);
}

void NoteDrawer::Draw(const Engine::Camera* camera)
{
    noteModels_->Draw(camera);
    bridgeModels_->Draw(camera);
}

void NoteDrawer::Clear()
{
    noteModels_->Clear();
    bridgeModels_->Clear();
}
