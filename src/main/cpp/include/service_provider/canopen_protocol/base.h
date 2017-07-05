//! robotkernel interface canopen protocol
/*!
 * author: Robert Burger
 */

// vim: tabstop=4 softtabstop=4 shiftwidth=4 expandtab:

/*
 * This file is part of robotkernel.
 *
 * robotkernel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * robotkernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with robotkernel.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SERVICE_PROVIDER__CANOPEN_PROTOCOL__BASE__H__
#define __SERVICE_PROVIDER__CANOPEN_PROTOCOL__BASE__H__

#include <list>

#include "robotkernel/service_interface.h"

namespace service_provider {

    namespace canopen_protocol {

        //! retrieve canopen index list of object dictionary 
        typedef std::vector<uint16_t> object_dictionary_list_t;

        //! read canopen object description
        typedef struct object_description {
            uint16_t data_type;       //!< datatype
            uint8_t object_code;      //!< object code
            uint8_t max_subindices;   //!< maximum number of subindices
            std::string name;         //!< object name
        } object_description_t;

        //! read canopen element description
        typedef struct element_description {
            uint8_t value_info;                     //!< value infos
            uint16_t data_type;                     //!< element data type
            uint16_t bit_length;                    //!< length in bits
            uint16_t obj_access;                    //!< object access
            uint16_t unit;                          //!< unit
            std::vector<uint8_t> default_value;     //!< default value
            std::vector<uint8_t> min_value;         //!< min value
            std::vector<uint8_t> max_value;         //!< max value
            std::string name;                       //!< element name
        } element_description_t;

        //! read or write canopen element values
        typedef std::vector<uint8_t> element_t;

        class base : public robotkernel::service_interface {
            public:
                //! construction
                base(std::string owner, std::string service_prefix)
                : robotkernel::service_interface(owner, service_prefix + ".canopen_protocol") {};

                //! destruction
                virtual ~base() = 0;

                //! return a list with all indices of the object dictionary
                /*!
                 * \param list returns the list with all indices
                 */
                virtual void get_object_dictionary_list(object_dictionary_list_t& list) = 0;

                //! return a object description of given index
                /*!
                 * \param index requested index
                 * \param desc returns the object description
                 */
                virtual void get_object_description(const uint16_t& index, 
                        object_description_t& desc) = 0;
                
                //! return a element description of given index and sub index
                /*!
                 * \param index requested index
                 * \param sub_index requested sub index
                 * \param desc returns the object description
                 */
                virtual void get_element_description(const uint16_t& index, const uint8_t& sub_index,
                        element_description_t& desc) = 0;
                
                //! reads one element
                /*!
                 * \param index requested index
                 * \param sub_index requested sub index
                 * \param value returns read value 
                 */
                virtual void read_element(const uint16_t& index, const uint8_t& sub_index,
                        element_t& value) = 0;

                //! writes one element
                /*!
                 * \param index requested index
                 * \param sub_index requested sub index
                 * \param value value to write
                 */
                virtual void write_element(const uint16_t& index, const uint8_t& sub_index,
                        const element_t& value) = 0;

        };

        inline base::~base() { }

    }; // namespace canopen_protocol

}; // namespace service_provider

#endif // __SERVICE_PROVIDER__CANOPEN_PROTOCOL__BASE__H__

