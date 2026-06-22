#include "GameEnvironment.h"

#include <Core/DXCommon/PSOManager/PSOBuilder.h>
#include <Core/DXCommon/ShaderCompiler/ShaderCompiler.h>
#include <Framework/LayerSystem/LayerSystem.h>
#include <Features/Json/Loader/JsonFileIO.h>
#include <Features/Event/EventManager.h>
#include <Utility/StringUtils/StringUitls.h>
#include <Math/MyLib.h>
#include <Math/Easing.h>
#include <Features/WaveformDisplay/WaveformAnalyzer.h>

#ifdef _DEBUG
#include <Debug/ImGuiDebugManager.h>
#endif

using namespace Engine;


namespace
{
Vector4 startColor = Vector4(0.168f, 0.69f, 0.753f, 1.0f);
Vector4 endColor = Vector4(0.272f, 0.280f, 0.502f, 1.0f);
}

GameBackground::GameBackground()
{
    EventManager::GetInstance()->AddEventListener("SpeakerEffectColorChange", this);
}

GameBackground::~GameBackground()
{
    EventManager::GetInstance()->RemoveEventListener("SpeakerEffectColorChange", this);
}

void GameBackground::Initialize(const std::string& filePath)
{
    GameTime::GetChannel("GameBackground");
    Serialize(filePath);

    spectrumFloor_ = std::make_unique<SpectrumFloor>();
    spectrumFloor_->Initialize(8, 8, Vector2(32.0f, 30.0f));

    InitializeOverlayFloor();
    CreateEmissivePSO();
    CreateBeamPSO();
    InitializeFFTReaction();

    stopwatch_.Reset();
    stopwatch_.Start();

    onsetDetector_->SetThreshold(1.0f);
}

void GameBackground::Update(float deltaTime, AudioSpectrum* audioSpectrum, SoundInstance* soundInstance, float duration)
{
    stopwatch_.Update();
    const float animateTime = 0.4f;
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

    for (auto& obj : backgroundObjects_)
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

    //overFloor_->Update();
    spectrumFloor_->Update(deltaTime, audioSpectrum, soundInstance, duration);
    overlayFloor_->Update();

    // ---- FFT連動演出 ----
    // オンセット/ドロップ検出
    if (onsetDetector_)
    {
        onsetDetector_->Update(audioSpectrum, deltaTime);

        if (onsetDetector_->ConsumeDrop())
        {
            for (auto& reaction : speakerReactions_)
                reaction->AddBoost(0.6f);
            for (auto& beam : ceilingBeams_)
            {
                beam->Burst(1.0f);
                beam->ShiftHue(80.0f); // ドロップで色相をパッとずらしてキメる
            }
            for (auto& beam : backBeams_)
                beam->Burst(1.0f);
        }
    }

    // 各スピーカーの周波数帯反応（ノーツ叩き中はFFT発光を抑制）
    {
        size_t i = 0;
        for (auto& [laneIndex, speaker] : speakerMap_)
        {
            if (i >= speakerReactions_.size())
                break;
            bool noteHitActive = (speakerColorTimers_.find(speaker) != speakerColorTimers_.end());
            speakerReactions_[i]->Update(deltaTime, audioSpectrum, noteHitActive);
            ++i;
        }
    }

    // しきい値で 0..1 に引き直す共通処理（floor以下は0、floor〜1を0〜1へ）
    auto applyFloor = [](float gained, float floor) -> float
    {
        if (gained <= floor)
            return 0.0f;
        return std::clamp((gained - floor) / (1.0f - floor), 0.0f, 1.0f);
    };

    // 天井ビーム：音圧（RMS）で常時ドライブ
    float energyA = 0.0f;
    if (soundInstance)
    {
        float rms = WaveformAnalyzer::GetRMSAtTime(soundInstance, duration);
        energyA = applyFloor(rms * beamEnergyGain_, beamEnergyFloor_);
    }

    // 奥ビーム：高域エネルギー（ハイハット等）でドライブ
    float energyC = 0.0f;
    float dominantBand = 0.0f; // 0=低域優勢 .. 1=高域優勢
    if (audioSpectrum)
    {
        size_t hiBegin = 0, hiEnd = 0;
        audioSpectrum->GetSpectrumIndexRange(FrequencyBands::kHighMid, FrequencyBands::kHigh, hiBegin, hiEnd);
        std::vector<float> hi;
        audioSpectrum->GetAmplitudesInRange(hiBegin, hiEnd, hi);
        float hiPeak = hi.empty() ? 0.0f : *std::max_element(hi.begin(), hi.end());
        energyC = applyFloor(hiPeak * backBeamGain_, backBeamFloor_);

        // 支配帯域：低域ピーク vs 高域ピーク の比で色相を寄せる
        size_t loBegin = 0, loEnd = 0;
        audioSpectrum->GetSpectrumIndexRange(FrequencyBands::kSubBass, FrequencyBands::kLowMid, loBegin, loEnd);
        std::vector<float> lo;
        audioSpectrum->GetAmplitudesInRange(loBegin, loEnd, lo);
        float loPeak = lo.empty() ? 0.0f : *std::max_element(lo.begin(), lo.end());
        float sum = loPeak + hiPeak;
        dominantBand = (sum > 1e-6f) ? (hiPeak / sum) : 0.0f;
    }

    for (auto& beam : ceilingBeams_)
    {
        beam->SetEnergy(energyA);
        beam->SetDominantBand(dominantBand);
        beam->SetComboRatio(comboColorRatio_);
        beam->Update(deltaTime);
    }
    for (auto& beam : backBeams_)
    {
        beam->SetEnergy(energyC);
        beam->Update(deltaTime);
    }

    // 手前のペンライト群更新（低〜中域FFTで揺れる）
    if (penlightCrowd_)
        penlightCrowd_->Update(deltaTime, audioSpectrum);

#ifdef _DEBUG
    if (Engine::ImGuiDebugManager::GetInstance()->Begin("LightBeam Debug"))
    {
        ImGui::SeparatorText("Ceiling (RMS)");
        ImGui::Text("Energy: %.3f", energyA);
        ImGui::DragFloat("Gain##ceiling", &beamEnergyGain_, 0.1f, 0.0f, 50.0f);
        ImGui::DragFloat("Floor##ceiling", &beamEnergyFloor_, 0.01f, 0.0f, 1.0f);
        ImGui::SeparatorText("Back (High band)");
        ImGui::Text("Energy: %.3f", energyC);
        ImGui::DragFloat("Gain##back", &backBeamGain_, 0.1f, 0.0f, 50.0f);
        ImGui::DragFloat("Floor##back", &backBeamFloor_, 0.01f, 0.0f, 1.0f);
        ImGui::End();
    }
#endif
}

void GameBackground::Draw(const Camera* camera)
{
    for (auto& obj : backgroundObjects_)
    {
        if (obj)
        {
            obj->Draw(camera);
        }
    }
    for (auto& spectrumBar : spectrumBars_)
    {
        spectrumBar->Draw(camera, emissivePso_.Get());
    }

    spectrumFloor_->Draw(camera);
    //if (enableEmissive_)
    //    overFloor_->DrawWithPSO(emissivePso_.Get(), camera);
    //else
    //    overFloor_->Draw(camera);

    overlayFloor_->DrawWithPSO(emissivePso_.Get(), camera, Vector4(0.3f, 0.765f, 1.0f, 0.25f));

    // 手前の観客ペンライト群
    if (penlightCrowd_)
        penlightCrowd_->Draw(camera);

    // 可視光線（加算ブレンド）を最後に描画
    for (auto& beam : ceilingBeams_)
        beam->Draw(camera, beamPso_.Get());
    for (auto& beam : backBeams_)
        beam->Draw(camera, beamPso_.Get());
}

void GameBackground::SetBPM(float bpm)
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
    GameTime::GetChannel("GameBackground").SetGameSpeed(timeScale_ * 1.3f);

    // 天井ビームのスイング速度をBPMに同期
    for (auto& beam : ceilingBeams_)
        beam->SetBPM(bpm);
}

void GameBackground::SetCombo(int32_t combo)
{
    // ComboThresholds が設定済みならそれを使う。未設定はフォールバック（/50）
    if (comboThresholds_)
    {
        // GetComboProgress はレベル内の進行度（境界でリセットする）なので
        // (level + progress) / maxLevel で全体通しの比率(0..1)を計算する
        float level    = static_cast<float>(comboThresholds_->GetComboLevel(combo));
        float progress = comboThresholds_->GetComboProgress(combo);
        float maxLevel = static_cast<float>(comboThresholds_->GetMaxComboLevel());
        comboColorRatio_ = std::clamp((level + progress) / maxLevel, 0.0f, 1.0f);
    }
    else
        comboColorRatio_ = std::clamp(static_cast<float>(combo) / 50.0f, 0.0f, 1.0f);

    if (penlightCrowd_)
        penlightCrowd_->SetComboRatio(comboColorRatio_);
}

ObjectModel* GameBackground::GetSpeaker(uint32_t laneIndex)
{
    auto it = speakerMap_.find(laneIndex);
    if (it != speakerMap_.end())
    {
        return it->second; // レーンインデックスに対応するスピーカーオブジェクトを返す
    }
    return nullptr; // 見つからなかった場合はnullptrを返す
}

void GameBackground::OnEvent(const GameEvent& event)
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

void GameBackground::Serialize(const std::string& filePath)
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
            std::string  number =  name.substr(name.size() - 1, 1);
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
        object->SetTimeChannel("GameBackground"); // アニメーションの時間チャンネルを設定


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
        backgroundObjects_.push_back(std::move(object)); // オブジェクトを追加

    }
}

void GameBackground::BuildSpeakerMap(const std::string& objName, ObjectModel* model, const std::string& filepath)
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

void GameBackground::UpdateSpeakerAnimation(float deltaTime)
{

    // 色変化の計算
//#ifdef _DEBUG
//    ImGui::Begin("Speaker Color Effect Debug");
//    ImGui::ColorEdit3("start", &startColor.x);
//    ImGui::ColorEdit3("end", &endColor.x);
//    ImGui::DragFloat("interval", &animationInterval_, 0.001f);
//    ImGui::Checkbox("Enable Emissive", &enableEmissive_);
//    ImGui::End();
//#endif // _DEBUG
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

        if (timer - deltaTime < 0.0f)
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
}

void GameBackground::InitializeWall(ObjectModel* wallModel)
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

void GameBackground::InitializeOverlayFloor()
{
    const Vector4 floorColor = Vector4(0.4f, 0.6f, 1.0f, 0.5f);
    auto& materials = overlayFloor_->GetMaterials();
    for (auto& material : materials)
    {
        material->SetColor(floorColor);
    }
}

void GameBackground::CreateEmissivePSO()
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

void GameBackground::CreateBeamPSO()
{
    ShaderCompiler::GetInstance()->Register("LightBeamPS", L"LightBeam.PS.hlsl", L"ps_6_0");

    // 加算ブレンド・両面・深度書き込みOFF（光線が他オブジェクトを隠さない）
    auto builder = PSOBuilder::Create();
    beamPso_ =
        builder
        .SetBlendMode(PSOFlags::BlendMode::Add)
        .SetCullMode(PSOFlags::CullMode::None)
        .SetDepthMode(PSOFlags::DepthMode::Comb_mZero_fLessEqual)
        .SetShaders("Model_VS", "LightBeamPS")
        .SetDepthTest(true)
        .SetDepthWrite(false)
        .UseModelInputLayout()
        .SetRootSignature(PSOManager::GetInstance()->GetRootSignature(PSOFlags::Type::Model).value())
        .Build();
}

void GameBackground::InitializeFFTReaction()
{
    // 各スピーカーに周波数帯を割り当てて反応クラスを生成する。
    // 低域→高域を 4 レーンに分配（FrequencyBands を使用）。
    for (auto& [laneIndex, speaker] : speakerMap_)
    {
        if (!speaker)
            continue;

        Vector2 hzRange;
        switch (laneIndex)
        {
        case 0:  hzRange = Vector2(FrequencyBands::kSubBass, FrequencyBands::kBass);    break; // 低域
        case 1:  hzRange = Vector2(FrequencyBands::kBass,    FrequencyBands::kMid);     break; // 低中域
        case 2:  hzRange = Vector2(FrequencyBands::kMid,     FrequencyBands::kHighMid); break; // 中高域
        default: hzRange = Vector2(FrequencyBands::kHighMid, FrequencyBands::kHigh);    break; // 高域
        }

        auto reaction = std::make_unique<SpeakerSpectrumReaction>();
        reaction->Initialize(speaker, hzRange, static_cast<int32_t>(laneIndex));
        speakerReactions_.push_back(std::move(reaction));

        // --- A: 天井トラスの照明。スピーカー上空からステージ(下)へ向ける ---
        Vector3 ceilOrigin = speaker->translate_ + Vector3(0.0f, 18.0f, 0.0f);
        Vector3 ceilTarget = speaker->translate_; // 真下のスピーカー/レーンへ
        Vector4 ceilColor = Vector4(0.5f, 0.75f, 1.0f, 1.0f);
        auto ceil = std::make_unique<SpotLightBeam>();
        ceil->Initialize("ceil" + std::to_string(laneIndex), ceilOrigin, ceilTarget, ceilColor);
        ceil->SetBaseline(0.25f);          // 常時点灯（消えない下限）
        // 位相をレーンごとにずらして全灯同時に動かない自然なバラつきに
        float phaseOffset = static_cast<float>(laneIndex) * (3.14159265f / 2.0f); // 90度ずつずらす
        ceil->SetSwing(18.0f, 1.5f, phaseOffset); // BPM設定後に速度は上書きされる
        ceil->SetThickness(1.2f);          // energyで太さが最大2.2倍まで脈動
        ceil->SetDynamicColor(true);       // 色相=時間＋帯域、彩度明度=energy
        // レーンごとに開始色相をずらして虹のグラデにする
        ceil->SetHueCycle(20.0f, static_cast<float>(laneIndex) * 90.0f);
        ceilingBeams_.push_back(std::move(ceil));
    }

    // --- C: ステージ奥の逆光。奥(z大)からカメラ方向(手前-z)へ斜め上に立てる ---
    // 高域に反応して点滅。常時は暗め。左右2基。
    const float backZ = 42.0f;
    const float backY = -5.0f;
    for (int i = 0; i < 2; ++i)
    {
        float x = (i == 0) ? -12.0f : 12.0f;
        Vector3 backOrigin = Vector3(x, backY, backZ);
        Vector3 backTarget = backOrigin + Vector3(0.0f, 1.0f, -1.0f); // 斜め上・手前
        Vector4 backColor = Vector4(1.0f, 0.85f, 0.55f, 1.0f);        // 高域らしく暖色寄り
        auto back = std::make_unique<SpotLightBeam>();
        back->Initialize("back" + std::to_string(i), backOrigin, backTarget, backColor);
        back->SetBaseline(0.0f);   // 常時は消灯。高域でのみ点く
        back->SetSwing(0.0f, 0.0f); // スイングなし
        backBeams_.push_back(std::move(back));
    }

    onsetDetector_ = std::make_unique<OnsetDetector>();

    // --- 手前の観客ペンライト群 ---
    // underFloor の手前あたり（z小・低位置）に格子配置
    Vector3 crowdCenter = Vector3(0.0f, -8.3f, 14.0f);
    Vector2 crowdArea = Vector2(25.0f, 3.45f);
    penlightCrowd_ = std::make_unique<PenlightCrowd>();
    penlightCrowd_->Initialize(crowdCenter, crowdArea, 24, 8);
}
