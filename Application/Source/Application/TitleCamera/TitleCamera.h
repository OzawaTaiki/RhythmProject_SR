#pragma once

#include <Features/Camera/Camera/Camera.h>
#include <Features/Animation/Sequence/AnimationSequence.h>

class LobbyCamera
{
public:

    LobbyCamera() = default;
    ~LobbyCamera() = default;

    void Initialize();
    void Update(float deltaTime);

    void PlayCameraAnimation();

    Camera* GetCamera() { return &camera_; }
private:

    Camera camera_;

    bool isAnimationPlaying_ = false;
    bool isAnimationCompleted_ =false;

    std::unique_ptr<AnimationSequence> cameraAnimationSequence_ = nullptr;
};

