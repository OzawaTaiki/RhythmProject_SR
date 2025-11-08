#include "SampleScene.h"

#include <Core/DXCommon/TextureManager/TextureManager.h>
#include <Core/DXCommon/RTV/RTVManager.h>

#include <Features/Scene/Manager/SceneManager.h>
#include <Features/Sprite/Sprite.h>
#include <Features/Model/Manager/ModelManager.h>
#include <Features/Model/Primitive/Triangle.h>
#include <Features/Model/Primitive/Plane.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Debug/Debug.h>


#include <Debug/ImGuiManager.h>
#include <Framework/LayerSystem/LayerSystem.h>

#include <Features/WaveformDisplay/WaveformAnalyzer.h>
#include <Features/AudioSpectrum/SpectrumValidator.h>

#include <System/Time/Stopwatch.h>

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize([[maybe_unused]] SceneData* _sceneData)
{

    // --------------------------------------------------
    // シーン関連 初期化
    SceneCamera_.Initialize();
    SceneCamera_.translate_ = { 0,5,-20 };
    SceneCamera_.rotate_ = { 0.26f,0,0 };
    SceneCamera_.UpdateMatrix();
    debugCamera_.Initialize();


    lineDrawer_ = LineDrawer::GetInstance();
    //lineDrawer_->Initialize();
    lineDrawer_->SetCameraPtr(&SceneCamera_);

    input_ = Input::GetInstance();

    // パーティクルシステムの初期化
    // カメラのポイントを設定する
    ParticleSystem::GetInstance()->SetCamera(&SceneCamera_);

    // ライトの設定
    // ライトグループの初期化 (関数内でDLは初期化される)
    lights_ = std::make_shared<LightGroup>();
    lights_->Initialize();

    // DLを取得して初期化 (任意)
    auto DL = lights_->GetDirectionalLight();
    DL->SetDirection(Vector3(-1.0f, -1.0f, 0.0f).Normalize());

    // ライトの追加(任意)
    // 追加するライトを初期化する
    std::shared_ptr<PointLightComponent> pointLight = std::make_shared<PointLightComponent>();
    pointLight->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    pointLight->SetIntensity(1.0f);
    pointLight->SetRadius(30.0f);
    pointLight->SetPosition({ 0.0f, 5.0f, 0.0f });

    // ライトグループにポイントライトを追加する
    lights_->AddPointLight("pointlight", pointLight);

    // アクティブなライトグループを設定する
    LightingSystem::GetInstance()->SetActiveGroup(lights_);


    // --------------------------------------------------
    // シーン固有の初期化


    // 地面ようのいたポリを生成する
    Plane groundPlane;
    groundPlane.SetSize(Vector2{ 100, 100 });   //サイズは100*100
    groundPlane.SetPivot({ 0,0,0 });        // 基準点は中央
    groundPlane.SetNormal({ 0,1,0 });       //上向き
    groundPlane.SetFlipU(false);            // U軸の反転はなし
    groundPlane.SetFlipV(false);            // V軸の反転はなし

    // 任意の名前で生成する
    groundPlane.Generate("groundPlane");

    Ring ring(1.0f, 3.0f);
    ring.SetDivide(512);

    // 音声データの読み込み
    //soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/Alarm01.wav");
    soundInstance_ = AudioSystem::GetInstance()->Load("Resources/Sounds/Music/Luminous_memory.wav");
    //soundInstance_ = AudioSystem::GetInstance()->Load("C:/Users/ozawa/Desktop/composite_100Hz_1000Hz_10000Hz.wav");


    LayerSystem::CreateLayer("Model", 0);
    LayerSystem::CreateLayer("Main", 1);

    textGenerator_.Initialize(FontConfig());

    //textureGenerator_ = std::make_unique<SpectrumTextureGenerator>();
    //textureGenerator_->Initialize({ 0.0f,0.0f ,0.0f ,0.3f });

    audioSpectrum_= AudioSpectrum(1024);
    const int sampleRate = 44100;

    //= SegmentedAudioGenerator::GenerateSegmentedTones(sampleRate, duration);
    audioSpectrum_.SetAudioData(soundInstance_->GetAudioData());
    audioSpectrum_.SetSampleRate(sampleRate);

    Time::SetDeltaTimeFixed(false);
}

void SampleScene::Update()
{
    // シーン関連更新
    static auto audioData = soundInstance_->GetAudioData();

    static float kminHz = 60.0f;
    static float kmaxHz = 13000.f;
    static int32_t barCount = 48;
    static bool changed = true;
#ifdef _DEBUG


    // デバッグカメラ
    if (Input::GetInstance()->IsKeyTriggered(DIK_F1))
        enableDebugCamera_ = !enableDebugCamera_;

    {
        ImGui::Begin("Engine");
        {
            // サウンドの再生
            if (ImGui::Button("play Sound"))
            {
                if (soundInstance_)
                {
                    // 返り値で VoiceInstanceを受け取る
                    // これを使用して音量やピッチの変更ができる
                    voiceInstance_ = soundInstance_->Play(0.3f);
                }
            }

            static float volume = 1.0f;
            if (ImGui::DragFloat("Volume", &volume, 0.0f, 1.0f))
            {
                if (voiceInstance_)
                {
                    voiceInstance_->SetVolume(volume);
                }
            }
            if (ImGui::Button("Stop Sound"))
            {
                if (voiceInstance_)
                {
                    voiceInstance_->Stop();
                    voiceInstance_ = nullptr; // VoiceInstanceを解放
                }
            }
        }
        changed = false;
        changed |= ImGui::DragFloat("minHz", &kminHz, 1.0f, 20.0f, 20000.0f);
        changed |= ImGui::DragFloat("maxHz", &kmaxHz, 1.0f, 20.0f, 20000.0f);
        changed |= ImGui::DragInt("barCount", &barCount, 1.0f, 1, 128);

        ImGui::End();

        // light調整用
        //lights_->ImGui();

    }




    {
        static std::map<std::string, std::string> wav = {
            {"Alarm","Resources/Sounds/Alarm01.wav"},
            {"Demo","Resources/Sounds/Music/demoMusic.wav"},
            {"Luminous_memory","Resources/Sounds/Music/Luminous_memory.wav"},
            {"composite_100Hz_1000Hz_10000Hz","C:/Users/ozawa/Desktop/composite_100Hz_1000Hz_10000Hz.wav"},

        };

        static int currentIndex = 0;
        if (ImGui::Combo("WAV", &currentIndex, [](void* data, int idx, const char** out_text)
                         {
                             auto& map = *static_cast<std::map<std::string, std::string>*>(data);
                             auto it = map.begin();
                             std::advance(it, idx);
                             *out_text = it->first.c_str();
                             return true;
                         }, static_cast<void*>(&wav), static_cast<int>(wav.size()), 4))
        {
            if (voiceInstance_)
                voiceInstance_->Stop();

            auto it = wav.begin();
            std::advance(it, currentIndex);
            soundInstance_ = AudioSystem::GetInstance()->Load(it->second);
            audioData = soundInstance_->GetAudioData();
        }
    }


#endif // _DEBUG

    if (input_->IsKeyTriggered(DIK_SPACE))
    {
        // シーンの切り替え
        //SceneManager::ReserveScene("GameScene",nullptr);
        voiceInstance_ = soundInstance_->Play(0.3f);
    }

    //{
    //    float curentTime = 0.0f;
    //    if (voiceInstance_ && voiceInstance_->IsPlaying())
    //        curentTime = voiceInstance_->GetElapsedTime();
    //    float rms = WaveformAnalyzer::GetRMSAtTime(soundInstance_.get(), curentTime, 50.0f);

    //    auto spectrum = audioSpectrum_.GetSpectrumAtTime(curentTime);
    //    if (changed)
    //        textureGenerator_->MakeLogRanges(static_cast<int32_t>(spectrum.size()),
    //                                         barCount,
    //                                         kminHz,
    //                                         kmaxHz,
    //                                         soundInstance_->GetSampleRate(), static_cast<int32_t>(spectrum.size() * 2));
    //    textureGenerator_->Generate(spectrum, rms, barCount);
    //    sprite_->SetTextureHandle(textureGenerator_->GetTextureHandle());
    //}

    //SpectrumTest::Test();

    //std::vector<std::complex<float>> test1;
    //std::vector<std::complex<float>> test2;
    //for (int i=0; i < 1024; ++i)
    //{
    //    test1.emplace_back(static_cast<float>(i), 0.0f);
    //    test2.emplace_back(static_cast<float>(i), 0.0f);
    //}

    //std::chrono::steady_clock::time_point start, end;

    //start = std::chrono::steady_clock::now();
    //AudioSpectrum::RecursiveFFT(test1);
    //end = std::chrono::steady_clock::now();

    //Debug::Log(std::format("RecursiveFFT: {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) +"\n");

    //start = std::chrono::steady_clock::now();
    //AudioSpectrum::IterativeFFT(test2);
    //end = std::chrono::steady_clock::now();
    //Debug::Log(std::format("IterativeFFT: {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + "\n");




    //textGenerator_.Draw(L"← → でグレースケールの強度変化", Vector2(200, 300), Vector4(1, 0, 0, 1));
    //textGenerator_.Draw(L"Space でシーンチェンジ", Vector2(200, 500), Vector4(1, 0, 0, 1));
    // --------------------------------
    // シーン共通更新処理

    if (enableDebugCamera_)
    {
        debugCamera_.Update();
        SceneCamera_.matView_ = debugCamera_.matView_;
        SceneCamera_.TransferData();
    }
    else
    {
        SceneCamera_.Update();
        SceneCamera_.UpdateMatrix();
    }

    ParticleSystem::GetInstance()->Update();
    CollisionManager::GetInstance()->Update();
}

void SampleScene::Draw()
{

    // Model描画用のPSO等をセット
    ModelManager::GetInstance()->PreDrawForObjectModel();

    LayerSystem::SetLayer("Model");

    // Sprite用のPSO等をセット
    Sprite::PreDraw();
    // スプライトの描画
    //sprite_->Draw(Vector4(1, 1, 1, 1));
    //textGenerator_.Draw(std::format(L"FPS: {:.2f}", Time::GetFramerate()), Vector2(10, 10), Vector4(1, 0, 0, 1));

}

void SampleScene::DrawShadow()
{
}

#ifdef _DEBUG
void SampleScene::ImGui()
{
}
#endif // _DEBUG
