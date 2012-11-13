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


#include "shoggoth-engine/kernel/inputs.hpp"

#include <iostream>
#include <sstream>
#include "shoggoth-engine/kernel/terminal.hpp"

using namespace std;

Inputs::Inputs():
    m_keyPressMap(),
    m_keyReleaseMap(),
    m_keyPressedMap(),
    m_mouseButtonPressMap(),
    m_mouseButtonReleaseMap(),
    m_mouseButtonPressedMap(),
    m_mouseMotionList(),
    m_lastMouseMotion()
{}

void Inputs::bindInput(const input_t type, const string& command, const size_t code) {
    pair<size_t, string> binding(code, command);
    switch (type) {
    case INPUT_KEY_PRESS:
        m_keyPressMap.insert(binding);
        break;
    case INPUT_KEY_RELEASE:
        m_keyReleaseMap.insert(binding);
        break;
    case INPUT_KEY_PRESSED:
        m_keyPressedMap.insert(binding);
        break;
    case INPUT_MOUSE_BUTTON_PRESS:
        m_mouseButtonPressMap.insert(binding);
        break;
    case INPUT_MOUSE_BUTTON_RELEASE:
        m_mouseButtonReleaseMap.insert(binding);
        break;
    case INPUT_MOUSE_BUTTON_PRESSED:
        m_mouseButtonPressedMap.insert(binding);
        break;
    case INPUT_MOUSE_MOTION:
        m_mouseMotionList.push_back(command);
        break;
    default:
        cerr << "Invalid input_t: " << type << endl;
    }
}

void Inputs::clearAllBindings() {
    m_keyPressMap.clear();
    m_keyReleaseMap.clear();
    m_keyPressedMap.clear();
    m_mouseButtonPressMap.clear();
    m_mouseButtonReleaseMap.clear();
    m_mouseButtonPressedMap.clear();
    m_mouseMotionList.clear();
}

void Inputs::onKeyPress(const size_t code) {
    input_map_t::iterator it = m_keyPressMap.find(code);
    if (it != m_keyPressMap.end())
        Terminal::pushCommand(it->second);
}

void Inputs::onKeyRelease(const size_t code) {
    input_map_t::iterator it = m_keyReleaseMap.find(code);
    if (it != m_keyReleaseMap.end())
        Terminal::pushCommand(it->second);
}

void Inputs::onKeyPressed(const size_t code) {
    input_map_t::iterator it = m_keyPressedMap.find(code);
    if (it != m_keyPressedMap.end())
        Terminal::pushCommand(it->second);
}

void Inputs::onMouseButtonPress(const size_t code) {
    input_map_t::iterator it = m_mouseButtonPressMap.find(code);
    if (it != m_mouseButtonPressMap.end())
        Terminal::pushCommand(it->second);
}

void Inputs::onMouseButtonRelease(const size_t code) {
    input_map_t::iterator it = m_mouseButtonReleaseMap.find(code);
    if (it != m_mouseButtonReleaseMap.end())
        Terminal::pushCommand(it->second);
}

void Inputs::onMouseButtonPressed(const size_t code) {
    input_map_t::iterator it = m_mouseButtonPressedMap.find(code);
    if (it != m_mouseButtonPressedMap.end())
        Terminal::pushCommand(it->second);
}

void Inputs::onMouseMotion(const mouse_motion_t& motion) {
    m_lastMouseMotion = motion;
    for (size_t i = 0; i < m_mouseMotionList.size(); ++i)
        Terminal::pushCommand(m_mouseMotionList[i]);
}

ostream& operator<<(ostream& out, const Inputs& rhs) {
    map<size_t, string>::const_iterator it;

    out << "Key Press Map:" << endl;
    for (it = rhs.m_keyPressMap.begin(); it != rhs.m_keyPressMap.end(); ++it)
        out << "\t" << it->first << "\t" << it->second << endl;

    out << "Key Release Map:" << endl;
    for (it = rhs.m_keyReleaseMap.begin(); it != rhs.m_keyReleaseMap.end(); ++it)
        out << "\t" << it->first << "\t" << it->second << endl;

    out << "Key Pressed Map:" << endl;
    for (it = rhs.m_keyPressedMap.begin(); it != rhs.m_keyPressedMap.end(); ++it)
        out << "\t" << it->first << "\t" << it->second << endl;

    out << "Mouse Button Press Map:" << endl;
    for (it = rhs.m_mouseButtonPressMap.begin(); it != rhs.m_mouseButtonPressMap.end(); ++it)
        out << "\t" << it->first << "\t" << it->second << endl;

    out << "Mouse Button Release Map:" << endl;
    for (it = rhs.m_mouseButtonReleaseMap.begin(); it != rhs.m_mouseButtonReleaseMap.end(); ++it)
        out << "\t" << it->first << "\t" << it->second << endl;

    out << "Mouse Button Pressed Map:" << endl;
    for (it = rhs.m_mouseButtonPressedMap.begin(); it != rhs.m_mouseButtonPressedMap.end(); ++it)
        out << "\t" << it->first << "\t" << it->second << endl;

    out << "Mouse Motion Map:" << endl;
    for (size_t i = 0; i < rhs.m_mouseMotionList.size(); ++i)
        out << "\t" << rhs.m_mouseMotionList[i] << endl;

    return out;
}
