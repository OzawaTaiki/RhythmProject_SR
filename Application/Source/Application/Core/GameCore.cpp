#include "GameCore.h"

#include <Debug/Debug.h>
#include <Debug/ImGuiDebugManager.h>
#include <Features/LineDrawer/LineDrawer.h>
#include <Features/Model/Manager/ModelManager.h>
#include <Features/TextRenderer/FontCache.h>
#include <Features/TextRenderer/Text3DRenderer.h>
#include <Math/MyLib.h>
#include <System/Input/TextInputManager.h>
#include <Utility/ConvertString/ConvertString.h>

using namespace Engine;

GameCore::GameCore(int32_t laneCount)
    : laneCount_(laneCount), isWaitingForStart_(true), waitTimer_(0.0f),
      noteDeletePosition_(0.0f), maxCombo_(0), combo_(0) {
  lanes_.resize(laneCount);
}

GameCore::~GameCore() {}

void GameCore::Initialize(float noteSpeed, float musicLatencyMs,
                          float beginOffset) {
  noteSpeed_ = noteSpeed;
  beginOffset_ = beginOffset;
  musicLatencyMs_ = musicLatencyMs;
  waitTimer_ = 0.0f;
  isWaitingForStart_ = true;

  // レーンの初期化
  lanes_.resize(laneCount_);
  for (int32_t i = 0; i < laneCount_; ++i) {
    lanes_[i] = std::make_unique<Lane>();
  }

  noteJudge_ = std::make_unique<NoteJudge>();
  noteJudge_->Initialize();

  judgeResult_ = std::make_unique<JudgeResult>();
  judgeResult_->Initialize();

  noteJudge_->SetLaneTotalWidth(Lane::GetTotalWidth());
  noteJudge_->SetSpeed(noteSpeed);

  noteDeletePosition_ = -noteJudge_->GetMissJudgeThreshold() *
                        noteSpeed_; // ノーツを削除する位置を設定

  combo_ = 0;    // コンボの初期化
  maxCombo_ = 0; // 最大コンボの初期化
}

void GameCore::Update(float deltaTime,
                      const std::vector<InputData> &inputData) {
#ifdef _DEBUG

  static bool autoMode = false;
  if (ImGuiDebugManager::GetInstance()->Begin("GameCore")) {
    ImGui::Checkbox("Auto Mode", &autoMode);
    ImGui::End();
  }
#endif // _DEBUG

  for (auto &lane : lanes_) {
    int32_t deleteCount = lane->DeleteNotesOutOfScreen(
        noteDeletePosition_); // 画面外に出たノーツ数を取得
    judgeResult_->AddJudge(
        JudgeType::Miss,
        deleteCount); // 削除されたノーツの数をミスとしてカウント
    if (deleteCount > 0) {
      combo_ = 0; // ノーツが削除されたらコンボをリセット
      if (onMissCallback_) {
        onMissCallback_(); // ミス時のコールバックを呼び出す
      }
    }
  }
#ifdef _DEBUG

  if (autoMode) {
    JudgeNotesInAutoMode(gamemusic_->GetElapsedTime());
  } else {
    JudgeNotes(inputData);
  }
#else

  // ノーツの判定処理
  JudgeNotes(inputData);

#endif // _DEBUG

  float elapsedTime = 0.0f;
  if (isWaitingForStart_) { // 開始前オフセット待機中
    waitTimer_ += deltaTime;
    elapsedTime = Lerp(-beginOffset_, 0.0f, waitTimer_ / beginOffset_);
  } else if (gamemusic_) {
    // 音楽の音声インスタンスが有効な場合、経過時間を取得
    if (gamemusic_->IsPlaying()) {
      // 音楽が再生中の場合、経過時間を取得
      elapsedTime = gamemusic_->GetElapsedTime();
    }
  }
  for (auto &lane : lanes_) {
    lane->Update(elapsedTime + musicLatencyMs_ / 1000.0f, noteSpeed_);
  }
}

void GameCore::Draw(const Camera *camera,
                    const std::map<int32_t, uint8_t> &keyBindings) {
  if (keyBindings.size() != lanes_.size()) {
    Debug::Log("KeyBindings size does not match lane count.\n");
    assert(false && "KeyBindings size does not match lane count.");
    return;
  }

  auto renderer = Text3DRenderer::GetInstance();
  auto textInputManager = TextInputManager::GetInstance();
  AtlasData *atlasData = FontCache::GetInstance()->GetAtlasData(
      "Resources/Fonts/NotoSansJP-Regular.ttf", 128);
  wchar_t wchar;
  for (size_t i = 0; i < lanes_.size(); ++i) {
    if (lanes_[i]) {
      ModelManager::GetInstance()->PreDrawForObjectModel();
      lanes_[i]->Draw(camera);
      // レーンのキー表示
      uint8_t kc = keyBindings.at(static_cast<int32_t>(i));
      textInputManager->ConvertDIKeyToChar(kc, true, wchar);
      std::wstring wStr(1, wchar);
      renderer->DrawText3D(
          wStr, atlasData, camera, lanes_[i]->GetStartPosition(), Vector3::zero,
          Vector2(1.0f, 1.0f), Vector4(1, 1, 1, 0.5f), Vector2(0.5f, 0.0f));
    }
  }
  // 判定ラインの描画
#ifdef _DEBUG
  LineDrawer::GetInstance()->RegisterPoint(Vector3(-4, 0, 0), Vector3(4, 0, 0));

  noteJudge_->DrawJudgeLine();
#endif // _DEBUG
}

void GameCore::GenerateNotes(const BeatMapData &beatMapData) {
  // 譜面データを解析してノーツを生成
  ParseBeatMapData(beatMapData);

  int32_t totalNotes = 0;
  for (const auto &lane : lanes_) {
    totalNotes += static_cast<int32_t>(lane->GetNoteCount());
  }
  scoreCalculator_.Initialize(totalNotes);
}

void GameCore::JudgeNotes(const std::vector<InputData> &inputData) {
  for (auto &inputdata : inputData) {
    // レーンインデックスの範囲チェック
    int32_t laneIndex = inputdata.laneIndex;
    if (laneIndex < 0 || laneIndex >= static_cast<int32_t>(lanes_.size())) {
      // エラー処理: レーンインデックスが範囲外
      throw std::out_of_range("InputData laneIndex is out of range.");
      return;
    }

    // 各レーンの最初のノーツを取得して判定を行う
    auto &lane = lanes_[laneIndex];
    auto note = lane->GetFirstNote();
    if (!note)
      continue;

    JudgeType result = JudgeType::None; // 初期化
    NoteType noteType =
        note->GetNoteType(); // ノーツの種類に応じて判定処理を分岐
    switch (noteType) {
    case NoteType::Normal:
      result = ProcessNormalNote(note, inputdata);
      break;
    case NoteType::Hold:
      result = ProcessHoldNote(note, inputdata, lane.get());
      break;
    case NoteType::HoldEnd:
      result = ProcessHoldEndNote(note, inputdata, lane.get());
      break;
    default:
      break;
    }
    if (result == JudgeType::None)
      continue; // 判定なしの場合はスキップ

    RecordJudgeResult(result, note);           // 判定結果を記録
    scoreCalculator_.AddScore(result, combo_); // スコア計算に反映
    UpdateCombo(result);

    if (onJudgeCallback_)
      onJudgeCallback_(laneIndex, result,
                       combo_); // 判定時のコールバックを呼び出す
  }
}

void GameCore::JudgeNotesInAutoMode(float elapsedTime) {
  for (size_t laneIndex = 0; laneIndex < lanes_.size(); ++laneIndex) {
    auto &lane = lanes_[laneIndex];
    auto note = lane->GetFirstNote();
    if (!note)
      continue;

    JudgeType result = noteJudge_->ProcessNoteJudge(
        note, elapsedTime + musicLatencyMs_ / 1000.0f);
    if (result != JudgeType::Perfect)
      continue; // 判定なしの場合はスキップ

    RecordJudgeResult(result, note);           // 判定結果を記録
    scoreCalculator_.AddScore(result, combo_); // スコア計算に反映
    UpdateCombo(result);

    if (onJudgeCallback_)
      onJudgeCallback_(static_cast<int32_t>(laneIndex), result,
                       combo_); // 判定時のコールバックを呼び出す
  }
}

JudgeType GameCore::ProcessNormalNote(Note *note, const InputData &inputData) {
  // 押した瞬間だけ判定
  if (inputData.state == KeyState::Trigger) {
    return noteJudge_->ProcessNoteJudge(note, inputData.elapsedTime +
                                                  musicLatencyMs_ / 1000.0f);
  }

  return JudgeType::None;
}

JudgeType GameCore::ProcessHoldNote(Note *note, const InputData &inputData,
                                    Lane *lane) {
  // 押した瞬間だけ判定
  if (inputData.state == KeyState::Trigger) {
    JudgeType result = noteJudge_->ProcessNoteJudge(
        note, inputData.elapsedTime + musicLatencyMs_ / 1000.0f);

    if (result != JudgeType::None && result != JudgeType::Miss) {
      // holdState_.StartHold(inputData.laneIndex); // ホールド状態を開始
      lane->StartHold();
      Debug::Log("Hold Enable\n");
    }

    return result;
  }

  return JudgeType::None;
}

JudgeType GameCore::ProcessHoldEndNote(Note *note, const InputData &inputData,
                                       Lane *lane) {
  if (inputData.state == KeyState::Trigger) {
    lane->StartHold();
  } else if (lane->IsHolding() &&
             /*holdState_.IsHoldingLane(inputData.laneIndex)*/ true) {
    Debug::Log("Hold State is holding lane: " +
               std::to_string(inputData.laneIndex) + "\n");

    if (onHoldCallback_)
      onHoldCallback_(
          inputData.laneIndex); // ホールド時のコールバックを呼び出す

    if (inputData.state == KeyState::Released) {
      JudgeType result = noteJudge_->ProcessNoteJudge(
          note, inputData.elapsedTime + musicLatencyMs_ / 1000.0f);

      lane->EndHold();
      // holdState_.EndHold(); // ホールド状態を終了
      //  noneてことはブリッジ内なので
      if (result == JudgeType::None) {
        // コンボを途切れさせる
        // 判定は反映させない
        UpdateCombo(result);
      }

      return result;
    }
  }

  return JudgeType::None;
}

void GameCore::UpdateCombo(JudgeType result) {
  if (result == JudgeType::Perfect || result == JudgeType::Good ||
      result == JudgeType::Bad) {
    ++combo_;                                  // コンボを増やす
    maxCombo_ = (std::max)(maxCombo_, combo_); // 最大コンボを更新
  } else {
    combo_ = 0; // コンボが途切れたらリセット
    if (onMissCallback_) {
      onMissCallback_(); // ミス時のコールバックを呼び出す
    }
  }
}

void GameCore::RecordJudgeResult(JudgeType result, Note *note) {
  if (result == JudgeType::None)
    return;
  // 判定結果を記録
  judgeResult_->AddJudge(result);
  note->Judge();
}

Rank GameCore::GetRank() const {
  return RankCalculator::CalculateRank(scoreCalculator_.GetDisplayScore(),
                                       scoreCalculator_.GetBaseScore());
}

void GameCore::ParseBeatMapData(const BeatMapData &beatMapData) {
  notesPerLane_.clear(); // 既存のデータをクリア

  // レーンごとのでーたに分ける
  notesPerLane_.resize(lanes_.size());

  for (const auto &note : beatMapData.notes) {
    if (note.laneIndex >= notesPerLane_.size()) {
      // エラー処理: レーンインデックスが範囲外
      throw std::out_of_range("Note laneIndex is out of range.");
      return;
    }

    notesPerLane_[note.laneIndex].push_back(note);
  }

  // 時間でソート
  for (auto &notes : notesPerLane_) {
    notes.sort([](const NoteData &a, const NoteData &b) {
      return a.targetTime < b.targetTime; // 昇順でソート
    });
  }

  CreateBeatMapNotes();
}

void GameCore::CreateBeatMapNotes() {
  for (int32_t index = 0; index < notesPerLane_.size(); ++index) {
    lanes_[index]->Initialize(
        notesPerLane_[index], index, 0.0f, noteSpeed_,
        beginOffset_); // TODO 仮の値を使用している 判定ラインの座標
  }
}

void GameCore::Restart() {
  isWaitingForStart_ = true;
  waitTimer_ = 0.0f;

  combo_ = 0;
  maxCombo_ = 0;

  scoreCalculator_.ScoreReset();

  CreateBeatMapNotes();
  judgeResult_->Initialize();
}
