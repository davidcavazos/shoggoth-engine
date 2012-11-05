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


#ifndef TOKENTABLE_HPP
#define TOKENTABLE_HPP

#include <string>
#include <vector>
#include <map>
#include "uniqueidgenerator.hpp"

class TokenTable {
public:
    TokenTable();

    size_t registerToken(const std::string& token);
    void unregisterToken(const std::string& token);
    bool findId(size_t& id, const std::string& token) const;
    std::string findName(const size_t id) const;
    std::vector<std::string> generateList(const bool shouldIncludeId = false) const;
    std::vector<std::string> autocompleteList(const std::string& token) const;

private:
    std::map<std::string, size_t> m_tokenMap;
    std::map<size_t, const std::string*> m_idMap;
    UniqueIdGenerator<size_t> m_idGenerator;
};

#endif // TOKENTABLE_HPP
