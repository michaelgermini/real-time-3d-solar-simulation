#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <functional>

class FileUtils {
public:
    // File reading
    static std::string ReadTextFile(const std::string& filePath);
    static std::vector<unsigned char> ReadBinaryFile(const std::string& filePath);
    static bool FileExists(const std::string& filePath);
    static size_t GetFileSize(const std::string& filePath);
    
    // File writing
    static bool WriteTextFile(const std::string& filePath, const std::string& content);
    static bool WriteBinaryFile(const std::string& filePath, const std::vector<unsigned char>& data);
    static bool CreateDirectory(const std::string& path);
    
    // Path utilities
    static std::string GetDirectory(const std::string& filePath);
    static std::string GetFileName(const std::string& filePath);
    static std::string GetFileExtension(const std::string& filePath);
    static std::string GetFileNameWithoutExtension(const std::string& filePath);
    static std::string CombinePath(const std::string& path1, const std::string& path2);
    static std::string GetAbsolutePath(const std::string& relativePath);
    static std::string GetRelativePath(const std::string& absolutePath, const std::string& basePath);
    
    // Directory operations
    static std::vector<std::string> GetFilesInDirectory(const std::string& directory, const std::string& extension = "");
    static std::vector<std::string> GetSubdirectories(const std::string& directory);
    static bool IsDirectory(const std::string& path);
    static bool DeleteFile(const std::string& filePath);
    static bool DeleteDirectory(const std::string& directory);
    
    // Asset management
    static std::string GetAssetPath(const std::string& assetName);
    static std::string GetShaderPath(const std::string& shaderName);
    static std::string GetTexturePath(const std::string& textureName);
    static std::string GetModelPath(const std::string& modelName);
    
    // File watching
    static void WatchFile(const std::string& filePath, std::function<void()> callback);
    static void UnwatchFile(const std::string& filePath);
    static void UpdateFileWatchers();
    
    // Configuration
    static void SetAssetDirectory(const std::string& directory);
    static void SetShaderDirectory(const std::string& directory);
    static void SetTextureDirectory(const std::string& directory);
    static void SetModelDirectory(const std::string& directory);
    
    // Error handling
    static std::string GetLastError();
    static void ClearLastError();

private:
    static std::string assetDirectory;
    static std::string shaderDirectory;
    static std::string textureDirectory;
    static std::string modelDirectory;
    static std::string lastError;
    
    struct FileWatcher {
        std::string filePath;
        std::function<void()> callback;
        std::time_t lastModified;
    };
    static std::vector<FileWatcher> fileWatchers;
    
    static std::time_t GetFileLastModified(const std::string& filePath);
    static std::string NormalizePath(const std::string& path);
};
