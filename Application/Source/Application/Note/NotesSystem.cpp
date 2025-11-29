#include "NotesSystem.h"

#include <Debug/ImGuiDebugManager.h>

#include <Features/Event/EventManager.h>

#include <Application/EventData/JudgeResultData.h>

#include <Debug/Debug.h>
#include <Math/MyLib.h>

// TODO : notesystem 以下に表記
// 最初のnoteが少し速い気がする

NotesSystem::NotesSystem(Lane* lane) : lane_(lane), playing_(false)
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->AddDebugWindow("NotesSystem", [&]() { DebugWindow(); });
#endif // _DEBUG
}

NotesSystem::~NotesSystem()
{
#ifdef _DEBUG
    ImGuiDebugManager::GetInstance()->RemoveDebugWindow("NotesSystem");
#endif // _DEBUG
}

// TODO : 描画と更新の最適化
void NotesSystem::Initialize(float noteSpeed, float noteSize, float startOffsetTime)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    noteSpeed_ = noteSpeed;
    noteSize_ = noteSize;

    startOffsetTime_ = startOffsetTime;

    notes_.clear();

    Model::CreateFromFile("cube/cube.obj");

}

void NotesSystem::Update(float deltaTime)
{
    float elapsedTime = musicVoiceInstance_->GetElapsedTime();

    if(!isStarted_)
    {
        waitTimer_ += deltaTime;
        elapsedTime = Lerp(-startOffsetTime_, 0.0f, waitTimer_ / startOffsetTime_);
    }


    for (auto it = notes_.begin(); it != notes_.end();)
    {
        // 判定済みのノーツは削除
        if ((*it)->IsJudged())
        {
            it = notes_.erase(it);
            continue;
        }

        if (autoPlay_)
        {
            // ノートが判定ラインを超えたら消す
            if ((*it)->GetPosition().z <= judgeLinePosition_)
            {
                // 判定を行う
                JudgeResultData noteJudgeData(NoteJudgeType::Perfect, (*it)->GetLaneIndex());
                EventManager::GetInstance()->DispatchEvent(
                    GameEvent("JudgeResult", &noteJudgeData)
                );
                (*it)->Judge();
                Debug::Log(std::format("note judged : elapse time{} laneIndex:{}\n", elapsedTime, (*it)->GetLaneIndex()));
                continue;
            }

        }

        // ノーツは次のフレームで消す
        if (((*it)->GetPosition().z - noteSize_ / 2.0f) < judgeLinePosition_ - (noteSpeed_ * missJudgeThreshold_))
        {
            // 判定を行う
            JudgeResultData noteJudgeData(NoteJudgeType::Miss, (*it)->GetLaneIndex());
            EventManager::GetInstance()->DispatchEvent(
                GameEvent("JudgeResult", &noteJudgeData)
            );
            (*it)->Judge();
        }
        else if (playing_)
            (*it)->Update(elapsedTime);

        ++it;
    }
}

void NotesSystem::DrawNotes(const Camera* camera)
{
    for (auto& note : notes_)
    {
        note->Draw(camera);
    }
}

void NotesSystem::SetBeatMapDataAndCreateNotes(const BeatMapData& beatMapData)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    beatMapData_ = beatMapData;
    for (const auto& note : beatMapData_.notes)
    {
        if (note.noteType == "normal")
        {
            CreateNormalNote(note.laneIndex, noteSpeed_, note.targetTime);
        }
        else if (note.noteType == "hold")
        {
            CreateLongNote(note);

        }
    }
}


void NotesSystem::Reload()
{
    notes_.clear();
    lane_->Reset();
    //stopwatch_->Reset();
    for (const auto& note : beatMapData_.notes)
    {
        if (note.noteType == "normal")
        {
            CreateNormalNote(note.laneIndex, noteSpeed_, note.targetTime);
        }
        else if (note.noteType == "hold")
        {
            CreateLongNote(note);
        }
    }
    isReloaded_ = true;
    //stopwatch_->Start();

}

bool NotesSystem::IsReloaded()
{
    if (isReloaded_)
    {
        isReloaded_ = false;
        return true;
    }
    return false;
}

void NotesSystem::CreateNormalNote(uint32_t laneIndex, float speed,float targetTime)
{
    if (lane_ == nullptr)        throw std::runtime_error("Lane is not initialized.");

    float elapsedTime = -startOffsetTime_;//stopwatch_->GetElapsedTime<float>();

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(laneIndex);
    laneStartPosition.z = judgeLinePosition_ + (targetTime - elapsedTime) * speed;

    laneStartPosition.y += noteSize_ / 2.0f;
    Vector3 targetPosition = laneStartPosition;
    targetPosition.z = judgeLinePosition_;
    auto note = std::make_shared<NormalNote>();
    note->Initialize(laneStartPosition, targetTime, elapsedTime, targetPosition, laneIndex);

    notes_.emplace_back(note);

    lane_->AddNote(laneIndex, note);
}



void NotesSystem::CreateLongNote(const NoteData& noteData)
{
    if (noteData.holdDuration <= 0.0f)
    {
        throw std::runtime_error("Hold duration must be greater than 0 for long notes.");
        return;
    }

    // ノーツの初期化
    std::shared_ptr<LongNote> note = std::make_shared<LongNote>();

    // 経過時間
    float elapsedTime = -startOffsetTime_;//stopwatch_->GetElapsedTime<float>();

    // ノーツの開始位置を計算
    Vector3 laneStartPosition = lane_->GetLaneStartPosition(noteData.laneIndex);
    // targetTimeから座標
    laneStartPosition.z = judgeLinePosition_ + (noteData.targetTime - elapsedTime) * noteSpeed_;
    laneStartPosition.y += noteSize_ / 2.0f;

    Vector3 targetPosition = laneStartPosition;
    targetPosition.z = judgeLinePosition_;
    // ノーツの初期化
    note->Initialize(laneStartPosition, noteData.targetTime, elapsedTime, targetPosition, noteData.laneIndex);




    std::shared_ptr<LongNote> nextNote = std::make_shared<LongNote>();

    float targetTime = noteData.targetTime + noteData.holdDuration;

    laneStartPosition = lane_->GetLaneStartPosition(noteData.laneIndex);
    laneStartPosition.z = judgeLinePosition_ + (targetTime - elapsedTime) * noteSpeed_;
    laneStartPosition.y += noteSize_ / 2.0f;

    nextNote->Initialize(laneStartPosition, targetTime, elapsedTime, targetPosition, noteData.laneIndex);
    nextNote->SetBeforeNote(note);

    // listとレーンに追加
    notes_.emplace_back(note);
    lane_->AddNote(noteData.laneIndex, note);

    notes_.emplace_back(nextNote);
    lane_->AddNote(noteData.laneIndex, nextNote);
}

std::shared_ptr<Note> NotesSystem::CreateNextNoteForLongNote(uint32_t laneIndex, float speed, float targetTime)
{
    auto nextNote = std::make_shared<LongNote>();

    float elapsedTime = -startOffsetTime_;// stopwatch_->GetElapsedTime<float>();

    Vector3 laneStartPosition = lane_->GetLaneStartPosition(laneIndex);
    laneStartPosition.y += noteSize_ / 2.0f;

    float targetTimeLocal = targetTime;
    laneStartPosition.z = judgeLinePosition_ + targetTimeLocal * speed;
    Vector3 targetPosition = laneStartPosition;
    targetPosition.z = judgeLinePosition_;
    nextNote->Initialize(laneStartPosition, targetTimeLocal, elapsedTime, targetPosition, laneIndex);

    return nextNote;
}

void NotesSystem::DebugWindow()
{
#ifdef _DEBUG

    ImGui::PushID(this);

    ImGui::Checkbox("Playing", &playing_);
    if (ImGui::Button("Reload"))
    {
        Reload();
    }
    ImGui::Checkbox("AutoPlay", &autoPlay_);

    static int laneIndex = 0;
    ImGui::InputInt("LaneIndex", &laneIndex);
    ImGui::DragFloat("NoteSpeed", &noteSpeed_, 0.01f);
    static float targetTime = 0.0f;
    ImGui::DragFloat("TargetTime", &targetTime, 0.01f);

    if (ImGui::Button("CreateNormalNote"))
    {
        float elapseTime = 0;//stopwatch_->GetElapsedTime<float>();
        CreateNormalNote(laneIndex, noteSpeed_, targetTime + elapseTime);
    }

    if (ImGui::Button("CreateLongNote"))
    {
        NoteData data;
        data.holdDuration = 1.0f; // デフォルトのホールド時間
        data.laneIndex = laneIndex;
        data.targetTime = targetTime + 1 + 0;// stopwatch_->GetElapsedTime<float>();
        CreateLongNote(data);

    }

    ImGui::PopID();

#endif // _DEBUG
}
