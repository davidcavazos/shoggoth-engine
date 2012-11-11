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


#include "engine/kernel/command.hpp"

#include <sstream>
#include "engine/kernel/terminal.hpp"
#include "engine/kernel/commandobject.hpp"

using namespace std;

const char COMMENT_CHAR = '#';

Command::Command():
    m_idObject(0),
    m_idCommand(0),
    m_arguments(),
    m_empty()
{}

void Command::appendToArguments(const string& newArg) {
    m_arguments.push_back(newArg);
}

bool Command::parseCommand(const string& expression) {
    string object;
    string command;
    string argument;

    m_arguments.resize(0);
    bool isComment = false;
    unsigned short token = 0;
    for (size_t i = 0; i < expression.length() && !isComment; ++i) {
        switch (expression[i]) {
        case ' ': case '\t': case '\n': case '\r': case '\v':
            if ((token == 0 && !object.empty()) ||
                (token == 1 && !command.empty()) ||
                (token >= 2 && !argument.empty())) {
                ++token;
                if (!argument.empty()) {
                    m_arguments.push_back(argument);
                    argument.resize(0);
                }
            }
            continue;
        case COMMENT_CHAR:
            isComment = true;
            if (object.empty())
                return false;
            break;
        default:
            switch(token) {
            case 0: // object
                object.push_back(expression[i]);
                break;
            case 1: // command
                command.push_back(expression[i]);
                break;
            default: // arguments
                argument.push_back(expression[i]);
            }
        }
    }
    if (!argument.empty())
        m_arguments.push_back(argument);

    if (Terminal::ms_objectsTable.findId(m_idObject, object))
        return Terminal::ms_commandsTable.findId(m_idCommand, command);
    return false;
}

bool Command::run() {
    CommandObject* object;
    if (Terminal::getObject(m_idObject, object))
        return object->runObjectCommand(m_idCommand, m_arguments);
    cerr << "ObjectID " << m_idObject << " not found!" << endl;
    return false;
}

ostream& operator<<(ostream& out, const Command& rhs) {
    //     out << rhs.m_idObject << " " << rhs.m_idCommand << " " << rhs.m_arguments;
    out << Terminal::getObjectName(rhs.m_idObject) << " " << Terminal::findCommandName(rhs.m_idCommand);
    for (size_t i = 0; i < rhs.m_arguments.size(); ++i)
        out << " " << rhs.m_arguments[i];
    return out;
}

istream& operator>>(istream& in, Command& rhs) {
    in >> rhs.m_idObject >> rhs.m_idCommand;
    rhs.m_arguments.clear();
    string temp;
    while (in.good()) {
        in >> temp;
        rhs.m_arguments.push_back(temp);
    }
    return in;
}
