#pragma once

#include <Application/BeatMapEditor/BeatMapDocument.h>
#include <string>

class BeatMapLoader;

namespace BME
{

/// <summary>
/// 譜面ファイルの入出力を管理するクラス
/// </summary>
class FileManager
{
public:
    FileManager();
    ~FileManager() = default;

    /// <summary>
    /// 譜面ファイルをロード
    /// </summary>
    /// <param name="_filePath">ロードするファイルパス</param>
    /// <param name="_document">データを格納するDocumentクラス</param>
    /// <returns>ロード成功ならtrue</returns>
    bool Load(const std::string& _filePath, Document* _document);

    /// <summary>
    /// 現在のファイルパスに保存
    /// </summary>
    /// <param name="_document">保存するDocumentクラス</param>
    /// <returns>保存成功ならtrue</returns>
    bool Save(Document* _document);

    /// <summary>
    /// 指定されたファイルパスに保存
    /// </summary>
    /// <param name="_filePath">保存先のファイルパス</param>
    /// <param name="_document">保存するDocumentクラス</param>
    /// <returns>保存成功ならtrue</returns>
    bool SaveAs(const std::string& _filePath, Document* _document);

    /// <summary>
    /// 新規譜面を作成
    /// </summary>
    /// <param name="_filePath">ファイルパス</param>
    /// <param name="_audioFilePath">音声ファイルパス</param>
    /// <param name="_document">データを格納するDocumentクラス</param>
    void CreateNew(const std::string& _filePath, const std::string& _audioFilePath, Document* _document);

    /// <summary>
    /// 現在のファイルパスを取得
    /// </summary>
    const std::string& GetCurrentFilePath() const { return currentFilePath_; }

    /// <summary>
    /// ファイルパスが設定されているか確認
    /// </summary>
    bool HasFilePath() const { return !currentFilePath_.empty(); }

    /// <summary>
    /// 最後のエラーメッセージを取得
    /// </summary>
    const std::string& GetLastError() const { return lastError_; }

private:
    /// <summary>
    /// 内部保存処理
    /// </summary>
    bool SaveInternal(const std::string& _filePath, Document* _document);

private:
    BeatMapLoader* beatMapLoader_ = nullptr; // BeatMapLoaderのポインタ
    std::string currentFilePath_;            // 現在のファイルパス
    std::string lastError_;                  // 最後のエラーメッセージ
};

} // namespace BME
