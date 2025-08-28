#include "Engine/Texture.h"
#include <GL/glew.h>
#include <iostream>

Texture::Texture() : id(0), width(0), height(0), channels(0) {
}

Texture::~Texture() {
    if (id != 0) {
        glDeleteTextures(1, &id);
    }
}

bool Texture::LoadFromFile(const std::string& path) {
    // For now, create a simple 1x1 texture
    // In a full implementation, this would load from file using stb_image
    
    unsigned char data[] = {255, 255, 255, 255}; // White texture
    
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    width = 1;
    height = 1;
    channels = 4;
    
    return true;
}

bool Texture::CreateFromData(unsigned char* data, int w, int h, int c) {
    if (id != 0) {
        glDeleteTextures(1, &id);
    }
    
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    GLenum format = (c == 4) ? GL_RGBA : (c == 3) ? GL_RGB : GL_RED;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    width = w;
    height = h;
    channels = c;
    
    return true;
}

void Texture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

GLuint Texture::GetID() const {
    return id;
}

int Texture::GetWidth() const {
    return width;
}

int Texture::GetHeight() const {
    return height;
}

int Texture::GetChannels() const {
    return channels;
}
