#include "musicSelectUI.h"

#include <Debug/ImGuiDebugManager.h>
#include <Application/MusicList/MusicListManager.h>
#include <System/Audio/AudioSystem.h>
#include <Application/Setting/Setting.h>
#include <System/Input/Input.h>
#include <Features/Event/EventManager.h>
#include <Application/Scene/Data/SceneDatas.h>
#include <Features/UI/UINavigationManager.h>
#include <Features/UI/Component/UISpriteRenderComponent.h>
using namespace Engine;


namespace
{
const int32_t kVisibleCount = 7; // 一度に表示するアイテム数
const int32_t kHalfVisibleCount = (kVisibleCount - 1) / 2;
Vector4 focusColor = Vector4(0.44f, 0.66f, 0.97f, 1.0f);
}

void MusicSelectUI::Initialize(std::shared_ptr<VoiceInstance> voiceInstance)
{
    isInitialized_ = false;
    selectedIndex_ = 0;

    // Jsonバインダー初期化
    InitJsonBinder();

    // リスト背景画像の初期化
    backImage_ = std::make_unique<UIImageElement>("MusicSelectListBg", layoutCenter_, BaseUISize_);
    backImage_->Initialize();

    // 最低限のアイテム数を確保
    EnsureMinimumItems();

    // 入場アニメーション初期化
    entranceSequence_ = std::make_unique<AnimationSequence>("MusicSelectEntrance");
    entranceSequence_->Initialize("Resources/Data/AnimSeq/");
    isEntranceAnimPlaying_ = true;
    entranceAnimTime_ = 0.0f;

    voiceInstance_ = voiceInstance;
}

void MusicSelectUI::Update(float deltaTime)
{
#ifdef _DEBUG

    if (ImGuiDebugManager::GetInstance()->Begin("MusicSelectUI"))
    {

        ImGui::DragFloat2("BaseUISize", &BaseUISize_.x);
        ImGui::DragFloat("marginBetweenItems", &marginBetweenItems_, 1.0f, 0.0f, 500.0f);
        ImGui::DragFloat2("layout Center", &layoutCenter_.x, 0.01f);
        ImGui::SliderAngle("layoutAngle", &layoutAngle_);
        ImGui::DragFloat("selectedScale", &selectedScale_, 0.01f, 0.1f, 3.0f);
        ImGui::DragFloat("normalScale", &normalScale_, 0.01f, 0.1f, 3.0f);
        ImGui::DragFloat("bgPaddingX", &bgPaddingX_, 1.0f, 0.0f, 500.0f);
        ImGui::DragFloat("bgPaddingY", &bgPaddingY_, 1.0f, 0.0f, 500.0f);
        ImGui::ColorEdit4("focusColor", &focusColor.x);

        ImGui::Separator();
        static UVTransform transform;
        Vector2 offset = transform.GetOffset();
        Vector2 scale = transform.GetScale();
        float rotation = transform.GetRotation();
        bool changed = false;
        changed |= ImGui::DragFloat2("UV Offset", &offset.x, 0.01f);
        changed |= ImGui::DragFloat2("UV Scale", &scale.x, 0.01f);
        changed |= ImGui::DragFloat("UV Rotation", &rotation, 0.01f);
        if (changed)
        {
            transform.SetOffset(offset);
            transform.SetScale(scale);
            transform.SetRotation(rotation);
            for (auto& item : uiItems_)
            {
                auto spriteComp = item->GetComponent<UISpriteRenderComponent>();
                if (spriteComp)
                {
                    spriteComp->GetUVTransform() = transform;
                }
            }
        }



        ImGui::InputInt("Select index", reinterpret_cast<int*>(&selectedIndex_));

        if (ImGui::Button("Add Item"))
        {
            auto button1 = std::make_unique<UIButtonElement>("MusicSelectButton1", Vector2(0.0f, 0.0f), Vector2(200.0f, 50.0f), "Button1");
            button1->Initialize();

            uiItems_.push_back(std::move(button1));
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

    // 入場アニメーション中は時間を進め、操作を無効化
    if (isEntranceAnimPlaying_)
    {
        entranceAnimTime_ += deltaTime;
        if (entranceAnimTime_ >= entranceSequence_->GetMaxPlayTime())
        {
            entranceAnimTime_ = entranceSequence_->GetMaxPlayTime();
            isEntranceAnimPlaying_ = false;
        }
    }
    else
    {
        MoveSelection();
    }

    UpdateLayout(deltaTime);
    UpdateScaling();
    UpdateItemColors();

    for (auto& item : uiItems_)
    {
        item->Update();
    }
    if (!voiceInstance_ || !voiceInstance_->IsPlaying())
    {
        PlaySelectedMusic();
    }

}

void MusicSelectUI::Draw()
{
    // 背景パネルをアイテムより先に描画
    if (backImage_)
        backImage_->Draw();

    int32_t itemCount = static_cast<int32_t>(uiItems_.size());

    for (int32_t i = -kHalfVisibleCount; i <= kHalfVisibleCount + 1; ++i)
    {
        int32_t index = (selectedIndex_ + i + itemCount) % itemCount;
        uiItems_[index]->Draw();
    }
}

float MusicSelectUI::GetSelectedMusicElapsedTime() const
{
    if (voiceInstance_ && voiceInstance_->IsPlaying())
        return voiceInstance_->GetElapsedTime();
    return 0.0f;
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
            button->SetNormalColor(Vector4(0.15f, 0.15f, 0.15f, 1.0f));
            button->SetTextColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            auto comp = button->GetComponent<UISpriteRenderComponent>();
            comp->LoadAndSetTexture("pattern_dot_medium.png");
            auto& uvTrans = comp->GetUVTransform();
            uvTrans.SetScale(Vector2(15.0f, 5.0f));
            uiItems_.push_back(std::move(button));
        }
    }

    // visibleCountより少ない場合は黒くする
    if (musicListSize_ < kVisibleCount)
    {
        for (size_t i = list.size(); i < uiItems_.size(); ++i)
        {
            auto& item = uiItems_[i];
            item->SetNormalColor(Vector4(   0.01f, 0.01f, 0.01f, 1.0f));
            item->SetHoverColor(Vector4(    0.01f, 0.01f, 0.01f, 1.0f));
            item->SetPressedColor(Vector4(  0.01f, 0.01f, 0.01f, 1.0f));
            item->SetDisabledColor(Vector4( 0.01f, 0.01f, 0.01f, 1.0f));
            item->SetTextColor(Vector4(0.5f, 0.5f, 0.5f, 0.0f));
        }
    }

    // listを走査
    for (size_t i = 0; i < list.size(); ++i)
    {
        // ボタンのテキストを設定
        auto& item = uiItems_[i];
        item->SetText(list[i].title);
        item->SetOnClick([this]()
                         {
                             OnItemSelected();
                         });
    }

    int32_t itemCount = static_cast<int32_t>(uiItems_.size());
    for (int32_t i = -kHalfVisibleCount; i <= kHalfVisibleCount; ++i)
    {
        int32_t index = (selectedIndex_ + i + itemCount) % itemCount;
        auto& item = uiItems_[index];
        float scale = (i == 0) ? selectedScale_ : normalScale_;
        Vector2 targetSize = BaseUISize_ * scale;
        item->SetSize(targetSize);
    }

    isInitialized_ = true;
}

void MusicSelectUI::InitJsonBinder()
{
    jsonBinder_ = std::make_unique<JsonBinder>("MusicSelectUI", "Resources/Data/Select/");

    jsonBinder_->RegisterVariable("baseUISize", &BaseUISize_);
    jsonBinder_->RegisterVariable("marginBetweenItems", &marginBetweenItems_);
    jsonBinder_->RegisterVariable("layoutAngle", &layoutAngle_);
    jsonBinder_->RegisterVariable("layoutCenter", &layoutCenter_);
    jsonBinder_->RegisterVariable("selectedScale", &selectedScale_);
    jsonBinder_->RegisterVariable("normalScale", &normalScale_);
    jsonBinder_->RegisterVariable("bgPaddingX", &bgPaddingX_);
    jsonBinder_->RegisterVariable("bgPaddingY", &bgPaddingY_);
}

void MusicSelectUI::UpdateLayout(float deltaTime)
{
    int32_t itemCount =static_cast<int32_t>(uiItems_.size());
    if (itemCount == 0)
        return;

    float t = 1.0f;
    if (scrollDirection_ != 0)
    {
        scrollElapsedTime_ += deltaTime;
        t = 1.0f - scrollElapsedTime_ / scrollDuration_;
        if (t <= 0.0f)
        {
            t = 0.0f;
            scrollDirection_ = 0;
            scrollElapsedTime_ = 0.0f;
        }
    }

    // 入場アニメーション値の取得
    Vector2 slideOffset = { 0.0f, 0.0f };
    float spreadFactor = 1.0f;
    Vector2 bgOffset = { 0.0f, 0.0f };
    if (isEntranceAnimPlaying_)
    {
        slideOffset          = entranceSequence_->GetValueAtTime<Vector2>("slideOffset", entranceAnimTime_);
        spreadFactor         = entranceSequence_->GetValueAtTime<float>("spreadFactor", entranceAnimTime_);
        float bgSlideFactor  = entranceSequence_->GetValueAtTime<float>("bgSlideFactor", entranceAnimTime_);

        // layoutAngleに沿った方向ベクトルの逆方向からスライドイン
        // angle=0なら真上、傾きがあればその方向に沿って入ってくる
        const float kBgSlideDistance = 800.0f;
        float dirX = sinf(layoutAngle_);
        float dirY = -cosf(layoutAngle_);
        bgOffset.x = dirX * kBgSlideDistance * (1.0f - bgSlideFactor);
        bgOffset.y = dirY * kBgSlideDistance * (1.0f - bgSlideFactor);
    }

    // 集合位置（選択アイテムの円周上の位置）
    float centerX = layoutCenter_.x;
    float centerY = layoutCenter_.y;

    const Vector2 offScreen = { 3000.0f, 3000.0f };

    // スクロール中は両側の表示範囲を1つ広げて退場アニメーションを可能にする
    // (アイテム数が偶数の場合、循環距離の境界ケースがあるため両側広げる)
    int32_t visibleMin = -kHalfVisibleCount;
    int32_t visibleMax = kHalfVisibleCount;
    if (scrollDirection_ != 0)
    {
        visibleMin = -(kHalfVisibleCount);
        visibleMax = kHalfVisibleCount;
    }

    for (int32_t i = 0; i < itemCount; ++i)
    {
        // selectedIndexからの相対位置を計算（循環考慮）
        int32_t diff = i - selectedIndex_;
        // 循環の最短距離に補正
        if (diff > itemCount / 2) diff -= itemCount;
        if (diff < -itemCount / 2) diff += itemCount;

        // 表示範囲外は画面外へ
        if (diff < visibleMin || diff > visibleMax)
        {
            uiItems_[i]->SetPosition(offScreen);
            continue;
        }

        auto& item = uiItems_[i];

        // layoutAngleで回転した方向ベクトルでリストの傾きを表現
        // layoutAngle_=0: 純粋な縦並び / 正値: 右上がりに傾く
        float dirX = sinf(layoutAngle_);
        float dirY = -cosf(layoutAngle_);

        // スクロールアニメーション中は開始位置を1ステップ分ずらし、tとともに収束させる
        float effectiveDiff = static_cast<float>(diff) + static_cast<float>(scrollDirection_) * t;

        float x = centerX + effectiveDiff * dirX * marginBetweenItems_;
        float y = centerY + effectiveDiff * dirY * marginBetweenItems_;

        // 入場アニメーション: 集合位置から各アイテムの本来位置へ拡散
        x = Lerp(centerX + slideOffset.x, x, spreadFactor);
        y = Lerp(centerY + slideOffset.y, y, spreadFactor);

        item->SetPosition(Vector2(x, y));
    }

    // 背景パネル: layoutAngleで回転させ、アイテム列全体を覆うサイズに設定
    if (backImage_)
    {
        // 幅: 選択中アイテムの横幅 + 左右余白
        float bgW = BaseUISize_.x * selectedScale_ + bgPaddingX_ * 2.0f;
        // 高さ: アイテム列の全スパン（上下 kHalfVisibleCount 個分）+ 端アイテムの縦幅 + 上下余白
        float bgH = static_cast<float>(kHalfVisibleCount * 2) * marginBetweenItems_
            + BaseUISize_.y * normalScale_ + bgPaddingY_ * 2.0f;

        backImage_->SetPosition(Vector2(layoutCenter_.x + bgOffset.x, layoutCenter_.y + bgOffset.y));
        backImage_->SetSize(Vector2(bgW, bgH));
        backImage_->SetRotation(layoutAngle_);  // リストの傾きに合わせて回転
        backImage_->Update();
    }
}

void MusicSelectUI::UpdateScaling()
{
    int32_t itemCount = static_cast<int32_t>(uiItems_.size());
    if (itemCount == 0)
        return;

    float t = 1.0f;
    if (scrollDirection_ != 0)
    {
        t = 1.0f - scrollElapsedTime_ / scrollDuration_;
        if (t <= 0.0f)
        {
            t = 0.0f;
            scrollDirection_ = 0;
            scrollElapsedTime_ = 0.0f;
        }
    }

    // 選択中: フルサイズ / それ以外: 一律縮小
    for (int32_t i = -kHalfVisibleCount; i <= kHalfVisibleCount; ++i)
    {
        int32_t index = (selectedIndex_ + i + itemCount) % itemCount;
        auto& item = uiItems_[index];
        float scale = (i == 0) ? selectedScale_ : normalScale_;
        Vector2 targetSize = BaseUISize_ * scale;
        // スクロール中のみLerpでアニメーション。停止中は目標サイズを直接適用
        Vector2 newSize = (scrollDirection_ != 0) ? Lerp(targetSize, item->GetSize(), t) : targetSize;
        item->SetSize(newSize);
    }
}

void MusicSelectUI::UpdateItemColors()
{
    int32_t itemCount = static_cast<int32_t>(uiItems_.size());
    for (int32_t i = 0; i < itemCount; ++i)
    {
        int32_t diff = i - selectedIndex_;
        if (diff > itemCount / 2) diff -= itemCount;
        if (diff < -itemCount / 2) diff += itemCount;

        if (diff == 0)
        {
            uiItems_[i]->SetFocusColor(focusColor);  // フォーカス時もブルーを維持
        }
        else
        {
            //uiItems_[i]->SetNormalColor(Vector4(0.93f, 0.93f, 0.95f, 1.0f)); // 非選択: ライトグレー
        }
    }
}

void MusicSelectUI::EnsureMinimumItems()
{
    // 最低限のアイテム数を確保
    while (uiItems_.size() < static_cast<size_t>(kVisibleCount))
    {
        std::string buttonName = "MusicSelectButton" + std::to_string(uiItems_.size());

        auto button = std::make_unique<UIButtonElement>("MusicSelectButton", Vector2(0.0f, 0.0f), Vector2(200.0f, 50.0f), buttonName);
        button->Initialize();
        button->SetNormalColor(Vector4(0.15f, 0.15f, 0.15f, 1.0f));
        button->SetTextColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        auto comp = button->GetComponent<UISpriteRenderComponent>();
        comp->LoadAndSetTexture("pattern_dot_medium.png");
        auto& uvTrans = comp->GetUVTransform();
        uvTrans.SetScale(Vector2(15.0f, 5.0f));
        uiItems_.push_back(std::move(button));
    }
}

void MusicSelectUI::ClampSelectedIndex()
{
    uint32_t itemCount = static_cast<int32_t>(musicListSize_);
    selectedIndex_ = (selectedIndex_ + itemCount) % itemCount;
}

void MusicSelectUI::MoveSelection()
{
    int32_t pre = selectedIndex_;

    auto input = Input::GetInstance();
    if (input->IsKeyTriggered(DIK_UP) ||
        input->IsKeyTriggered(DIK_W))
    {
        selectedIndex_++;
        if (scrollDirection_ == 0)
            scrollDirection_ = 1;
    }
    else if (input->IsKeyTriggered(DIK_DOWN)
             || input->IsKeyTriggered(DIK_S))
    {
        selectedIndex_--;
        if (scrollDirection_ == 0)
            scrollDirection_ = -1;
    }
    ClampSelectedIndex();

    if (pre != selectedIndex_)
    {
        OnItemFocusEnter();
    }
}

void MusicSelectUI::OnItemFocusEnter()
{
    PlaySelectedMusic();

    // navigationの設定
    if (selectedIndex_ >= 0 && uiItems_.size() > selectedIndex_)
        UINavigationManager::GetInstance()->SetFocus(uiItems_[selectedIndex_].get());
}

void MusicSelectUI::OnItemSelected()
{
    MusicSelectUIEventData eventData;
    eventData.selectedFilePath = MusicListManager::GetInstance()->GetMusicMetaDataAt(selectedIndex_).filePath;

    if (eventData.selectedFilePath.empty())
        return;

    EventManager::GetInstance()->DispatchEvent(GameEvent("StartGame", &eventData));
    UINavigationManager::GetInstance()->ClearFocus();
}

void MusicSelectUI::PlaySelectedMusic()
{
    if (voiceInstance_ && voiceInstance_->IsPlaying())
        voiceInstance_->Stop();

    auto metaData = MusicListManager::GetInstance()->GetMusicMetaDataAt(selectedIndex_);
    if (metaData.audioFilePath.empty())
    {
        return;
    }

    // 音声の読み込みと再生
    bgmSoundInstance_ = AudioSystem::GetInstance()->Load(metaData.audioFilePath);

    if (bgmSoundInstance_)
    {
        voiceInstance_ = bgmSoundInstance_->Play(Setting::current_.musicVolume, false);
    }

}
