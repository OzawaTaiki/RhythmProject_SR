#include "SpectrumFloor.h"
#include <Debug/ImGuiDebugManager.h>
#include <Features/WaveformDisplay/WaveformAnalyzer.h>

using namespace Engine;


void SpectrumFloor::Initialize(int32_t row, int32_t col, const Vector2& area)
{
    xCount_ = row;
    zCount_ = col;
    areaSize_ = area;

    jsonBinder_ = std::make_unique<JsonBinder>("SpectrumFloor", "Resources/Data/Game/GameEnvironment/");
    jsonBinder_->RegisterVariable("basePos", &basePos);
    jsonBinder_->RegisterVariable("areaSize", &areaSize_);
    jsonBinder_->RegisterVariable("row", &zCount_);
    jsonBinder_->RegisterVariable("col", &xCount_);


    basePos={ 0,-7.6f,30.8f };

    models_ = std::make_unique<InstancedObjectModel>();
    models_->Initialize("OverFloor/mono_overFloor.obj", xCount_ * zCount_);
    tilePositions_.resize(xCount_);

    for (int32_t x = 0; x < xCount_; ++x)
    {
        auto& columnPositions = tilePositions_[x];
        columnPositions.resize(zCount_);
        for (int32_t y = 0; y < zCount_; ++y)
        {
            columnPositions[y] = Vector3(
                basePos.x + (areaSize_.x / static_cast<float>(xCount_)) * (static_cast<float>(x) + 0.5f) - (areaSize_.x / 2.0f),
                basePos.y,
                basePos.z + (areaSize_.y / static_cast<float>(zCount_)) * (static_cast<float>(y) + 0.5f) - (areaSize_.y / 2.0f));
        }
    }

    columnsData_.resize(xCount_);
}

void SpectrumFloor::Update(float deltaTime, AudioSpectrum* audioSpectrum, SoundInstance* soundInstance, float duration)
{
#ifdef _DEBUG
    if (ImGuiDebugManager::GetInstance()->Begin("SpectrumFloor"))
    {
        ImGui::DragFloat3("Base Position", &basePos.x, 0.1f);
        ImGui::DragFloat3("Area Size", &areaSize_.x, 0.1f);
        ImGui::DragInt("Rows", reinterpret_cast<int*>(&zCount_), 1, 1, 64);
        ImGui::DragInt("Columns", reinterpret_cast<int*>(&xCount_), 1, 1, 64);
        if (ImGui::Button("Apply"))
        {
            tilePositions_.resize(xCount_);
            for (int32_t x = 0; x < xCount_; ++x)
            {
                auto& columnPositions = tilePositions_[x];
                columnPositions.resize(zCount_);
                for (int32_t y = 0; y < zCount_; ++y)
                {
                    columnPositions[y] = Vector3(
                        basePos.x + (areaSize_.x / static_cast<float>(xCount_)) * (static_cast<float>(x) + 0.5f) - (areaSize_.x / 2.0f),
                        basePos.y,
                        basePos.z + (areaSize_.y / static_cast<float>(zCount_)) * (static_cast<float>(y) + 0.5f) - (areaSize_.y / 2.0f));
                }
            }
        }
        if (ImGui::Button("Save"))
        {
            jsonBinder_->Save();
        }

        ImGui::End();
    }

#endif // _DEBUG

    if (!audioSpectrum || !soundInstance)
        return;

    if (beginIndex_ == 0 && endIndex_ == 0)
    {
        audioSpectrum->GetSpectrumIndexRange(minHz_, maxHz_, beginIndex_, endIndex_);
        for (size_t i = 0; i < xCount_; ++i)
        {
            auto& column = columnsData_[i];

            float t0 = static_cast<float>(i) / static_cast<float>(xCount_);
            float t1 = static_cast<float>(i + 1) / static_cast<float>(xCount_);

            // 対数スケールで分割
            float range = static_cast<float>(endIndex_ - beginIndex_);
            float exp0 = std::pow(2.0f, t0 * std::log2(range));
            float exp1 = std::pow(2.0f, t1 * std::log2(range));

            column.beginIndex_ = beginIndex_ + static_cast<size_t>(exp0);
            column.endIndex_ = beginIndex_ + static_cast<size_t>(exp1);

            // 範囲チェック
            if (column.endIndex_ > endIndex_)
            {
                column.endIndex_ = endIndex_;
            }

            column.tiles_.resize(zCount_);
        }
    }

    deltaTime;
    float rms = WaveformAnalyzer::GetRMSAtTime(soundInstance, duration);
    for (size_t i = 0; i < xCount_; ++i)
    {
        auto& columnData = columnsData_[i];
        audioSpectrum->GetAmplitudesInRange(columnData.beginIndex_, columnData.endIndex_, spectrumData_);
        float maxAmplitude = 0.0f;
        if (!spectrumData_.empty())
            maxAmplitude = *std::max_element(spectrumData_.begin(), spectrumData_.end());

        // 振幅をデシベルに変換
        float amplitudeDb = 20.0f * std::log10(maxAmplitude + 0.0001f); // log10(0)回避
        // -80dB ~ 0dB を 0.0 ~ 1.0 にマップ
        float normalizedDb = (amplitudeDb + 60.0f) / 70.0f;
        normalizedDb = std::clamp(normalizedDb, 0.0f, 1.0f);

        normalizedDb = Easing::EaseInOutCubic(normalizedDb + (rms * 0.7f));
        normalizedDb = std::clamp(normalizedDb, 0.0f, 1.0f);

        // 最上段を1.0としたとき
        // maxAmpは0.0~1.0なので
        // タイル数×maxAmpの位置まで点灯させる

        size_t activeHeight = static_cast<size_t>(zCount_ * normalizedDb);

        for (size_t j = 0; j < zCount_; ++j)
        {
            auto& tileData = columnData.tiles_[j];
            if (j < activeHeight)
            {// 点灯
                tileData.isActive_ = true;
                tileData.timer_ = 1.0f;// 1秒でフェードイン
            }
            else
            {
                tileData.isActive_ = false;
                tileData.timer_ -= deltaTime;
                if (tileData.timer_ < 0.0f)
                    tileData.timer_ = 0.0f;
            }
            float t = Easing::EaseOutQuart(tileData.timer_);
            tileData.color_ = Lerp(Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), t);
        }
    }
}

void SpectrumFloor::Draw(const Camera* camera)
{
    models_->Clear();

    for (size_t i = 0; i < xCount_; ++i)
    {
        for (size_t j = 0; j < zCount_; ++j)
        {
            Vector4 color = Vector4(1, 1, 1, 1);
            if (columnsData_.size() > i && columnsData_[i].tiles_.size() > j)
            {
                const auto& tileData = columnsData_[i].tiles_[j];
                color = tileData.color_;
            }
            Matrix4x4 world = MakeAffineMatrix(Vector3::one, Vector3::zero, tilePositions_[i][j]);
            models_->AddInstance(world, color);
        }
    }

    models_->Draw(camera);

}
