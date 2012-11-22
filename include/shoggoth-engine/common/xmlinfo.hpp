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

#ifndef XMLINFO_HPP
#define XMLINFO_HPP

#include <boost/property_tree/ptree.hpp>

const std::string XML_SCENE = "scene";
const std::string XML_DELIMITER = "/";
const std::string XML_COMMENT = "<xmlcomment>";
const std::string XML_ATTRIBUTE = "<xmlattr>";
const std::string XML_ATTR_POSITION = "position";
const std::string XML_ATTR_ORIENTATION = "orientation";
const std::string XML_ATTR_TYPE = "type";
const std::string XML_ATTR_TYPE_ROOT = "root-node";
const std::string XML_ATTR_TYPE_ENTITY = "entity";
const std::string XML_ATTR_TYPE_COMPONENT = "component";



boost::property_tree::ptree::path_type xmlPath(const std::string& path);



inline boost::property_tree::ptree::path_type xmlPath(const std::string& path) {
    return boost::property_tree::ptree::path_type(path, XML_DELIMITER[0]);
}

#endif // XMLINFO_HPP
