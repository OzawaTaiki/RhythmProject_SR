#include "GameEnvironment.h"

#include <Features/Json/Loader/JsonFileIO.h>
#include <Utility/StringUtils/StringUitls.h>

void GameEnvironment::Initialize(const std::string& _filePath)
{
    GameTime::GetChannel("GameEnvironment");
    Serialize(_filePath);

    spriteSheetAnimation_ = SpriteSheetAnimation(4, 1, 4, 1.0f);
    UVTransform& uvTransform = overFloor_->GetMaterial()->GetUVTransform();
    spriteSheetAnimation_.AddTransform(&uvTransform);
    spriteSheetAnimation_.Play();
    spriteSheetAnimation_.SetDuration(4.0f);
    spriteSheetAnimation_.SetLooping(true);
}

void GameEnvironment::Update(float _deltaTime)
{
    spriteSheetAnimation_.Update(_deltaTime);

    for (auto& obj : environmentObjects_)
    {
        if (obj)
        {
            obj->Update();
        }
    }
    overFloor_->Update();

}

void GameEnvironment::Draw(const Camera* _camera)
{
    for (auto& obj : environmentObjects_)
    {
        if (obj)
        {
            obj->Draw(_camera);
        }
    }
    overFloor_->Draw(_camera, Vector4(1, 1, 1, 1));

}

void GameEnvironment::SetBPM(float _bpm)
{
    // BPMに基づいて時間スケールを設定

    const float kSpeakerSwingBpmThreshold = 160.0f;// スピーカーのアニメーション速度を変更する閾値

    timeScale_ = 1.0f / (60.0f / _bpm);
    // スピーカーのアニメーション速度を調整 早くなりすぎないようにスケーリング。
    if (_bpm < kSpeakerSwingBpmThreshold)
        timeScale_ *= 0.5f;
    else
        timeScale_ *= 0.25f;

    GameTime::GetChannel("GameEnvironment").SetGameSpeed(timeScale_);

}

void GameEnvironment::StartAnimation()
{
}

void GameEnvironment::Serialize(const std::string& _filePath)
{
    json data = JsonFileIO::Load(_filePath, "");

    if (!data.contains("name"))
        return;
    if (data["name"] != "scene")
        return;
    if (!data.contains("objects"))
        return;

    int32_t objectCount = 0;

    for (json obj : data["objects"])
    {
        if (!obj.contains("type") || obj["type"] != "MESH")
            continue;

        if (!obj.contains("name") || obj["name"].empty())
            obj["name"] = "obj" + std::to_string(objectCount++); // 名前がない場合は自動で名前を設定

        if (StringUtils::Contains(obj["name"].get<std::string>(), "lane"))
            continue; // レーンのオブジェクトはスキップ

        if (obj["name"] == "overlayFloor")
        {
            continue;
        }


        auto object = std::make_unique<ObjectModel>(obj["name"].get<std::string>());
        std::string filepath = "";

        if (obj.contains("file_name") && !obj["file_name"].empty())
            filepath = obj["file_name"].get<std::string>();

        if (filepath.empty())
            filepath = "cube/cube.obj";
        object->Initialize(filepath); // モデルの初期化

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
        object->euler_ = rotation;
        object->translate_ = translation;
        //object->GetMaterial()->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // デフォルトの色を設定
        object->SetTimeChannel("GameEnvironment"); // アニメーションの時間チャンネルを設定


        if (obj["name"] == "overFloor")
        {
            overFloor_ = std::move(object);
            continue;
        }
        environmentObjects_.push_back(std::move(object)); // オブジェクトを追加

    }

}
