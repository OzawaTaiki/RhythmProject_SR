#include "Lane.h"

#include <Features/Camera/Camera/Camera.h>

float Lane::laneLength_ = 100.0f; // 初期値
float Lane::totalWidth_ = 8.0f; // 初期値
int32_t Lane::laneCount_ = 4; // 初期値
float Lane::laneWidth_ = Lane::totalWidth_ / static_cast<float>(Lane::laneCount_); // 一本あたりの幅

Lane::~Lane()
{
    notes_.clear(); // 明示的にクリア
}

void Lane::Initialize(const std::list<NoteData>& noteDataList, int32_t laneIndex, float judgeLine, float  speed, float startOffsetTime)
{
    endPosition_ = GetLaneEndPosition(laneIndex, judgeLine); // レーンの開始位置を取得

    startPosition_ = endPosition_;
    startPosition_.z += laneLength_;


    CreateLaneModel();

    // ノーツを生成
    CreateNotes(noteDataList, laneIndex, judgeLine, speed, startOffsetTime);

}

void Lane::Update(float elapseTime, float speed)
{
    auto it = notes_.begin();
    if (isHolding_)
    {
        // ホールド中なら最初のノーツがホールド終端か確認してヘッドを押したことにする
        if (it != notes_.end() && (*it)->GetNoteType() == NoteType::HoldEnd)
        {
            auto holdNote = std::dynamic_pointer_cast<LongNote>(*it);
            if (holdNote)
            {
                holdNote->HeadPressed();
            }
        }
    }
    for (; it != notes_.end();)
    {
        auto& note = *it;
        if (note->IsJudged())
        {
            it = notes_.erase(it); // 判定済みのノーツは削除
            continue;
        }

        note->Update(elapseTime,speed);
        ++it; // 次のノーツへ
    }

}


void Lane::Draw(const Camera* camera) const
{
    if (laneModel_)
    {
        laneModel_->Draw(camera, { 0.5f,0.5f,0.5f,0.7f });
    }

    for (const auto& note : notes_)
    {
        note->Draw(camera);
    }
}

Note* Lane::GetFirstNote() const
{
    if (notes_.empty())
    {
        return nullptr; // ノーツがない場合はnullptrを返す
    }

    return notes_.front().get(); // 最初のノーツを返す
}

int32_t Lane::DeleteNotesOutOfScreen(float noteDeletePos)
{
    int32_t deleteCount = 0;
    for (auto it = notes_.begin(); it != notes_.end();)
    {
        auto& note = *it;
        if (note->GetPosition().z < noteDeletePos) // 画面外のノーツ
        {
            note->Judge(); // ノーツを判定済みにする 念のため
            it = notes_.erase(it); // ノーツを削除
            ++deleteCount; // 削除カウントを増やす
        }
        else
        {
            // 時間順に並んでいるからここにきたら以降のノートは画面内にあるはず
            break;
        }
    }
    return deleteCount; // 削除したノーツの数を返す

}

void Lane::StartHold()
{
    isHolding_ = true;
}

void Lane::EndHold()
{
    isHolding_ = false;
}

Vector3 Lane::GetLaneEndPosition(int32_t laneIndex, float judgeLine)
{
    // レーンの座標を計算
    // 基準は(0,0,judgeLine)
    static Vector3 basePosition = Vector3(0.0f, 0.0f, judgeLine);
    static float laneLeftEdge = basePosition.x - (totalWidth_ / 2.0f);

    Vector3 endPosition;

    endPosition.x = laneLeftEdge + (static_cast<float>(laneIndex) * laneWidth_) + laneWidth_ / 2.0f;
    endPosition.y = 0.0f; // 高さは0
    endPosition.z = basePosition.z; // 奥

    return endPosition; // レーンの開始位置を返す
}

void Lane::CreateNotes(const std::list<NoteData>& noteDataList, int32_t laneIndex, float judgeLine, [[maybe_unused]] float  speed, [[maybe_unused]] float startOffsetTime)
{
    notes_.clear();

    Vector3 noteTargetPosition = endPosition_;
    startPosition_.z = judgeLine; // レーンの開始位置のZ座標を判定ラインに合わせる
    startPosition_.y = 0.0f; // 高さは0
    Vector3 noteStartPosition = startPosition_;


    // noteDataListは時間でソート済み
    for (const auto& noteData : noteDataList)
    {
        if (noteData.laneIndex == laneIndex) // 念のためチェック
        {
            // ノーツを生成して追加
            if(noteData.noteType =="normal")
            {
                auto note = std::make_shared<NormalNote>();

                note->Initialize(noteData.targetTime ,noteTargetPosition);
                notes_.push_back((note));
            }
            else if (noteData.noteType == "hold")
            {
                // 前のノートを生成
                auto note = std::make_shared<LongNote>();
                note->Initialize(noteData.targetTime, noteTargetPosition);
                note->SetHoldEnd(false); // ブリッジはまだ生成しない
                notes_.push_back((note));

                // ホールド終端を生成
                auto holdEndNote = std::make_shared<LongNote>();
                holdEndNote->Initialize(noteData.targetTime + noteData.holdDuration, noteTargetPosition);
                holdEndNote->SetHoldEnd(true); // ブリッジを生成するフラグを立てる
                holdEndNote->SetHoldDuration(noteData.holdDuration); // ホールド時間を設定
                notes_.push_back((holdEndNote));
            }
        }
    }
}

void Lane::CreateLaneModel()
{
    laneModel_ = std::make_unique<ObjectModel>("Lane");
    laneModel_->Initialize("pY1x1p01Plane");
    laneModel_->scale_ = Vector3(laneWidth_ - 0.1f, 1.0f, laneLength_); // レーンの幅と長さを設定
    laneModel_->translate_ = startPosition_;
    laneModel_->translate_.y -= 0.01f; // zファイティング対策で少し下げる
    laneModel_->Update();
}
