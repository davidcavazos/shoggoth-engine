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


#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <ostream>
#include <istream>
#include <sstream>
#include <string>

class Terminal;

class Command {
public:
    friend class Terminal;
    friend std::ostream& operator<<(std::ostream& out, const Command& rhs);
    friend std::istream& operator>>(std::istream& in, Command& rhs);

    Command();
    Command(const size_t idObject, const size_t idCommand, const std::string& arguments = "");

    size_t getIdObject() const;
    size_t getIdCommand() const;
    const std::string& getArguments() const;
    template <typename T>
    void setArguments(const T& arguments);
    void setArguments(const char* const arguments);
    void setArguments(const std::string& arguments);

    void appendToArguments(const std::string& argsAppended);
    bool parseCommand(const std::string& expression);

private:
    size_t m_idObject;
    size_t m_idCommand;
    std::string m_arguments;

    bool run() const;
};



inline size_t Command::getIdObject() const {
    return m_idObject;
}

inline size_t Command::getIdCommand() const {
    return m_idCommand;
}

inline const std::string& Command::getArguments() const {
    return m_arguments;
}

template <typename T>
void Command::setArguments(const T& arguments) {
    std::ostringstream ss;
    ss << arguments;
    m_arguments = ss.str();
}

inline void Command::setArguments(const char* const arguments) {
    m_arguments = std::string(arguments);
}

inline void Command::setArguments(const std::string& arguments) {
    m_arguments = arguments;
}

#endif // COMMAND_HPP
