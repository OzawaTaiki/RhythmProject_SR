#include "musicSelectUI.h"

#include <Debug/ImGuiDebugManager.h>
#include <Application/MusicList/MusicListManager.h>
#include <System/Input/Input.h>

namespace
{
const int32_t kVisibleCount = 5; // 一度に表示するアイテム数
const int32_t kHalfVisibleCount = kVisibleCount / 2;
}

void MusicSelectUI::Initialize()
{
    isInitialized_ = false;
    selectedIndex_ = 0;

    // Jsonバインダー初期化
    InitJsonBinder();

    // 最低限のアイテム数を確保
    EnsureMinimumItems();
}

void MusicSelectUI::Update()
{
#ifdef _DEBUG

    if (ImGuiDebugManager::GetInstance()->Begin("MusicSelectUI"))
    {
        ImGui::DragFloat("scrollTime", &scrollTime_, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("marginAngle", &marginAngle_, 0.01f, 0.0f, 3.14f);
        ImGui::DragFloat2("BaseUISize", &BaseUISize_.x);

        ImGui::DragFloat2("layoutCircle_center", &layoutCircle_.center.x, 1.0f);
        ImGui::DragFloat("layoutCircle_radius", &layoutCircle_.radius, 1.0f, 0.0f, 2000.0f);
        ImGui::DragFloat("layoutCircle_startAngle", &layoutCircle_.startAngle, 0.01f);
        ImGui::DragFloat("layoutCircle_endAngle", &layoutCircle_.endAngle, 0.01f);
        ImGui::InputInt("focus", reinterpret_cast<int*>(&selectedIndex_));

        if (ImGui::Button("Add Item"))
        {
            auto button1 = std::make_unique<UIButtonElement>("MusicSelectButton1", Vector2(0.0f, 0.0f), Vector2(200.0f, 50.0f), "Button1");
            button1->Initialize();

            uiItems_.push_back({ std::move(button1), 0.0f });
        }

        if (ImGui::Button("Save"))
        {
            jsonBinder_->Save();
        }

        ImGui::End();
    }

#endif // _DEBUG

    if (!isInitialized_)
    {
        if (MusicListManager::GetInstance()->IsLoadComplete())
        {
            // 読み込み完了してたら初期化
            InitializeItemsFromData();
            isInitialized_ = true;
        }
    }

    MoveSelection();

    ClampSelectedIndex();

    UpdateLayout();
    UpdateScaling();

    for (auto& item : uiItems_)
    {
        item.item->Update();
    }
}

void MusicSelectUI::Draw()
{
    int32_t itemCount = static_cast<int32_t>(uiItems_.size());

    for (int32_t i = -kHalfVisibleCount; i <= kHalfVisibleCount; ++i)
    {
        int32_t index = (selectedIndex_ + i + itemCount) % itemCount;
        auto& item = uiItems_[index];
        item.item->Draw();
    }
}

void MusicSelectUI::InitializeItemsFromData()
{
    auto& list = MusicListManager::GetInstance()->GetMusicList();
    musicListSize_ =static_cast<int32_t>(list.size());
    // UIアイテム数が足りない場合は追加
    if (musicListSize_ >= kVisibleCount && musicListSize_ > uiItems_.size())
    {
        int32_t itemsToAdd = musicListSize_ - static_cast<int32_t>(uiItems_.size());
        for (int32_t i = 0; i < itemsToAdd; ++i)
        {
            std::string buttonName = "MusicSelectButton" + std::to_string(uiItems_.size());
            auto button = std::make_unique<UIButtonElement>("MusicSelectButton", Vector2(0.0f, 0.0f), Vector2(200.0f, 50.0f), buttonName);
            button->Initialize();
            uiItems_.push_back({ std::move(button), 0.0f });
        }
    }

    // visibleCountより少ない場合は黒くする
    if (musicListSize_ < kVisibleCount)
    {
        for (size_t i = list.size(); i < uiItems_.size(); ++i)
        {
            auto& item = uiItems_[i];
            item.item->SetNormalColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
            item.item->SetHoverColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
            item.item->SetPressedColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
            item.item->SetDisabledColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
            item.item->SetTextColor(Vector4(0.5f, 0.5f, 0.5f, 0.0f));
        }
    }

    // listを走査
    for (size_t i = 0; i < list.size(); ++i)
    {
        // ボタンのテキストを設定
        auto& item = uiItems_[i];
        item.item->SetText(list[i].title);
    }

    isInitialized_ = true;
}

void MusicSelectUI::InitJsonBinder()
{
    jsonBinder_ = std::make_unique<JsonBinder>("MusicSelectUI", "Resources/Data/Select/");

    jsonBinder_->RegisterVariable("scrollTime", &scrollTime_);
    jsonBinder_->RegisterVariable("marginAngle", &marginAngle_);
    jsonBinder_->RegisterVariable("baseUISize", &BaseUISize_);

    jsonBinder_->RegisterVariable("layoutCircle_center", &layoutCircle_.center);
    jsonBinder_->RegisterVariable("layoutCircle_radius", &layoutCircle_.radius);
    jsonBinder_->RegisterVariable("layoutCircle_startAngle", &layoutCircle_.startAngle);
    jsonBinder_->RegisterVariable("layoutCircle_endAngle", &layoutCircle_.endAngle);

}

void MusicSelectUI::UpdateLayout()
{
    int32_t itemCount =static_cast<int32_t>(uiItems_.size());
    if (itemCount == 0)
        return;

    // 選択しているアイテムの角度
    const float selectedAngle = (layoutCircle_.startAngle + layoutCircle_.endAngle) / 2.0f;// 中央に配置
    // アイテムの間隔角度
    const float angleStep = -(layoutCircle_.endAngle - layoutCircle_.startAngle) / static_cast<float>(kVisibleCount);

    int32_t startIndex = selectedIndex_ - kHalfVisibleCount;
    int32_t endIndex = selectedIndex_ + kHalfVisibleCount;

    for (int32_t i = startIndex; i <= endIndex; ++i)
    {
        float angle = selectedAngle + (i - selectedIndex_) * angleStep;
        // 対象のアイテムのインデックスを計算（循環考慮）
        int32_t itemIndex = (i + itemCount) % itemCount;
        auto& item = uiItems_[itemIndex];
        item.angle = angle;

        // 位置を計算
        float x = layoutCircle_.center.x + layoutCircle_.radius * cosf(angle);
        float y = layoutCircle_.center.y + layoutCircle_.radius * sinf(angle);
        item.item->SetPosition(Vector2(x, y));
    }
}

void MusicSelectUI::UpdateScaling()
{
    int32_t itemCount = static_cast<int32_t>(uiItems_.size());
    if (itemCount == 0)
        return;

    // 0.6 0.8 1.0 0.8 0.6
    for (int32_t i = -kHalfVisibleCount; i <= kHalfVisibleCount; ++i)
    {
        int32_t index = (selectedIndex_ + i + itemCount) % itemCount;
        auto& item = uiItems_[index];
        float scale = 1.0f - (std::abs(static_cast<float>(i)) * 0.2f);
        Vector2 newSize = BaseUISize_ * scale;
        item.item->SetSize(newSize);
    }
}

void MusicSelectUI::EnsureMinimumItems()
{
    while (uiItems_.size() < static_cast<size_t>(kVisibleCount))
    {
        std::string buttonName = "MusicSelectButton" + std::to_string(uiItems_.size());

        auto button = std::make_unique<UIButtonElement>("MusicSelectButton", Vector2(0.0f, 0.0f), Vector2(200.0f, 50.0f), buttonName);
        button->Initialize();
        uiItems_.push_back({ std::move(button), 0.0f });
    }
}

void MusicSelectUI::ClampSelectedIndex()
{
    uint32_t itemCount = static_cast<int32_t>(uiItems_.size());
    selectedIndex_ = (selectedIndex_ + itemCount) % itemCount;
}

void MusicSelectUI::MoveSelection()
{
    auto input = Input::GetInstance();
    if (input->IsKeyTriggered(DIK_UP) ||
        input->IsKeyTriggered(DIK_W))
    {
        selectedIndex_++;
    }
    else if (input->IsKeyTriggered(DIK_DOWN)
             || input->IsKeyTriggered(DIK_S))
    {
        selectedIndex_--;
    }
}
