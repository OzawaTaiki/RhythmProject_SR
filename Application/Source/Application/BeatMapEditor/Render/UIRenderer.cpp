#include "UIRenderer.h"
#include <Application/BeatMapEditor/EditorState.h>
#include <Application/BeatMapEditor/BeatMapDocument.h>
#include <Application/BeatMapEditor/AudioController.h>
#include <Application/BeatMapEditor/BeatMapFileManager.h>
#include <Application/BeatMapEditor/EditorCoordinate.h>
#include <Application/BeatsManager/BeatManager.h>
#include <Core/WinApp/WinApp.h>
#include <Debug/ImGuiHelper.h>
#include <Utility/StringUtils/StringUitls.h>
#include <Utility/FileDialog/FileDialog.h>

namespace BME
{

void UIRenderer::Initialize()
{
    // TODO: UI初期化
    draggingAreaSprite_ = std::make_unique<Sprite>("DraggingAreaSprite", false);
    draggingAreaSprite_->Initialize();
    draggingAreaSprite_->SetColor(Vector4(0.0f, 0.5f, 1.0f, 0.3f)); // ドラッグエリアの色を設定
    draggingAreaSprite_->SetAnchor(Vector2(0.0f, 0.0f)); // アンカーを左上に設定
}

void UIRenderer::Draw(
    State* state,
    Document* document,
    AudioController* audioController,
    FileManager* fileManager,
    BeatManager* beatManager,
    EditorCoordinate* coordinate)
{
    DrawLeftPanel(state, document, audioController, beatManager, coordinate);
    DrawRightPanel(state, document, audioController, fileManager);
    DrawDraggingArea(state);
}

void UIRenderer::DrawLeftPanel(State* state, Document* document, AudioController* audioController, BeatManager* beatManager, EditorCoordinate* coordinate)
{
#ifdef _DEBUG

    // Modeの表示
    // BPM offset snap

    const float timelineHeight = 100.0f; // タイムラインの高さを設定
    const ImVec2 panelSize(300.0f, WinApp::kWindowSize_.y - timelineHeight); // 左パネルのサイズを設定
    const ImVec2 panelPos(0.0f, 0.0f); // 左パネルの位置を設定

    ImGui::SetNextWindowPos(panelPos); // パネルの位置を設定
    ImGui::SetNextWindowSize(panelSize); // パネルのサイズを設定

    const float spacing = 4.0f; // パネル内のスペーシングを設定

    ImGui::Begin("Editor Controls", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::SeparatorText("Editor Mode");
        int currentEditorMode = static_cast<int>(state->GetCurrentMode());
        bool hasChanged = false;
        hasChanged |= ImGui::RadioButton("Select", &currentEditorMode, static_cast<int>(EditorMode::Select));
        hasChanged |= ImGui::RadioButton("Note", &currentEditorMode, static_cast<int>(EditorMode::PlaceNormalNote));
        hasChanged |= ImGui::RadioButton("Long Note", &currentEditorMode, static_cast<int>(EditorMode::PlaceLongNote));

        if (hasChanged)
        {
            state->ChangeEditorMode(static_cast<BME::EditorMode>(currentEditorMode));
            //ChangeEditorMode(static_cast<EditorMode>(currentEditorMode)); // エディターのモードを変更
        }

        auto& beatMapData = document->GetMutableData();

        ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

        ImGui::SeparatorText("BPM");
        if (ImGui::DragFloat("BPM", &beatMapData.bpm, 0.1f, 0.1f, 1000.0f, " %.1f")) // BPMの入力フィールド
        {
            beatManager->SetBPM(beatMapData.bpm); // BPMを設定
        }
        static bool bpmCounter = false; // BPMカウンターの状態
        if (ImGui::Checkbox("BPM Counter", &bpmCounter)) // BPMカウンターのチェックボックス
        {
            if (bpmCounter)
            {
                state->ChangeEditorMode(EditorMode::BPMSetting);
            }
            else
            {
                state->ChangeEditorMode(state->GetPreviousMode());
            }
        }

        ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

        ImGui::SeparatorText("Offset");
        if (ImGui::DragFloat("Offset", &beatMapData.offset, 0.01f, -100.0f, 100.0f, " %.3f s")) // オフセットの入力フィールド
        {
            beatManager->SetOffset(beatMapData.offset); // オフセットを設定
            coordinate->SetOffsetTime(beatMapData.offset); // エディターのオフセットを設定
        }

        ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

        ImGui::SeparatorText("Snap Settings");
        static int snapIntervalIndex = 2; // デフォルトのスナップ間隔を1/4に設定
        ImGui::RadioButton("1/1", &snapIntervalIndex, 0); ImGui::SameLine();
        ImGui::RadioButton("1/2", &snapIntervalIndex, 1); ImGui::SameLine();
        ImGui::RadioButton("1/4", &snapIntervalIndex, 2);
        ImGui::RadioButton("1/8", &snapIntervalIndex, 3); ImGui::SameLine();
        ImGui::RadioButton("1/16", &snapIntervalIndex, 4);
        state->SetSnapInterval(1.0f / std::powf(2.0f, static_cast<float>(snapIntervalIndex))); // スナップ間隔を計算
        bool enabled = state->IsGridSnapEnabled();
        ImGui::Checkbox("Grid Snap", &enabled); // グリッドスナップのチェックボックス
        state->SetGridSnapEnabled(enabled);

        ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

        ImGui::SeparatorText("Music");
        auto musicSoundInstance = audioController->GetSoundInstance();
        auto musicVoiceInstance = audioController->GetVoiceInstance();
        static float volume = 0.5f;
        ImGui::Text("Music File: %s", beatMapData.audioFilePath.empty() ? "None" : StringUtils::GetAfterLast(beatMapData.audioFilePath, "\\").c_str());

        ImGui::Text("Music Duration: %.2f seconds", musicSoundInstance ? musicSoundInstance->GetDuration() : 0.0f); // 音楽の再生時間を表示
        if (ImGui::DragFloat("Volume", &volume, 0.01f, 0.0f, 1.0f, " %.2f")) // 音量の入力フィールド
            if (musicVoiceInstance)
            {
                audioController->SetVolume(volume);
                musicVoiceInstance->SetVolume(volume); // 音量を設定
            }

        if (ImGui::Button("Load Music"))
        {
            std::string musicFilePath = FileDialog::OpenFile(FileFilterBuilder::GetFilterString(FileFilterBuilder::FilterType::AudioFiles));
            if (!musicFilePath.empty())
            {
                // 音楽ファイルのパスを設定
                beatMapData.audioFilePath = musicFilePath;
                // 音楽をロード
                audioController->Load(musicFilePath);
            }
        }
        ImGui::BeginDisabled(!musicSoundInstance); // 音楽がロードされていない場合は無効化
        {
            if (ImGui::Button("Play Begin"))
            {
                audioController->Play(0.0f);
            }
            ImGui::SameLine();
            if (ImGui::Button("Play Current Time"))
            {
                audioController->Play(audioController->GetElapsedTime());
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(!musicVoiceInstance || !musicVoiceInstance->IsPlaying()); // 音楽が再生中でない場合は無効化
            {
                if (ImGui::Button("Stop Music"))
                {
                    audioController->Stop();
                }
            }
            ImGui::EndDisabled(); // 無効化終了
        }
        ImGui::EndDisabled(); // 音楽がロードされていない場合は無効化終了
        static float playSpeed = 1.0f; // 再生速度の初期値
        ImGui::DragFloat("play Speed", &playSpeed, 0.01f, 0.1f, 2.0f, " %.2f"); // 再生速度の入力フィールド
        if (musicVoiceInstance)
        {
            musicVoiceInstance->SetPlaySpeed(playSpeed); // 再生速度を設定
        }
        bool enableBeats = beatManager->GetSoundEnabled();
        ImGui::Checkbox("enable Beats", &enableBeats);// TODO
        beatManager->SetEnableSound(enableBeats);
    }

    ImGui::End();

#endif // _DEBUG
}

void UIRenderer::DrawRightPanel(State* state, Document* document, AudioController* audioController, FileManager* fileManager)
{
#ifdef _DEBUG

    // File 情報
    // 選択ノートの情報
    // beatMapInfoの情報 noteの総数 楽曲時間

    std::string currentPath = fileManager->GetCurrentFilePath();

    const float timelineHeight = 100.0f; // タイムラインの高さを設定
    const ImVec2 panelSize(300.0f, WinApp::kWindowSize_.y - timelineHeight); // 左パネルのサイズを設定
    const ImVec2 panelPos(WinApp::kWindowSize_.x - panelSize.x, 0.0f); // 左パネルの位置を設定

    ImGui::SetNextWindowPos(panelPos); // パネルの位置を設定
    ImGui::SetNextWindowSize(panelSize); // パネルのサイズを設定

    const float spacing = 10.0f; // パネル内のスペーシングを設定

    ImGui::Begin("Information", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::SeparatorText("BeatMap Info");

        ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

        ImGui::Text("Current File:");
        ImGui::Text("\t%s", currentPath.empty() ? "None" : StringUtils::GetAfterLast(currentPath, "\\").c_str());


        ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

        FileFilterBuilder filterBuilder;
        filterBuilder.AddSeparateExtensions(FileFilterBuilder::FilterType::DataFiles);
        static std::string filter = filterBuilder.Build();

        if (ImGui::Button("Load BeatMap"))
        {
            currentPath = FileDialog::OpenFile((filter));
            fileManager->Load(currentPath, document);
            audioController->Load(document->GetData().audioFilePath);
            //LoadBeatMap(currentPath); // 譜面のロード
        }
        ImGui::SameLine();
        //ImGui::BeginDisabled(currentFilePath_ == "");
        {
            if (ImGui::Button("Save BeatMap"))
            {
                currentPath = FileDialog::SaveFile(filter);
                ImGui::OpenPopup("Save Confirmation");
                //SaveBeatMap(currentFilePath_); // 譜面の保存
            }
        }
        //ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("New BeatMap"))
        {
            //CreateNewBeatMap("newFile", "");
            fileManager->CreateNew("newFile", "", document);
        }

        auto& data = document->GetMutableData();

        if (ImGui::BeginPopupModal("Save Confirmation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("SavePath : %s", currentPath.c_str());
            ImGui::Separator();

            ImGuiHelper::InputText("Title", data.title); // タイトルの入力フィールド
            ImGui::DragFloat("BPM", &data.bpm, 0.1f, 0.1f, 1000.0f); // BPMの入力フィールド
            ImGui::DragFloat("Offset", &data.offset, 0.01f, -100.0f, 100.0f); // オフセットの入力フィールド

            if (ImGui::Button("ok"))
            {
                fileManager->SaveAs(currentPath, document);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(0.0f, spacing / 2.0f)); // スペーシングを追加
        ImGui::Text("Notes : %zu", data.notes.size());

        ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        ImGui::SeparatorText("Notes Info");

        const auto& selectedNotes = state->GetSelectedNotes();
        if (!selectedNotes.empty())
        {
            ImGui::Text("Selected Notes: ");
            ImGui::Text("\t%zu", selectedNotes.size());
            ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

            const size_t kMaxViewableNotes = 5; // 表示可能なノートの最大数

            for (size_t i = 0; i < selectedNotes.size() && i < kMaxViewableNotes; ++i)
            {
                const NoteData& note = data.notes[selectedNotes[i]];
                ImGui::Separator();
                ImGui::Text("Note Index: %zu", selectedNotes[i]);
                ImGui::Text("\tLane: %d", note.laneIndex);
                ImGui::Text("\tTime: %.2f", note.targetTime);
                ImGui::Text("\tType: %s", note.noteType.c_str());
                if (note.noteType == "hold")
                {
                    ImGui::Text("\tHold Duration: %.2f", note.holdDuration);
                }
                ImGui::Dummy(ImVec2(0.0f, spacing)); // スペーシングを追加

            }
            if (selectedNotes.size() > kMaxViewableNotes)
            {
                ImGui::Text("...and %zu more notes", selectedNotes.size() - kMaxViewableNotes);
            }
        }

    }
    ImGui::End();

#endif
}

void UIRenderer::DrawDraggingArea(State* state)
{
    if (!state || !state->IsDragging())
        return;

    Vector2 rectLT, rectRB;

    state->GetDragSelectionRect(rectLT, rectRB);
    Vector2 size = rectRB - rectLT;
    draggingAreaSprite_->translate_= rectLT;
    draggingAreaSprite_->SetSize(size);

    draggingAreaSprite_->Draw();
}

void UIRenderer::Finalize()
{
    // TODO: 終了処理
}

} // namespace BME
