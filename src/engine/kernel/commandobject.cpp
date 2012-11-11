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


#include "engine/kernel/commandobject.hpp"

#include <iostream>
#include <iomanip>
#include "engine/kernel/terminal.hpp"

using namespace std;

const size_t MAX_EXPECTED_ID_DIGITS = 4;

const string SET_COMMAND = "set";

CommandObject::CommandObject(const string& objectName) :
    m_objectName(objectName),
    m_idObject(0),
    m_commands(),
    m_attributes()
{
    m_idObject = Terminal::registerObject(m_objectName, this);
}

CommandObject::~CommandObject() {
    Terminal::unregisterObject(m_objectName);
}

bool CommandObject::operator<(const CommandObject& rhs) const {
    return m_idObject < rhs.m_idObject;
}

bool CommandObject::operator>(const CommandObject& rhs) const {
    return m_idObject > rhs.m_idObject;
}

bool CommandObject::runObjectCommand(const size_t idCommand, deque<string>& arguments) {
    cmd_table_t::iterator it = m_commands.find(idCommand);
    if (it != m_commands.end()) {
        (it->second)(arguments);
        return true;
    }
    cerr << "Object \"" << m_objectName << "\" has no CommandID " << idCommand << endl;
    return false;
}

size_t CommandObject::registerCommand(const string& cmd, const slot_t& slot) {
    size_t id = Terminal::ms_commandsTable.registerToken(cmd);
    cmd_table_t::iterator it = m_commands.find(id);
    if (it == m_commands.end())
        m_commands.insert(pair<size_t, slot_t>(id, slot));
    return id;
}

size_t CommandObject::registerAttribute(const string& attrName, const slot_t& slot) {
    size_t id = Terminal::ms_attributesTable.registerToken(attrName);
    cmd_table_t::iterator it = m_attributes.find(id);
    if (it == m_attributes.end())
        m_attributes.insert(pair<size_t, slot_t>(id, slot));
    registerCommand(SET_COMMAND, boost::bind(&CommandObject::cmdSetAttribute, this, _1));
    return id;
}

void CommandObject::unregisterCommand(const std::string& cmd) {
    size_t id;
    if (Terminal::ms_commandsTable.findId(id, cmd))
        m_commands.erase(id);
}

void CommandObject::unregisterAttribute(const std::string& attrName) {
    size_t id;
    if (Terminal::ms_attributesTable.findId(id, attrName))
        m_attributes.erase(id);
}

void CommandObject::unregisterAllCommands() {
    m_commands.clear();
}

void CommandObject::unregisterAllAttributes() {
    m_attributes.clear();
}

string CommandObject::cmdSetAttribute(deque<string>& args) {
    size_t id;
    if (args.size() == 0)
        return "Error: no attribute specified";
    if (Terminal::ms_attributesTable.findId(id, args[0])) {
        cmd_table_t::iterator it = m_attributes.find(id);
        if (it != m_attributes.end()) {
            args.pop_front();
            return (it->second)(args);
        }
    }
    return "";
}

ostream& operator<<(ostream& out, const CommandObject& rhs) {
    out << setw(MAX_EXPECTED_ID_DIGITS) << rhs.m_idObject << " " << rhs.m_objectName << "   ";
    std::map<size_t, boost::function<std::string (deque<string>&)> >::const_iterator it;
    for (it = rhs.m_commands.begin(); it != rhs.m_commands.end(); ++it)
        out << it->first << " ";
    return out;
}
