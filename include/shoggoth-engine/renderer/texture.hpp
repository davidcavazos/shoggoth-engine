/*
 *    Copyright (c) 2012 David Cavazos <davido262@gmail.com>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>

class Renderer;

typedef enum {
    TEXTURE_FORMAT_RGBA,
    TEXTURE_FORMAT_BGRA,
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_BGR
} texture_format_t;

class Texture {
public:
    Texture(const std::string& fileName, Renderer* renderer);

    const std::string& getFileName() const;
    unsigned int getId() const;
    size_t getBytesPerPixel() const;
    size_t getWidth() const;
    size_t getHeight() const;
    const texture_format_t& getTextureFormat() const;
    void* getPixels() const;
    void setId(const unsigned int id);

    void loadToGPU();

private:
    std::string m_fileName;
    Renderer* m_renderer;
    unsigned int m_id;
    size_t m_bytesPerPixel;
    size_t m_width;
    size_t m_height;
    texture_format_t m_textureFormat;
    void* m_pixels;

    Texture(const Texture& rhs);
    Texture& operator=(const Texture&);
};



inline const std::string& Texture::getFileName() const {
    return m_fileName;
}

inline unsigned int Texture::getId() const {
    return m_id;
}

inline size_t Texture::getBytesPerPixel() const {
    return m_bytesPerPixel;
}

inline size_t Texture::getWidth() const {
    return m_width;
}

inline size_t Texture::getHeight() const {
    return m_height;
}

inline const texture_format_t& Texture::getTextureFormat() const {
    return m_textureFormat;
}

inline void* Texture::getPixels() const {
    return m_pixels;
}

inline void Texture::setId(const unsigned int id) {
    m_id = id;
}

#endif // TEXTURE_HPP
