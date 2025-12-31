/*
 * @file        features.hpp
 * @brief       移植自 http://jsoncpp.sourceforge.net/
 * @author      Baptiste Lepilleur <blep@users.sourceforge.net>
 * @date        2012/09/01
 * @copyright   MIT License
*/
#ifndef __JSON_FEATURES_HPP__
#define __JSON_FEATURES_HPP__

#include <base/config_.hpp>
#include "forwards.hpp"

namespace aps 
{
    namespace base 
    {

        /** \brief Configuration passed to reader and writer.
        * This configuration object can be used to force the Reader or Writer
        * to behave in a standard conforming way.
        */
        class LIBAPS_API Features
        {
        public:
            /** \brief A configuration that allows all features and assumes all strings are UTF-8.
            * - C & C++ comments are allowed
            * - Root object can be any JSON value
            * - Assumes Value strings are encoded in UTF-8
            */
            static Features all();

            /** \brief A configuration that is strictly compatible with the JSON specification.
            * - Comments are forbidden.
            * - Root object must be either an array or an object value.
            * - Assumes Value strings are encoded in UTF-8
            */
            static Features strictMode();

            /** \brief Initialize the configuration like JsonConfig::allFeatures;
            */
            Features();

            /// \c true if comments are allowed. Default: \c true.
            bool allowComments_;

            /// \c true if root must be either an array or an object value. Default: \c false.
            bool strictRoot_;
        };

    } // namespace 
} // namespace 

#endif // __JSON_FEATURES_HPP__
