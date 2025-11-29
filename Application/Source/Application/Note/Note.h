#pragma once

#include <Features/Camera/Camera/Camera.h>
#include <Features/Model/ObjectModel.h>

#include <Application/Note/NoteType.h>


#include <cstdint>

/// <summary>
/// ノーツの基底クラス。
/// </summary>
class Note
{
public:
    Note() = default;
    virtual ~Note() = default;

    /// <summary>
    /// ノーツを初期化する。
    /// </summary>
    /// <param name="targetTime">ノーツのターゲット時間（秒）</param>
    /// <param name="targetPosition">ノーツの目標位置（ワールド座標）</param>
    virtual void Initialize(float targetTime, const Vector3& targetPosition);

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    /// <param name="elapseTime">経過時間（秒）</param>
    /// <param name="speed">ノーツの速度係数</param>
    virtual void Update(float elapseTime, float speed);

    /// <summary>
    /// 描画処理。
    /// </summary>
    /// <param name="camera">描画用カメラ</param>
    virtual void Draw(const Camera* camera);

    /// <summary>
    /// ノーツのターゲット時間を取得する。
    /// </summary>
    float GetTargetTime() const { return targetTime_; }

    /// <summary>
    /// ノーツの現在位置（ワールド座標）を取得する。
    /// </summary>
    Vector3 GetPosition() const { return model_->translate_; }

    /// <summary>
    /// ノーツの種類を取得する。
    /// </summary>
    NoteType GetNoteType() const { return noteType_; }

    /// <summary>
    /// ノーツが判定済みかを返す。
    /// </summary>
    bool IsJudged() const { return isJudged_; }

    /// <summary>
    /// ノーツの判定処理を行う（派生クラスでオーバーライド）。
    /// </summary>
    virtual void Judge();

protected:
    std::unique_ptr<ObjectModel> model_ = nullptr;
    Vector4 color_ = { 1,1,1,1 };
    float targetTime_ = 0.0f;
    Vector3 targetPosition_ = { 0,0,0 }; // ノーツの目標位置
    bool isJudged_ = false;
    NoteType noteType_ = NoteType::Normal; // ノーツのタイプ

};

// 通常ノーツクラス
class NormalNote : public Note
{
public:
    NormalNote();
    ~NormalNote() override;


    void Initialize(float targetTime, const Vector3& targetPosition) override;
    void Update(float elapseTime, float speed) override;
    void Draw(const Camera* camera) override;

};

// ロングノーツクラス
class LongNote : public Note
{
public:
    LongNote();;
    ~LongNote() override;

    void Initialize(float targetTime, const Vector3& targetPosition) override;
    void Update(float elapseTime, float speed) override;
    void Draw(const Camera* camera) override;

    virtual void Judge() override;

    void SetHoldEnd(bool isHoldEnd);
    bool IsHoldEnd() const { return isHoldEnd_; }

    void SetJudgePosition(const Vector3& judgePosition) { judgePosition_ = judgePosition; }

    void SetHoldDuration(float holdDuration) { holdDuration_ = holdDuration; }
    float GetHoldDuration() const { return holdDuration_; }

    void HeadPressed() { isHeadPressed_ = true; }

private:

    std::unique_ptr<ObjectModel> noteBridge_ = nullptr; // ノーツブリッジ

    Vector3 judgePosition_ = { 0,0,0 }; // 判定位置
    bool isHoldEnd_ = false; // ロングノート終端かどうか
    float holdDuration_ = 0.0f; // ホールド時間
    bool isHeadPressed_ = false; // ヘッド(始端ノーツ)が押されているか


};
