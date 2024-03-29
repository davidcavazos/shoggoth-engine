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


#include "shoggoth-engine/kernel/device.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <SDL/SDL.h>

using namespace std;

const size_t DEFAULT_SCREEN_WIDTH = 640;
const size_t DEFAULT_SCREEN_HEIGHT = 480;
const size_t DEFAULT_SCREEN_DEPTH = 32;

const Uint32 SDL_INIT_FLAGS = SDL_INIT_VIDEO;// | SDL_INIT_JOYSTICK;
const Uint32 SDL_VIDEO_FLAGS = SDL_HWSURFACE | SDL_ANYFORMAT | SDL_OPENGL | SDL_DOUBLEBUF;

Inputs Device::ms_inputs = Inputs();
SDL_Surface* Device::ms_screen = 0;

Device::Device(const string& objectName):
    CommandObject(objectName),
    m_width(DEFAULT_SCREEN_WIDTH),
    m_height(DEFAULT_SCREEN_HEIGHT),
    m_halfWidth(m_width / 2),
    m_halfHeight(m_height / 2),
    m_depth(DEFAULT_SCREEN_DEPTH),
    m_keysPressed(),
    m_mouseButtonsPressed(),
    m_startTime(0.0),
    m_deltaTime(0.0),
    m_fps(0.0)
{
    registerCommand("swap-buffers", boost::bind(&Device::cmdSwapBuffers, this, _1));
    registerAttribute("title", boost::bind(&Device::cmdTitle, this, _1));
    registerAttribute("fullscreen", boost::bind(&Device::cmdFullscreen, this, _1));
    registerAttribute("resolution", boost::bind(&Device::cmdResolution, this, _1));

    cout << "Creating SDL-OpenGL device" << endl;
    if (SDL_Init(SDL_INIT_FLAGS) != 0) // 0 success, -1 failure
        exit(EXIT_FAILURE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);

    //     SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //     SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    const SDL_VideoInfo* info = SDL_GetVideoInfo();
    m_width = info->current_w;
    m_halfWidth = m_width / 2;
    m_height = info->current_h;
    m_halfHeight = m_height / 2;
    m_depth = info->vfmt->BitsPerPixel;

    ms_screen = SDL_SetVideoMode(m_width, m_height, m_depth, SDL_VIDEO_FLAGS);
    if (ms_screen == 0)
        exit(EXIT_FAILURE);

    SDL_ShowCursor(SDL_FALSE);
}

Device::~Device() {
    cout << "SDL-OpenGL device quit" << endl;
    SDL_Quit();

    unregisterAllCommands();
    unregisterAllAttributes();
}

void Device::onFrameStart() {
    m_startTime = SDL_GetTicks() * 0.001;
}

void Device::onFrameEnd() {
    m_deltaTime = SDL_GetTicks() * 0.001 - m_startTime;
    m_fps = 1.0 / m_deltaTime;
}

void Device::swapBuffers() const {
    SDL_GL_SwapBuffers();
}

size_t Device::videoMemKB() {
    const SDL_VideoInfo* info = SDL_GetVideoInfo();
    return info->video_mem;
}

void Device::setTitle(const string& title) {
    SDL_WM_SetCaption(title.c_str(), title.c_str());
}

void Device::setFullscreen(const bool useFullscreen) {
    ms_screen = SDL_GetVideoSurface();
    Uint32 flags = ms_screen->flags;
    Uint32 fullscreenBit = useFullscreen? SDL_FULLSCREEN : 0;
    ms_screen = SDL_SetVideoMode(0, 0, 0, flags | fullscreenBit);
    if (ms_screen == 0)
        ms_screen = SDL_SetVideoMode(0, 0, 0, flags);
    if (ms_screen == 0)
        exit(1);
}

void Device::setResolution(const size_t width, const size_t height) {
    Uint32 flags = SDL_GetVideoSurface()->flags;
    ms_screen = SDL_SetVideoMode(width, height, 0, flags);
    m_width = static_cast<size_t>(ms_screen->w);
    m_halfWidth = m_width / 2;
    m_height = static_cast<size_t>(ms_screen->h);
    m_halfHeight = m_height / 2;
}

size_t Device::getWinWidth() const {
    return ms_screen->w;
}

size_t Device::getWinHeight() const {
    return ms_screen->h;
}

void Device::processEvents(bool& isRunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            ms_inputs.onKeyPress(event.key.keysym.sym);
            m_keysPressed.insert(event.key.keysym.sym);
            break;
        case SDL_KEYUP:
            ms_inputs.onKeyRelease(event.key.keysym.sym);
            m_keysPressed.erase(event.key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONDOWN:
            ms_inputs.onMouseButtonPress(event.button.button);
            m_mouseButtonsPressed.insert(event.button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            ms_inputs.onMouseButtonRelease(event.button.button);
            m_mouseButtonsPressed.erase(event.button.button);
            break;
        case SDL_MOUSEMOTION: {
            mouse_motion_t motion;
            motion.x = event.motion.x;
            motion.y = event.motion.y;
            motion.xrel = event.motion.xrel;
            motion.yrel = event.motion.yrel;
            ms_inputs.onMouseMotion(motion);
            SDL_WarpMouse(Uint16(m_halfWidth), Uint16(m_halfHeight));
            break; }
        default:
            // ignore other events
            break;
        }
    }
    set<size_t>::iterator it;
    for (it = m_keysPressed.begin(); it != m_keysPressed.end(); ++it)
        ms_inputs.onKeyPressed(*it);
    for (it = m_mouseButtonsPressed.begin(); it != m_mouseButtonsPressed.end(); ++it)
        ms_inputs.onMouseButtonPressed(*it);
}



string Device::cmdFullscreen(deque<string>& args) {
    if (args.size() < 1)
        return "Error: too few arguments";
    bool useFullscreen = boost::lexical_cast<bool>(args[0]);
    setFullscreen(useFullscreen);
    return string("Video mode to ") + (useFullscreen? "fullscreen" : "windowed");
}

string Device::cmdResolution(deque<string>& args) {
    if (args.size() < 2)
        return "Error: too few arguments";
    size_t w = boost::lexical_cast<size_t>(args[0]);
    size_t h = boost::lexical_cast<size_t>(args[1]);
    if (w != 0 && h != 0)
        setResolution(w, h);
    return string("Resolution changed to: ") + args[0] + "x" + args[1];
}
