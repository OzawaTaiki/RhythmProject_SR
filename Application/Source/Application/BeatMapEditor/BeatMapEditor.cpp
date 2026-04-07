#include "BeatMapEditor.h"

#include <Features/LineDrawer/LineDrawer.h>
#include <Debug/Debug.h>
#include <Debug/ImGuiHelper.h>
#include <Debug/ImGuiDebugManager.h>
#include <System/Input/Input.h>
#include <System/Audio/AudioSystem.h>
#include <Features/AudioSpectrum/AudioSpectrum.h>

#include <Application/BeatMapLoader/BeatMapLoader.h>
#include <Utility/FileDialog/FileDialog.h>
#include <Utility/StringUtils/StringUitls.h>

#include <Application/BeatMapEditor/Command/PlaceNoteCommand.h>
#include <Application/BeatMapEditor/Command/DeleteNoteCommand.h>
#include <Application/BeatMapEditor/Command/MoveNoteCommand.h>
#include <Application/BeatMapEditor/Command/ChangeHoldDurationCommand.h>
#include <Application/BeatMapEditor/Command/PasteCommand.h>
#include <Application/BeatMapEditor/Command/BatchInsertNotesCommand.h>

#include <Application/BeatMapEditor/AutoChartGenerator/autoChartGenerator.h>

#include <fstream>
#include <Framework/LayerSystem/LayerSystem.h>

using namespace Engine;


// TODO いろいろ
/// ブリッジに重ねてノーツを置けてしまう
/// zoom スクロール grid分割 全部キーボードあるいはマウスでできるように
///     Undo実装したい
///     BPM調整するやつ
//      流しながらノート入力したーい
//      targetTimeをもとに再生を行う

BeatMapEditor::BeatMapEditor()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->HideAllWindow();
#endif
}

BeatMapEditor::~BeatMapEditor()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->ShowAllWindow();
#endif
}

void BeatMapEditor::Initialize(const BeatMapData& _beatMapData)
{

    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->SetCameraPtr2D(&for2dCamera_); // 2Dカメラをライン描画クラスに設定

    for2dCamera_.Initialize(CameraType::Orthographic, WinApp::kWindowSize_); // 2Dカメラの初期化
    for2dCamera_.matProjection_ = Matrix4x4::Identity();
    for2dCamera_.matView_ = Matrix4x4::Identity();
    for2dCamera_.UpdateMatrix(); // カメラの行列を更新

    //                                       左右UI   マージン
    Vector2 laneAreaSize = Vector2(WinApp::kWindowSize_.x - 600.0f - 120.0f, WinApp::kWindowSize_.y); // レーンエリアのサイズを設定
    editorCoordinate_.Initialize(laneAreaSize); // 初期画面サイズとレーン数を設定
    editorCoordinate_.SetTimeZeroOffsetRatio(0.1f);

    beatManager_ = std::make_unique<BeatManager>();
    beatManager_->Initialize(120.0f, 0.0f); // 初期BPMとオフセットを設定
    // ========================================
    // 新アーキテクチャクラスの初期化
    // ========================================
    document_ = std::make_unique<BME::Document>();
    state_ = std::make_unique<BME::State>();
    audioController_ = std::make_unique<BME::AudioController>();
    inputHandler_ = std::make_unique<BME::InputHandler>();
    renderer_ = std::make_unique<BME::EditorRenderer>();
    fileManager_ = std::make_unique<BME::FileManager>();

    // レンダラー初期化
    renderer_->Initialize(&editorCoordinate_, state_.get(), for2dCamera_.GetViewProjection());

    // 初期譜面データを新Documentに設定
    document_->SetData(_beatMapData);

    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/SE/JudgeSound.wav");
}

void BeatMapEditor::Update()
{
    auto voice = audioController_->GetVoiceInstance();
    if (voice && voice->IsPlaying())
    {
        currentTime_ = voice->GetElapsedTime();
        beatManager_->Update();
    }

    editorCoordinate_.SetScrollOffset(currentTime_);

    inputHandler_->HandleInput(
        state_.get(),
        document_.get(),
        audioController_.get(),
        &commandHistory_,
        &editorCoordinate_,
        beatManager_.get(),
        currentTime_
    );

    static BME::AutoChartGenerator::GenerateRequest autoGenerateRequest ={};
    autoGenerateRequest.isRequested = false;

    // レンダラーの更新処理
    renderer_->Update(
        state_.get(),
        document_.get(),
        fileManager_.get(),
        audioController_.get(),
        &editorCoordinate_,
        beatManager_.get(),
        currentTime_,
        autoGenerateRequest
    );

    if (autoGenerateRequest.isRequested)
    {
        TriggerAutoGenerate(autoGenerateRequest.settings);
    }

    for (auto it = voiceInstance_.begin(); it != voiceInstance_.end(); )
    {
        if (!(*it)->IsPlaying())
            it = voiceInstance_.erase(it);
        else
            ++it;
    }

    static size_t lastNoteIndex = 0;
    if (!audioController_->IsPlaying())
        lastNoteIndex = 0;
    else
    {
        auto& beatMapData = document_->GetData();
        bool shouldSound = false;// サウンドを鳴らすかどうかのフラグ
        for (size_t i = lastNoteIndex; i < beatMapData.notes.size(); ++i)
        {
            auto& note = beatMapData.notes[i];
            if (std::abs(note.targetTime - currentTime_) < 0.01f)
            {
                shouldSound = true;
                lastNoteIndex = i;
            }
            else
            {
                // 現在の時間を過ぎたら次のノートへ
                if (note.targetTime > currentTime_)
                    break;
            }
        }
        if (shouldSound)
        {
            // 音ならす
            if (soundInstance_)
            {
                voiceInstance_.push_back(soundInstance_->Play(0.5f, false, true, nullptr, AudioSystem::GetInstance()->GetSESubmix()));
            }
        }
    }
}

void BeatMapEditor::Draw()
{
    // エディターの描画処理

    Sprite::PreDraw();

    renderer_->Draw(
        state_.get(),
        document_.get(),
        audioController_.get(),
        &editorCoordinate_,
        currentTime_
    );

}


void BeatMapEditor::SetNoteDuration(size_t _noteIndex, float _newDuration)
{
    if (_noteIndex >= currentBeatMapData_.notes.size())
    {
        Debug::Log("Invalid note index: " + std::to_string(_noteIndex) + "\n");
        return; // 無効なインデックスの場合は何もしない
    }
    if (currentBeatMapData_.notes[_noteIndex].noteType != "hold")
    {
        Debug::Log("Cannot set duration for non-hold note at index: " + std::to_string(_noteIndex) + "\n");
        return; // ホールドノート以外は変更できない
    }
    const auto& note = currentBeatMapData_.notes[_noteIndex];

    if (_newDuration <= 0.0f)
        return;

    float targetTime = note.targetTime + _newDuration;

    // グリッドスナップ
    if (state_->IsGridSnapEnabled())
    {
        _newDuration = editorCoordinate_.SnapTimeToGrid(targetTime - note.targetTime, currentBeatMapData_.bpm, static_cast<int>(1.0f / state_->GetSnapInterval()));
        if (_newDuration <= 0.0f)
            return;
    }
    currentBeatMapData_.notes[_noteIndex].holdDuration = _newDuration; // ノートの持続時間を更新
}

void BeatMapEditor::TriggerAutoGenerate(const BME::AutoChartGenerator::Settings& settings)
{
    if (!audioController_->HasAudio())
        return;

    const size_t windowSize = 1ull << static_cast<size_t>(settings.windowN);
    auto spectrum = std::make_unique<Engine::AudioSpectrum>(windowSize);
    auto soundInstance = audioController_->GetSoundInstance();
    spectrum->SetAudioData(soundInstance->GetAudioData());
    spectrum->SetSampleRate(soundInstance->GetSampleRate());
    spectrum->SetUseGPU(settings.useGpuFFT);

    float duration = soundInstance->GetDuration();
    float bpm = document_->GetData().bpm;
    float offset = document_->GetData().offset;

    std::vector<NoteData> generatedNotes = autoChartGenerator_.Generate(spectrum.get(), duration, bpm, offset, settings);

    if (generatedNotes.empty())
    {
        Debug::Log("Auto generation did not produce any notes.\n");
        return;
    }

    commandHistory_.ExecuteCommand(std::make_unique<BME::BatchInsertNotesCommand>(document_.get(), generatedNotes));

}
