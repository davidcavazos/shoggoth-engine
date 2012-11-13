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


#include "shoggoth-engine/kernel/tokentable.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

const size_t MAX_EXPECTED_ID_DIGITS = 4;

TokenTable::TokenTable() :
    m_tokenMap(),
    m_idMap(),
    m_idGenerator()
{}

size_t TokenTable::registerToken(const string& token) {
    size_t id;
    pair<map<string, size_t>::iterator, bool> inserted;
    map<string, size_t>::iterator it = m_tokenMap.find(token);
    if (it == m_tokenMap.end()) {
        id = m_idGenerator.nextId();
        inserted = m_tokenMap.insert(pair<string, size_t>(token, id));
        m_idMap.insert(pair<size_t, const string*>(id, &inserted.first->first));
    }
    else
        id = it->second;
    return id;
}

void TokenTable::unregisterToken(const string& token) {
    map<string, size_t>::iterator it = m_tokenMap.find(token);
    if (it != m_tokenMap.end()) {
        m_idMap.erase(it->second);
        m_idGenerator.removeId(it->second);
        m_tokenMap.erase(it);
    }
}

bool TokenTable::findId(size_t& id, const string& token) const {
    map<string, size_t>::const_iterator it = m_tokenMap.find(token);
    if (it != m_tokenMap.end()) {
        id = it->second;
        return true;
    }
    cerr << "Token \"" << token << "\" not found!" << endl;
    id = 0;
    return false;
}

string TokenTable::findName(const size_t id) const {
    map<size_t, const string*>::const_iterator it = m_idMap.find(id);
    if (it != m_idMap.end())
        return *it->second;
    cerr << "Command ID \"" << id << "\" not found!" << endl;
    return "";
}

vector<string> TokenTable::generateList(const bool shouldIncludeId) const {
    vector<string> tokenList(m_tokenMap.size());
    size_t i = 0;
    map<string, size_t>::const_iterator it;
    for (it = m_tokenMap.begin(); it != m_tokenMap.end(); ++it) {
        stringstream ss;
        if (shouldIncludeId)
            ss << setw(MAX_EXPECTED_ID_DIGITS) << it->second << " ";
        ss << it->first;
        tokenList[i] = ss.str();
        ++i;
    }
    return tokenList;
}

vector<string> TokenTable::autocompleteList(const std::string& token) const {
    vector<string> autocomplete;
    map<string, size_t>::const_iterator it;
    for (it = m_tokenMap.lower_bound(token); it != m_tokenMap.end(); ++it) {
        if (token.size() > it->first.size())
            break;
        bool isDifferent = false;
        for (size_t i = 0; i < token.size() && !isDifferent; ++i) {
            if (token[i] != it->first[i])
                isDifferent = true;
        }
        if (!isDifferent)
            autocomplete.push_back(it->first);
        else
            break;
    }
    return autocomplete;
}
