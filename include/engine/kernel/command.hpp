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
#include <deque>

class Terminal;

class Command {
public:
    friend class Terminal;
    friend std::ostream& operator<<(std::ostream& out, const Command& rhs);
    friend std::istream& operator>>(std::istream& in, Command& rhs);

    Command();

    size_t getIdObject() const;
    size_t getIdCommand() const;
    const std::deque<std::string>& getArguments() const;
    std::deque<std::string>& arguments();
    const std::string& getArgument(const size_t i) const;
    void setArguments(const std::deque<std::string>& arguments);
    const std::string& getOutput() const;

    void appendToArguments(const std::string& newArg);
    bool parseCommand(const std::string& expression);

private:
    size_t m_idObject;
    size_t m_idCommand;
    std::deque<std::string> m_arguments;
    std::string m_output;
    std::string m_empty;

    bool run();
};



inline size_t Command::getIdObject() const {
    return m_idObject;
}

inline size_t Command::getIdCommand() const {
    return m_idCommand;
}

inline const std::deque<std::string>& Command::getArguments() const {
    return m_arguments;
}

inline std::deque<std::string>& Command::arguments() {
    return m_arguments;
}

inline const std::string& Command::getArgument(const size_t i) const {
    if (i < m_arguments.size())
        return m_arguments[i];
    return m_empty;
}

inline void Command::setArguments(const std::deque<std::string>& arguments) {
    m_arguments = arguments;
}

inline const std::string& Command::getOutput() const {
    return m_output;
}

#endif // COMMAND_HPP
