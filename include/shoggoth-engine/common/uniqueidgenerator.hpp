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


#ifndef UNIQUEIDGENERATOR_HPP
#define UNIQUEIDGENERATOR_HPP

#include <set>
#include <iostream>

template <typename T>
class UniqueIdGenerator {
public:
    UniqueIdGenerator(const T& start = 0, const T& step = 1);

    T& getStart() const;
    T& getStep() const;
    T& getLastId() const;
    void setStart(const T& start);
    void setStep(const T& step);

    T nextId();
    void removeId(const T& id);
    void printAvailableIds() const;

private:
    T m_start;
    T m_step;
    T m_lastId;
    std::set<T> m_availableIds;
};



template <typename T>
UniqueIdGenerator<T>::UniqueIdGenerator(const T& start, const T& step) :
    m_start(start),
    m_step(step),
    m_lastId(start),
    m_availableIds()
{}

template <typename T>
inline T& UniqueIdGenerator<T>::getStart() const {
    return m_start;
}

template <typename T>
inline T& UniqueIdGenerator<T>::getStep() const {
    return m_step;
}

template <typename T>
inline T& UniqueIdGenerator<T>::getLastId() const {
    return m_lastId;
}

template <typename T>
inline void UniqueIdGenerator<T>::setStart(const T& start) {
    m_start = start;
}

template <typename T>
inline void UniqueIdGenerator<T>::setStep(const T& step) {
    m_step = step;
}

template <typename T>
T UniqueIdGenerator<T>::nextId() {
    T id;
    if (m_availableIds.empty()) {
        id = m_lastId;
        m_lastId += m_step;
    }
    else {
        id = *m_availableIds.begin();
        m_availableIds.erase(m_availableIds.begin());
    }
    return id;
}

template <typename T>
void UniqueIdGenerator<T>::removeId(const T& id) {
    m_availableIds.insert(id);
    bool isSimplified = false;
    while (!m_availableIds.empty() && !isSimplified) {
        typename std::set<T>::iterator it = m_availableIds.end();
        --it;
        if (m_lastId - m_step <= *it) {
            m_lastId -= m_step;
            m_availableIds.erase(it);
        }
        else
            isSimplified = true;
    }
}

template <typename T>
void UniqueIdGenerator<T>::printAvailableIds() const {
    std::cout << "Available IDs: ";
    typename std::set<T>::iterator it;
    for (it = m_availableIds.begin(); it != m_availableIds.end(); ++it)
        std::cout << *it << ", ";
    std::cout << m_lastId << "+" << std::endl;
}

#endif // UNIQUEIDGENERATOR_HPP
