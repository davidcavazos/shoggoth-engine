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


#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <ostream>
#include <string>
#include <vector>
#include <map>

typedef enum {
    INPUT_KEY_PRESS,
    INPUT_KEY_RELEASE,
    INPUT_KEY_PRESSED,
    INPUT_MOUSE_BUTTON_PRESS,
    INPUT_MOUSE_BUTTON_RELEASE,
    INPUT_MOUSE_BUTTON_PRESSED,
    INPUT_MOUSE_MOTION
} input_t;

typedef struct {
    int x;
    int y;
    int xrel;
    int yrel;
} mouse_motion_t;

class Inputs {
public:
    Inputs();
    friend std::ostream& operator<<(std::ostream& out, const Inputs& rhs);

    const mouse_motion_t& getLastMouseMotion();

    void bindInput(const input_t type, const std::string& command, const size_t code = 0);
    void clearAllBindings();
    void onKeyPress(const size_t code);
    void onKeyRelease(const size_t code);
    void onKeyPressed(const size_t code);
    void onMouseButtonPress(const size_t code);
    void onMouseButtonRelease(const size_t code);
    void onMouseButtonPressed(const size_t code);
    void onMouseMotion(const mouse_motion_t& motion);

private:
    typedef std::map<size_t, std::string> input_map_t;
    input_map_t m_keyPressMap;
    input_map_t m_keyReleaseMap;
    input_map_t m_keyPressedMap;
    input_map_t m_mouseButtonPressMap;
    input_map_t m_mouseButtonReleaseMap;
    input_map_t m_mouseButtonPressedMap;
    std::vector<std::string> m_mouseMotionList;
    mouse_motion_t m_lastMouseMotion;
};



inline const mouse_motion_t& Inputs::getLastMouseMotion() {
    return m_lastMouseMotion;
}

#endif // INPUTS_HPP
