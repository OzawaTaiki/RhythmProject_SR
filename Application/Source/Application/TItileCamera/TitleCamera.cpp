#include "TitleCamera.h"
#include <Debug/ImguITools.h>

void TitleCamera::Initialize()
{
    camera_.Initialize(CameraType::Perspective);

    cameraAnimationSequence_ = std::make_unique<AnimationSequence>("TitleCameraAnimation");
    cameraAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

}

void TitleCamera::Update(float _deltaTime)
{
    ImGuiTool::TimeLine("TitleCameraAnimation", cameraAnimationSequence_.get());
    ImGui::Begin("TitleCameraDebug", nullptr, ImGuiWindowFlags_NoTitleBar);
    if(ImGui::Checkbox("isAnimationPlaying", &isAnimationPlaying_))
    {
        if (isAnimationPlaying_)
        {
            cameraAnimationSequence_->SetCurrentTime(0.0f);
        }
    }
    ImGui::End();

    if (isAnimationPlaying_)
    {
        cameraAnimationSequence_->Update(_deltaTime);
        camera_.translate_ = cameraAnimationSequence_->GetValue<Vector3>("translate");
    }

    camera_.Update();
    camera_.UpdateMatrix();
}

void TitleCamera::PlayCameraAnimation()
{
    isAnimationPlaying_ = true;
    cameraAnimationSequence_->SetCurrentTime(0.0f);
}
