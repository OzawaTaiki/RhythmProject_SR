// ============================================
// Text3DRendererを既存のシーンに統合する例
// ============================================

/*
// 1. ヘッダーファイル (.h) に追加
#include <Features/TextRenderer/Text3DRenderer.h>
#include <Features/TextRenderer/AtlasData.h>

private:
    Text3DRenderer* text3DRenderer_ = nullptr;
    AtlasData* fontAtlas_ = nullptr;
*/

/*
// 2. 初期化処理 (Initialize関数) に追加

void GameScene::Initialize(SceneData* sceneData)
{
    // ... 既存の初期化処理 ...

    // Text3DRendererの初期化
    text3DRenderer_ = Text3DRenderer::GetInstance();

    // フォントアトラスの取得（AtlasManagerから既存のフォントを使用）
    fontAtlas_ = AtlasManager::GetInstance()->GetAtlas("GenShinGothic-Bold.ttf_64");
    // または新しいフォントを読み込む場合
    // fontAtlas_ = AtlasManager::GetInstance()->LoadFont("path/to/font.ttf", 64);
}
*/

/*
// 3. 描画処理 (Draw関数) に追加

void GameScene::Draw()
{
    // ... 既存の描画処理 ...

    // Text3DRendererの描画開始
    text3DRenderer_->BeginFrame();

    // 3Dテキストの描画例
    if (fontAtlas_)
    {
        // 基本的な3Dテキスト
        text3DRenderer_->DrawText3D(
            L"SCORE: 12345",          // テキスト
            fontAtlas_,               // フォントアトラス
            &SceneCamera_,            // カメラ
            Vector3{0, 3, 0},         // 位置
            Vector3::zero,            // 回転
            Vector2{1, 1},            // スケール
            Vector4{1, 1, 1, 1}       // 色（白）
        );

        // グラデーション付きテキスト
        text3DRenderer_->DrawText3D(
            L"COMBO x100",
            fontAtlas_,
            &SceneCamera_,
            Vector3{0, 5, 0},
            Vector3::zero,
            Vector2{1.5f, 1.5f},
            Vector4{1, 1, 0, 1},      // 上の色（黄色）
            Vector4{1, 0.5f, 0, 1}    // 下の色（オレンジ）
        );

        // 回転したテキスト
        text3DRenderer_->DrawText3D(
            L"PERFECT!",
            fontAtlas_,
            &SceneCamera_,
            Vector3{0, 0, 5},
            Vector3{0, 0.5f, 0},      // Y軸回転
            Vector2{2, 2},
            Vector4{0, 1, 1, 1}       // シアン
        );
    }

    // Text3DRendererの描画終了
    text3DRenderer_->EndFrame();
}
*/

// ============================================
// 実用的な使用例
// ============================================

/*
// ゲーム内でスコアを3D表示する例

void GameScene::DrawScore()
{
    if (!fontAtlas_ || !text3DRenderer_) return;

    text3DRenderer_->BeginFrame();

    // スコア表示
    std::wstring scoreText = L"SCORE: " + std::to_wstring(currentScore_);
    text3DRenderer_->DrawText3D(
        scoreText,
        fontAtlas_,
        &SceneCamera_,
        Vector3{-8, 6, 0},  // 画面左上寄り
        Vector3::zero,
        Vector2{0.8f, 0.8f},
        Vector4{1, 1, 1, 1}
    );

    // コンボ表示（大きく強調）
    if (currentCombo_ > 0)
    {
        std::wstring comboText = L"COMBO x" + std::to_wstring(currentCombo_);
        text3DRenderer_->DrawText3D(
            comboText,
            fontAtlas_,
            &SceneCamera_,
            Vector3{0, 4, 0},  // 画面中央上部
            Vector3::zero,
            Vector2{1.5f, 1.5f},
            Vector4{1, 0.8f, 0, 1},   // 上：ゴールド
            Vector4{1, 0.5f, 0, 1}    // 下：オレンジ
        );
    }

    // 判定テキスト（PERFECT/GOOD/BAD）を一時的に表示
    if (judgementDisplayTimer_ > 0)
    {
        Vector4 judgementColor = Vector4{1, 1, 1, 1};
        if (lastJudgement_ == "PERFECT") judgementColor = Vector4{0, 1, 1, 1};  // シアン
        else if (lastJudgement_ == "GOOD") judgementColor = Vector4{0, 1, 0, 1}; // 緑
        else if (lastJudgement_ == "BAD") judgementColor = Vector4{1, 1, 0, 1};  // 黄色
        else if (lastJudgement_ == "MISS") judgementColor = Vector4{1, 0, 0, 1}; // 赤

        text3DRenderer_->DrawText3D(
            StringUtils::ToWString(lastJudgement_),
            fontAtlas_,
            &SceneCamera_,
            Vector3{0, 0, 3},
            Vector3::zero,
            Vector2{2, 2},
            judgementColor
        );

        judgementDisplayTimer_ -= deltaTime;
    }

    text3DRenderer_->EndFrame();
}
*/

// ============================================
// 注意点
// ============================================

/*
1. Text3DRendererの初期化
   - Text3DRenderer::GetInstance()->Initialize(device, commandList)
     は、エンジン側で自動的に初期化されているはずです
   - 明示的に初期化する必要はありません

2. フォントアトラスの準備
   - AtlasManagerから既存のフォントアトラスを取得します
   - 使用可能なフォント名は AtlasManager::GetInstance()->GetLoadedFontNames() で確認できます

3. 描画順序
   - BeginFrame() と EndFrame() の間で DrawText3D() を呼び出します
   - EndFrame() で実際のGPU転送と描画が実行されます

4. パフォーマンス
   - 大量のテキストを描画する場合は、フレームごとに再生成されるため注意
   - 静的なテキストは問題ありませんが、動的な更新が多い場合は最適化を検討してください

5. 深度テスト
   - 深度バッファを使用しますが、深度の書き込みは行いません（Comb_mZero_fLessEqual）
   - 3Dオブジェクトの後ろに隠れます
*/
