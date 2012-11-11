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


#ifndef COMMANDOBJECT_HPP
#define COMMANDOBJECT_HPP

#include <string>
#include <deque>
#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

class Command;

class CommandObject {

public:
    friend std::ostream& operator<<(std::ostream& out, const CommandObject& rhs);

    typedef boost::function<std::string (std::deque<std::string>&)> slot_t;
    typedef std::map<size_t, slot_t> cmd_table_t;

    CommandObject(const std::string& objectName);
    virtual ~CommandObject();
    bool operator<(const CommandObject& rhs) const;
    bool operator>(const CommandObject& rhs) const;

    size_t getIdObject() const;
    const std::string& getObjectName() const;

    bool isCommandFound(const size_t idCommand) const;
    bool isAttributeFound(const size_t idAttribute) const;

    bool runObjectCommand(const size_t idCommand, std::deque<std::string>& arguments);

    size_t registerCommand(const std::string& cmd, const slot_t& slot);
    size_t registerAttribute(const std::string& attrName, const slot_t& slot);
    void unregisterCommand(const std::string& cmd);
    void unregisterAttribute(const std::string& attrName);
    void unregisterAllCommands();
    void unregisterAllAttributes();

protected:
    std::string m_objectName;
    size_t m_idObject;

private:
    cmd_table_t m_commands;
    cmd_table_t m_attributes;

    std::string cmdSetAttribute(std::deque<std::string>& arg);
};



inline size_t CommandObject::getIdObject() const {
    return m_idObject;
}

inline const std::string& CommandObject::getObjectName() const {
    return m_objectName;
}

inline bool CommandObject::isCommandFound(const size_t idCommand) const {
    cmd_table_t::const_iterator it = m_commands.find(idCommand);
    if (it != m_commands.end())
        return true;
    return false;
}

inline bool CommandObject::isAttributeFound(const size_t idAttribute) const {
    cmd_table_t::const_iterator it = m_attributes.find(idAttribute);
    if (it != m_attributes.end())
        return true;
    return false;
}

#endif // COMMANDOBJECT_HPP
