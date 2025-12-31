/*
 * @file        forwards.hpp
 * @brief       移植自http://jsoncpp.sourceforge.net/
 * @author      Baptiste Lepilleur <blep@users.sourceforge.net>
 * @date        2012/09/01
 * @copyright   MIT License
*/
#ifndef __JSON_FORWARDS_HPP__
#define __JSON_FORWARDS_HPP__

namespace aps 
{
    namespace base 
    {
        // writer.hpp
        class FastWriter;
        class StyledWriter;

        // reader.hpp
        class Reader;

        // features.hpp
        class Features;

        // value.hpp
        typedef int Int;
        typedef unsigned int UInt;
        class StaticString;
        class Path;
        class PathArgument;
        class Value;
        class ValueIteratorBase;
        class ValueIterator;
        class ValueConstIterator;

    } // namespace 
} // namespace 

#endif // __JSON_FORWARDS_HPP__
