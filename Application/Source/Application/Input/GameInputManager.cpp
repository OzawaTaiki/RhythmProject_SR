#include "GameInputManager.h"

#include <Debug/Debug.h>

#include <algorithm>

void GameInputManager::Initialize(Input* input)
{
    if (input == nullptr)
        input_ = Input::GetInstance();
    else
        input_ = input;

    SetDefaultKeyBindings();

    inputData_.clear();
}

void GameInputManager::Update()
{
    if (!gameMusic_)
    {
        Debug::Log("Music instance is not set.\n");
        return;
    }

    inputData_.clear(); // 前回の入力データをクリア

    for (const auto& [laneIndex, keycode] : keyBindings_)
    {
        InputData inputData;
        inputData.elapsedTime = gameMusic_->GetElapsedTime();
        inputData.laneIndex = laneIndex;

        if (input_->IsKeyTriggered(keycode))
        {
            inputData.state = KeyState::Trigger;

            inputData_.push_back(inputData);

            Debug::Log("Key triggered: " + std::to_string(keycode) + " on lane " + std::to_string(laneIndex) + "\n");
        }
        else if (input_->IsKeyPressed(keycode))
        {
            inputData.state = KeyState::Hold;
            inputData_.push_back(inputData);
        }
        else if (input_->IsKeyReleased(keycode))
        {
            inputData.state = KeyState::Released;
            inputData_.push_back(inputData);
        }
    }
}

void GameInputManager::SetKeyBinding(std::map<int32_t,uint8_t> keyBindings)
{
    keyBindings_ = keyBindings;
}

void GameInputManager::SetKeyBinding(uint8_t key, int32_t lane)
{
    //keyBindings_からおなじレーンを持つキーを探して削除
    for (auto it = keyBindings_.begin(); it != keyBindings_.end(); )
    {
        if (it->first == lane)
        {
            it = keyBindings_.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // 新しいバインディングを追加
    keyBindings_[lane] = key;
}

void GameInputManager::SetDefaultKeyBindings()
{
    keyBindings_[0] = DIK_D; // レーン0
    keyBindings_[1] = DIK_F; // レーン1
    keyBindings_[2] = DIK_J; // レーン2
    keyBindings_[3] = DIK_K; // レーン3
}
