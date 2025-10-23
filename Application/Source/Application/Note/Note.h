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
    /// <param name="_targetTime">ノーツのターゲット時間（秒）</param>
    /// <param name="_targetPosition">ノーツの目標位置（ワールド座標）</param>
    virtual void Initilize(float _targetTime, const Vector3& _targetPosition);

    /// <summary>
    /// 毎フレームの更新処理。
    /// </summary>
    /// <param name="_elapseTime">経過時間（秒）</param>
    /// <param name="_speed">ノーツの速度係数</param>
    virtual void Update(float _elapseTime, float _speed);

    /// <summary>
    /// 描画処理。
    /// </summary>
    /// <param name="_camera">描画用カメラ</param>
    virtual void Draw(const Camera* _camera);

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
class NomalNote : public Note
{
public:
    NomalNote();
    ~NomalNote() override;


    void Initilize(float _targetTime, const Vector3& _targetPosition) override;
    void Update(float _elapseTime, float _speed) override;
    void Draw(const Camera* _camera) override;

};

// ロングノーツクラス
class LongNote : public Note
{
public:
    LongNote();;
    ~LongNote() override;

    void Initilize(float _targetTime, const Vector3& _targetPosition) override;
    void Update(float _elapseTime, float _speed) override;
    void Draw(const Camera* _camera) override;

    virtual void Judge() override;

    void SetHoldEnd(bool _isHoldEnd);
    bool IsHoldEnd() const { return isHoldEnd_; }

    void SetHoldDuration(float _holdDuration) { holdDuration_ = _holdDuration; }
    float GetHoldDuration() const { return holdDuration_; }

private:

    std::unique_ptr<ObjectModel> noteBridge_ = nullptr; // ノーツブリッジ

    bool isHoldEnd_ = false; // ロングノート終端かどうか
    float holdDuration_ = 0.0f; // ホールド時間

};
