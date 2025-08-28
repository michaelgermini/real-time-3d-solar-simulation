#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>

enum class TextureType {
    DIFFUSE,
    NORMAL,
    SPECULAR,
    ROUGHNESS,
    METALLIC,
    AMBIENT_OCCLUSION,
    HEIGHT,
    EMISSIVE,
    CUBEMAP,
    SHADOW_MAP
};

enum class TextureFormat {
    RGB,
    RGBA,
    DEPTH,
    DEPTH_STENCIL,
    R,
    RG,
    RGB16F,
    RGBA16F,
    RGB32F,
    RGBA32F
};

enum class TextureFilter {
    NEAREST,
    LINEAR,
    NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR
};

enum class TextureWrap {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER
};

class Texture {
public:
    Texture();
    Texture(const std::string& filePath, TextureType type = TextureType::DIFFUSE);
    Texture(int width, int height, TextureFormat format, TextureType type = TextureType::DIFFUSE);
    ~Texture();

    // Texture loading and creation
    bool LoadFromFile(const std::string& filePath);
    bool LoadFromMemory(const unsigned char* data, int width, int height, int channels);
    void Create(int width, int height, TextureFormat format);
    void CreateCubemap(const std::vector<std::string>& facePaths);
    void CreateShadowMap(int width, int height);

    // Texture parameters
    void SetFilter(TextureFilter minFilter, TextureFilter magFilter);
    void SetWrap(TextureWrap sWrap, TextureWrap tWrap, TextureWrap rWrap = TextureWrap::REPEAT);
    void SetBorderColor(const glm::vec4& color);
    void GenerateMipmaps();
    void SetAnisotropicFiltering(float maxAnisotropy);

    // Binding and rendering
    void Bind(unsigned int slot = 0);
    void Unbind();
    void BindAsRenderTarget();
    void UnbindRenderTarget();

    // Getters
    GLuint GetID() const { return textureID; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    TextureType GetType() const { return type; }
    TextureFormat GetFormat() const { return format; }
    bool IsValid() const { return textureID != 0; }

    // Utility
    void UpdateData(const unsigned char* data);
    void Resize(int newWidth, int newHeight);
    void SaveToFile(const std::string& filePath);
    void GetData(unsigned char* data);

private:
    GLuint textureID;
    int width, height;
    TextureType type;
    TextureFormat format;
    int channels;
    bool hasMipmaps;
    
    void InitializeTexture();
    void SetTextureParameters();
    GLenum GetGLFormat(TextureFormat format);
    GLenum GetGLInternalFormat(TextureFormat format);
    GLenum GetGLDataType(TextureFormat format);
    void DeleteTexture();
};
