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


#include "shoggoth-engine/kernel/terminal.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include "shoggoth-engine/kernel/commandobject.hpp"

using namespace std;

TokenTable Terminal::ms_objectsTable = TokenTable();
TokenTable Terminal::ms_commandsTable = TokenTable();
TokenTable Terminal::ms_attributesTable = TokenTable();
Terminal::obj_ptr_table_t Terminal:: ms_objectPointersTable = obj_ptr_table_t();
deque<string> Terminal::ms_commandsQueue = deque<string>();

enum token_state_t {
    TOKEN_OBJECT,
    TOKEN_COMMAND,
    TOKEN_ARGUMENTS
};

bool charIsSpace(const char c) {
    switch (c) {
    case ' ': case '\t': case '\n': case '\r':
        return true;
    }
    return false;
}

void parse(const string& expression, size_t& i, string& token, token_state_t& curState, const token_state_t nextState) {
    if (charIsSpace(expression[i])) {
        token.append(" ");
        curState = nextState;
        while (i < expression.size() && charIsSpace(expression[i]))
            ++i;
        --i;
    }
    else
        token.push_back(expression[i]);
}


const std::string Terminal::getObjectName(const size_t idObject) {
    return ms_objectPointersTable[idObject]->getObjectName();
}

void Terminal::pushCommand(const string& cmd) {
    ms_commandsQueue.push_back(cmd);
}

string Terminal::runScript(const string& fileName) {
    string expression;
    Command cmd;
    deque<Command> commands;
    stringstream output;

    // read script
    fstream file(fileName.c_str(), ios::in);
    while (file.good()) {
        getline(file, expression);
        if (!expression.empty() && cmd.parseCommand(expression)) {
            commands.push_back(cmd);
        }
    }
    file.close();

    // run commands
    for (size_t i = 0; i < commands.size(); ++i) {
        output << "> " << commands[i] << endl;
        if (commands[i].run() && !commands[i].getOutput().empty())
            output << commands[i].getOutput() << endl;
    }
    return output.str();
}

string Terminal::processCommandsQueue() {
    string output;
    Command cmd;
    while (!ms_commandsQueue.empty()) {
        if (cmd.parseCommand(ms_commandsQueue.front()) && cmd.run() && !cmd.getOutput().empty())
            output.append(cmd.getOutput() + "\n");
        ms_commandsQueue.pop_front();
    }
    return output;
}

vector<string> Terminal::generateObjectsList(const bool shouldIncludeId) {
    return ms_objectsTable.generateList(shouldIncludeId);
}

vector<string> Terminal::generateCommandsList(const bool shouldIncludeId) {
    return ms_commandsTable.generateList(shouldIncludeId);
}

vector<string> Terminal::generateAttributesList(const bool shouldIncludeId) {
    return ms_attributesTable.generateList(shouldIncludeId);
}

vector<string> Terminal::generateAutocompleteList(const std::string& expression) {
    vector<string> list;

    string object;
    string command;
    string arguments;

    token_state_t tokenState = TOKEN_OBJECT;
    size_t i = 0;
    while (charIsSpace(expression[i]))
        ++i;
    while (i < expression.size()) {
        switch (tokenState) {
        case TOKEN_OBJECT:
            parse(expression, i, object, tokenState, TOKEN_COMMAND);
            break;
        case TOKEN_COMMAND:
            parse(expression, i, command, tokenState, TOKEN_ARGUMENTS);
            break;
        case TOKEN_ARGUMENTS:
            parse(expression, i, arguments, tokenState, TOKEN_ARGUMENTS);
            break;
        default:
            cerr << "Invalid token state: " << tokenState << endl;
        }
        ++i;
    }

    size_t idObject = 0;
    if (command[command.size()-1] == ' ') {
        ms_objectsTable.findId(idObject, object.substr(0, object.size() - 1));
        list = generateAutocompleteAttributeList(idObject, arguments);
    }
    else if (object[object.size()-1] == ' ') {
        ms_objectsTable.findId(idObject, object.substr(0, object.size() - 1));
        list = generateAutocompleteCommandList(idObject, command);
    }
    else
        list = generateAutocompleteObjectList(object);
    return list;
}



size_t Terminal::registerObject(const std::string& objectName, CommandObject* obj) {
    size_t id = ms_objectsTable.registerToken(objectName);
    ms_objectPointersTable.insert(pair<size_t, CommandObject*>(id, obj));
    return id;
}

void Terminal::unregisterObject(const std::string& objectName) {
    size_t id;
    ms_objectsTable.findId(id, objectName);
    obj_ptr_table_t::iterator it = ms_objectPointersTable.find(id);
    if (it != ms_objectPointersTable.end()) {
        ms_objectsTable.unregisterToken(objectName);
        ms_objectPointersTable.erase(it);
    }
}

vector<string> Terminal::generateAutocompleteObjectList(const string& object) {
    return ms_objectsTable.autocompleteList(object);
}

vector<string> Terminal::generateAutocompleteCommandList(const size_t idObject, const string& command) {
    vector<string> list, completeList;
    size_t idCommand;
    CommandObject* obj = ms_objectPointersTable.find(idObject)->second;
    completeList = ms_commandsTable.autocompleteList(command);
    for (size_t i = 0; i < completeList.size(); ++i) {
        if (ms_commandsTable.findId(idCommand, completeList[i])) {
            if (obj->isCommandFound(idCommand))
                list.push_back(completeList[i]);
        }
    }
    return list;
}

vector< string > Terminal::generateAutocompleteAttributeList(const size_t idObject, const string& attr) {
    vector<string> list, completeList;
    size_t idAttr;
    CommandObject* obj = ms_objectPointersTable.find(idObject)->second;
    completeList = ms_attributesTable.autocompleteList(attr);
    for (size_t i = 0; i < completeList.size(); ++i) {
        if (ms_attributesTable.findId(idAttr, completeList[i])) {
            if (obj->isAttributeFound(idAttr))
                list.push_back(completeList[i]);
        }
    }
    return list;
}

string Terminal::listsToString() {
    stringstream ss;

    ss << "Objects:" << endl;
    obj_ptr_table_t::iterator it;
    for (it = ms_objectPointersTable.begin(); it != ms_objectPointersTable.end(); ++it)
        ss << "    " << *it->second << endl;
    ss << endl;

    ss << "Commands:" << endl;
    vector<string> list = generateCommandsList(true);
    for (size_t i = 0; i < list.size(); ++i)
        ss << "    " << list[i] << endl;
//     ss << endl;
    return ss.str();
}
