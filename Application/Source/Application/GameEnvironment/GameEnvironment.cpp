#include "GameEnvironment.h"

#include <Features/Json/Loader/JsonFileIO.h>
#include <Utility/StringUtils/StringUitls.h>

void GameEnvironment::Initialize(const std::string& filePath)
{
    GameTime::GetChannel("GameEnvironment");
    Serialize(filePath);

    spriteSheetAnimation_ = SpriteSheetAnimation(4, 1, 4, 1.0f);
    UVTransform& uvTransform = overFloor_->GetMaterial()->GetUVTransform();
    spriteSheetAnimation_.AddTransform(&uvTransform);
    spriteSheetAnimation_.Play();
    spriteSheetAnimation_.SetDuration(4.0f);
    spriteSheetAnimation_.SetLooping(true);
}

void GameEnvironment::Update(float deltaTime)
{
    spriteSheetAnimation_.Update(deltaTime);

    for (auto& obj : environmentObjects_)
    {
        if (obj)
        {
            obj->Update();
        }
    }
    overFloor_->Update();
    screen_->Update();
}

void GameEnvironment::Draw(const Camera* camera)
{
    for (auto& obj : environmentObjects_)
    {
        if (obj)
        {
            obj->Draw(camera);
        }
    }
    screen_->Draw(camera, spectrumTextureHandle_, Vector4(1, 1, 1, 1));

    overFloor_->Draw(camera, Vector4(1, 1, 1, 1));
}

void GameEnvironment::SetBPM(float bpm)
{
    // BPMに基づいて時間スケールを設定

    const float kSpeakerSwingBpmThreshold = 160.0f;// スピーカーのアニメーション速度を変更する閾値

    timeScale_ = 1.0f / (60.0f / bpm);
    // スピーカーのアニメーション速度を調整 早くなりすぎないようにスケーリング。
    if (bpm < kSpeakerSwingBpmThreshold)
        timeScale_ *= 0.5f;
    else
        timeScale_ *= 0.25f;

    // 時間スケールをゲームタイムチャネルに設定 アニメーション速度を調整
    GameTime::GetChannel("GameEnvironment").SetGameSpeed(timeScale_);

}

ObjectModel* GameEnvironment::GetSpeaker(uint32_t laneIndex)
{
    auto it = speakerMap_.find(laneIndex);
    if (it != speakerMap_.end())
    {
        return it->second; // レーンインデックスに対応するスピーカーオブジェクトを返す
    }
    return nullptr; // 見つからなかった場合はnullptrを返す
}

void GameEnvironment::StartAnimation()
{
}

void GameEnvironment::Serialize(const std::string& filePath)
{
    json data = JsonFileIO::Load(filePath, "");

    if (!data.contains("name"))
        return; // 名前がない場合は終了
    if (data["name"] != "scene")
        return; // シーンデータでない場合は終了
    if (!data.contains("objects"))
        return; // オブジェクトデータがない場合は終了

    int32_t objectCount = 0;

    for (json obj : data["objects"])
    {
        if (!obj.contains("type") || obj["type"] != "MESH")
            continue;

        if (!obj.contains("name") || obj["name"].empty())
            obj["name"] = "obj" + std::to_string(objectCount++); // 名前がない場合は自動で名前を設定

        if (obj["name"] == "overlayFloor")
        {
            continue;
        }

        auto object = std::make_unique<ObjectModel>(obj["name"].get<std::string>());
        std::string filepath = "";
        // モデルファイル名の取得
        if (obj.contains("file_name") && !obj["file_name"].empty())
            filepath = obj["file_name"].get<std::string>();

        if (filepath.empty())
            filepath = "cube/cube.obj"; // デフォルトのモデルファイルパス

        object->Initialize(filepath); // モデルの初期化

        // Transformの設定
        Vector3 scale, rotation, translation;
        if (obj.contains("transform"))
        {
            auto transform = obj["transform"];

            if (transform.contains("scale"))
                scale = Vector3(transform["scale"][0], transform["scale"][1], transform["scale"][2]);
            if (transform.contains("rotation"))
                rotation = Vector3(transform["rotation"][0], transform["rotation"][1], transform["rotation"][2]);
            if (transform.contains("transform"))
                translation = Vector3(transform["transform"][0], transform["transform"][1], transform["transform"][2]);
        }
        else
        {
            scale = Vector3(1.0f, 1.0f, 1.0f); // デフォルトのスケール
            rotation = Vector3(0.0f, 0.0f, 0.0f); // デフォルトの回転
            translation = Vector3(0.0f, 0.0f, 0.0f); // デフォルトの位置
        }

        object->scale_ = scale;
        object->quaternion_ = Quaternion::EulerToQuaternion(rotation); // 回転をクォータニオンに変換
        object->translate_ = translation;
        object->useQuaternion_ = true; // クォータニオンを使用するように設定

        //object->GetMaterial()->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // デフォルトの色を設定
        object->SetTimeChannel("GameEnvironment"); // アニメーションの時間チャンネルを設定


        if (obj["name"] == "overFloor")
        {
            overFloor_ = std::move(object);
            continue;
        }
        if (obj["name"] == "screen")
        {
            screen_ = std::move(object);
            continue;
        }
        // スピーカーオブジェクトの検出
        if (StringUtils::Contains(obj["name"].get<std::string>(), "Speaker")||
            StringUtils::Contains(obj["name"].get<std::string>(), "speaker"))
        {
            BuildSpeakerMap(obj["name"].get<std::string>(), object.get(), filepath); // スピーカーのマップを構築
        }
        environmentObjects_.push_back(std::move(object)); // オブジェクトを追加

    }

}

void GameEnvironment::BuildSpeakerMap(const std::string& objName, ObjectModel* model, const std::string& filepath)
{
    if (!StringUtils::Contains(objName, "lane"))
        return;


    // 最後のアンダースコア以降の文字列を取得 数字のはず
    std::string name = StringUtils::GetAfterLast(objName, '_'); // '_'以降の文字列を取得

    if (name.empty())
        return; // 名前が空の場合はスキップ

    uint32_t laneIndex = static_cast<uint32_t>(std::stoi(name)); // 文字列を整数に変換
    speakerMap_[laneIndex] = model; // レーンインデックスとモデルをマップに追加

    model->LoadAnimation(filepath,"anim");
}
