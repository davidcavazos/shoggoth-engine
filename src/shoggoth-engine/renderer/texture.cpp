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


#include "shoggoth-engine/renderer/texture.hpp"

#include <iostream>
#include <SDL/SDL_image.h>
#include "shoggoth-engine/renderer/renderer.hpp"

using namespace std;

Texture::Texture(const string& fileName, Renderer* renderer):
    m_fileName(fileName),
    m_renderer(renderer),
    m_id(0),
    m_bytesPerPixel(0),
    m_width(0),
    m_height(0),
    m_textureFormat(TEXTURE_FORMAT_RGBA),
    m_pixels(0)
{}

void Texture::loadToGPU() {
    SDL_Surface* img = IMG_Load(m_fileName.c_str());
    if (img == 0) {
        cerr << "Error opening image file: " << m_fileName << endl;
        return;
    }

    // set attributes
    m_bytesPerPixel = img->format->BytesPerPixel;
    m_width = img->w;
    m_height = img->h;
    m_pixels = img->pixels;

    // check width and height
    if ((m_width & (m_width - 1)) != 0)
        cerr << "Warning: image's width is not a power of 2: " << m_fileName << endl;
    if ((m_height & (m_height - 1)) != 0)
        cerr << "Warning: image's height is not a power of 2: " << m_fileName << endl;

    // check for number of channels in each pixel
    switch (m_bytesPerPixel) {
    case 4: // has alpha channel
        if (img->format->Rmask == 0x000000ff)
            m_textureFormat = TEXTURE_FORMAT_RGBA;
        else
            m_textureFormat = TEXTURE_FORMAT_BGRA;
        break;
    case 3: // no alpha channel
        if (img->format->Rmask == 0x000000ff)
            m_textureFormat = TEXTURE_FORMAT_RGB;
        else
            m_textureFormat = TEXTURE_FORMAT_BGR;
        break;
    default:
        cerr << "Warning: image is not truecolor: " << m_fileName << endl;
    }

    m_renderer->uploadTextureToGPU(*this);
    SDL_FreeSurface(img);
}



Texture::Texture(const Texture& rhs):
    m_fileName(rhs.m_fileName),
    m_renderer(rhs.m_renderer),
    m_id(rhs.m_id),
    m_bytesPerPixel(rhs.m_bytesPerPixel),
    m_width(rhs.m_width),
    m_height(rhs.m_height),
    m_textureFormat(rhs.m_textureFormat),
    m_pixels(rhs.m_pixels)
{
    cerr << "Error: Texture copy constructor should not be called!" << endl;
}

Texture& Texture::operator=(const Texture&) {
    cerr << "Error: Texture assignment operator should not be called!" << endl;
    return *this;
}
