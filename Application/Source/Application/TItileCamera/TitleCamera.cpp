#include "TitleCamera.h"
#include <Debug/ImguITools.h>
#include <Features/Event/EventManager.h>
#include <System/Input/Input.h>

void TitleCamera::Initialize()
{
    camera_.Initialize(CameraType::Perspective);

    cameraAnimationSequence_ = std::make_unique<AnimationSequence>("TitleCameraAnimation");
    cameraAnimationSequence_->Initialize("Resources/Data/AnimSeq/");

    camera_.translate_ = cameraAnimationSequence_->GetValueAtTime<Vector3>("translate", 0.0f);
}

void TitleCamera::Update(float _deltaTime)
{
#ifdef _DEBUG
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
#endif // _DEBUG

    if(Input::GetInstance()->IsKeyTriggered(DIK_SPACE)||
       Input::GetInstance()->IsKeyTriggered(DIK_RETURN))
    {
        PlayCameraAnimation();
    }

    if (isAnimationPlaying_)
    {
        cameraAnimationSequence_->Update(_deltaTime);
        camera_.translate_ = cameraAnimationSequence_->GetValue<Vector3>("translate");
        if (cameraAnimationSequence_->IsEnd())
        {
            isAnimationPlaying_ = false;
            EventManager::GetInstance()->DispatchEvent(GameEvent("TitleCameraAnimationEnd", nullptr));
        }
    }

    camera_.Update();
    camera_.UpdateMatrix();
}

void TitleCamera::PlayCameraAnimation()
{
    if (isAnimationPlaying_) return;

    isAnimationPlaying_ = true;
    cameraAnimationSequence_->SetCurrentTime(0.0f);
}
