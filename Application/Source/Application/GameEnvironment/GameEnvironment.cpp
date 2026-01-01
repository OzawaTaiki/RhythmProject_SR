#include "GameEnvironment.h"

#include <Core/DXCommon/PSOManager/PSOBuilder.h>
#include <Core/DXCommon/ShaderCompiler/ShaderCompiler.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <Features/Json/Loader/JsonFileIO.h>
#include <Features/Event/EventManager.h>
#include <Utility/StringUtils/StringUitls.h>
#include <Math/MyLib.h>
#include <Math/Easing.h>

namespace
{
Vector4 startColor = Vector4(0.168f, 0.69f, 0.753f, 1.0f);
Vector4 endColor = Vector4(0.272f, 0.280f, 0.502f, 1.0f);
}

GameEnvironment::GameEnvironment()
{
    EventManager::GetInstance()->AddEventListener("SpeakerEffectColorChange", this);
}

GameEnvironment::~GameEnvironment()
{
    EventManager::GetInstance()->RemoveEventListener("SpeakerEffectColorChange", this);
}

void GameEnvironment::Initialize(const std::string& filePath)
{
    GameTime::GetChannel("GameEnvironment");
    Serialize(filePath);
    InitializeOverlayFloor();
    CreateEmissivePSO();

    stopwatch_.Reset();
    stopwatch_.Start();
}

void GameEnvironment::Update(float deltaTime, AudioSpectrum* audioSpectrum)
{
    stopwatch_.Update();
    const float animateTime = 1.0f;
    if (stopwatch_.GetElapsedTime<float>() > animateTime)
    {
        stopwatch_.Reset();

        const int32_t animationMaxStep = 3;
        ++currentAnimationStep_;
        if (currentAnimationStep_ >= animationMaxStep)
            currentAnimationStep_ = 0;
        float progress = static_cast<float>(currentAnimationStep_) / static_cast<float>(animationMaxStep);

        auto& materials = overFloor_->GetMaterials();
        for (auto& material : materials)
        {
            UVTransform& uvTransform = material->GetUVTransform();

            uvTransform.SetOffset(progress, progress);
        }
    }
    UpdateSpeakerAnimation(deltaTime);

    for (auto& obj : environmentObjects_)
    {
        if (obj)
        {
            obj->Update();
        }
    }

    if (audioSpectrum != nullptr)
    {
        for (auto& spectrumBar : spectrumBars_)
        {
            spectrumBar->Update(deltaTime, audioSpectrum);
        }
    }

    overFloor_->Update();
    overlayFloor_->Update();
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
    for (auto& spectrumBar : spectrumBars_)
    {
        spectrumBar->Draw(camera,emissivePso_.Get());
    }

    if(enableEmissive_)
        overFloor_->DrawWithPSO(emissivePso_.Get(), camera);
    else
        overFloor_->Draw(camera);

    overlayFloor_->DrawWithPSO(emissivePso_.Get(),camera, Vector4(0.3f,0.765f,1.0f,0.25f));

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

    animationInterval_ = (60.0f / bpm) * 2.0f; // 二拍に一回
    // 時間スケールをゲームタイムチャネルに設定 アニメーション速度を調整
    GameTime::GetChannel("GameEnvironment").SetGameSpeed(timeScale_ * 1.3f);
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

void GameEnvironment::OnEvent(const GameEvent& event)
{
    if (event.GetEventType() == "SpeakerEffectColorChange")
    {
        auto data = event.GetData();
        if (!data)
            return;

        auto speakerData = dynamic_cast<ColorChangeEvent*>(data);
        if (!speakerData)
            return;

        // スピーカーオブジェクトの色変化タイマーをリセット
        speakerColorTimers_[speakerData->targets] = -speakerData->delayTime;

    }
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

        std::string name = obj["name"].get<std::string>();


        if (StringUtils::Contains(name, "spectrumBar") ||
            StringUtils::Contains(name, "SpectrumBar"))
        {
            auto spectrumBar = std::make_unique<SpectrumBar>();
            std::string  number =  name.substr(name.size()-1, 1);
            int32_t id = std::stoi(number);
            if (spectrumBars_.size() <= id)
            {
                spectrumBars_.resize(id);// 0 からidまでのサイズに拡張
            }
            spectrumBar->Initialize(name, id - 1, obj);
            spectrumBars_[id - 1] = std::move(spectrumBar);
            continue;
        }

        auto object = std::make_unique<ObjectModel>(name);
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
            auto& transform = obj["transform"];

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


        if (name == "overFloor")
        {
            overFloor_ = std::move(object);
            continue;
        }
        if (obj["name"] == "overlayFloor")
        {
            overlayFloor_ = std::move(object);
            continue;
        }
        // スピーカーオブジェクトの検出
        if (StringUtils::Contains(name, "Speaker") ||
            StringUtils::Contains(name, "speaker"))
        {
            BuildSpeakerMap(name, object.get(), filepath); // スピーカーのマップを構築
        }
        if (StringUtils::Contains(name, "wall") ||
            StringUtils::Contains(name, "Wall"))
        {
            InitializeWall(object.get()); // Wallの初期化
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


    auto& materials = model->GetMaterials();
    for (auto& material : materials)
    {
        if (material->GetName() == "diaphragmMaterial") // 振動盤マテリアルのみに適用
        {
            material->SetShininess(100.0f);
            material->SetColor(endColor);
        }
    }

    model->LoadAnimation(filepath, "anim");
}

void GameEnvironment::UpdateSpeakerAnimation(float deltaTime)
{

    // 色変化の計算
#ifdef _DEBUG
    ImGui::Begin("Speaker Color Effect Debug");
    ImGui::ColorEdit3("start", &startColor.x);
    ImGui::ColorEdit3("end", &endColor.x);
    ImGui::DragFloat("interval", &animationInterval_, 0.001f);
    ImGui::Checkbox("Enable Emissive", &enableEmissive_);
    ImGui::End();
#endif // _DEBUG
    for (auto it = speakerColorTimers_.begin(); it != speakerColorTimers_.end(); )
    {
        ObjectModel* speaker = it->first;
        float& timer = it->second;

        timer += deltaTime;
        if (timer < 0.0f)
        {
            ++it;
            continue;
        }

        if(timer - deltaTime < 0.0f)
            speaker->ChangeAnimation("anim", 0.1f, false);


        const float effectDuration = 1.0f; // 色変化の持続時間
        float progress = timer / effectDuration;
        float eased = Easing::EaseInCubic(progress);
        if (eased > 1.0f)
        {
            eased = 1.0f;
        }

        Vector4 currentColor = startColor * (1.0f - eased) + endColor * eased;
        // スピーカーオブジェクトのマテリアルに色を設定
        auto& materials = speaker->GetMaterials();
        for (auto& material : materials)
        {
            if (material->GetName() == "diaphragmMaterial") // 振動盤マテリアルのみに適用
                material->SetColor(currentColor);
        }

        // 効果が終了したらマップから削除
        if (timer >= effectDuration)
        {
            it = speakerColorTimers_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    //// スケールアニメーション
    //// 大きくなって小さくなる を繰り返す
    //animationTimer_ += deltaTime;
    ////TODO: BPMに合わせたアニメーション
    //const float kMaxScale = 1.2f;
    //const float kMinScale = 1.0f;
    //float progress = fmodf(animationTimer_, animationInterval_) / animationInterval_;
    //float easedProgress = Easing::EaseInOutQuad(progress);
    //float scaleValue = 1.0f;
    //if (static_cast<int>(animationTimer_ / animationInterval_) % 2 == 0)
    //{
    //    // 拡大
    //    scaleValue = Lerp(kMinScale, kMaxScale, easedProgress);
    //}
    //else
    //{
    //    // 縮小
    //    scaleValue = Lerp(kMaxScale, kMinScale, easedProgress);
    //}

    //for (auto& [num,obj] : speakerMap_)
    //{
    //    obj->scale_ = Vector3(scaleValue, scaleValue, scaleValue);
    //}
}

void GameEnvironment::InitializeWall(ObjectModel* wallModel)
{
    if (!wallModel)
        return;

    const Vector4 wallColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

    auto& materials = wallModel->GetMaterials();
    for (auto& material : materials)
    {
        material->SetColor(wallColor);
    }
}

void GameEnvironment::InitializeOverlayFloor()
{
    const Vector4 floorColor = Vector4(0.4f, 0.6f, 1.0f, 0.5f);
    auto& materials = overlayFloor_->GetMaterials();
    for (auto& material : materials)
    {
        material->SetColor(floorColor);
    }
}

void GameEnvironment::CreateEmissivePSO()
{
    ShaderCompiler::GetInstance()->Register("EmissivePS", L"EmissiveModel.PS.hlsl", L"ps_6_0");

        auto builder = PSOBuilder::Create();
        emissivePso_=

        builder
        .SetBlendMode(PSOFlags::BlendMode::Normal)
        .SetCullMode(PSOFlags::CullMode::Back)
        .SetDepthMode(PSOFlags::DepthMode::Comb_mAll_fLessEqual)
        .SetShaders("Model_VS", "EmissivePS")
        .SetDepthTest(true)
        .SetDepthWrite(true)
        .UseModelInputLayout()
        .SetRootSignature(PSOManager::GetInstance()->GetRootSignature(PSOFlags::Type::Model).value())
        .Build();

}
