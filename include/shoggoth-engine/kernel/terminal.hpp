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


#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <ostream>
#include <string>
#include <vector>
#include <deque>
#include "shoggoth-engine/kernel/tokentable.hpp"
#include "command.hpp"

class CommandObject;

class Terminal {
public:
    friend class Command;
    friend class CommandObject;

    static bool getObject(const size_t id, CommandObject*& object);
    static const std::string getObjectName(const size_t idObject);
    static std::string findCommandName(const size_t idCommand);

    static void pushCommand(const std::string& cmd);
    static std::string runScript(const std::string& fileName);
    static std::string processCommandsQueue();
    static std::vector<std::string> generateObjectsList(const bool shouldIncludeId = false);
    static std::vector<std::string> generateCommandsList(const bool shouldIncludeId = false);
    static std::vector<std::string> generateAttributesList(const bool shouldIncludeId = false);
    static std::vector<std::string> generateAutocompleteList(const std::string& expression);
    static std::string listsToString();

private:
    typedef std::map<size_t, CommandObject*> obj_ptr_table_t;

    static TokenTable ms_objectsTable;
    static TokenTable ms_commandsTable;
    static TokenTable ms_attributesTable;
    static obj_ptr_table_t ms_objectPointersTable;
    static std::deque<std::string> ms_commandsQueue;

    static size_t registerObject(const std::string& objectName, CommandObject* obj);
    static void unregisterObject(const std::string& objectName);
    static std::vector<std::string> generateAutocompleteObjectList(const std::string& object);
    static std::vector<std::string> generateAutocompleteCommandList(const size_t idObject, const std::string& command);
    static std::vector<std::string> generateAutocompleteAttributeList(const size_t idObject, const std::string& attr);
};



inline bool Terminal::getObject(const size_t id, CommandObject*& object) {
    if (id < ms_objectPointersTable.size()) {
        object = ms_objectPointersTable[id];
        if (object != 0)
            return true;
    }
    return false;
}

inline std::string Terminal::findCommandName(const size_t idCommand) {
    return ms_commandsTable.findName(idCommand);
}

#endif // TERMINAL_HPP
