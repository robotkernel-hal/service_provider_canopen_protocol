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
#include <exception>


namespace service_provider {

    namespace canopen_protocol {

        class sdo_abort_exception : virtual public std::exception {
            public:
                const uint32_t abort_code;

                /** Constructor (C++ STL string, int, int).
                 *  @param msg The error message
                 */
                explicit sdo_abort_exception(uint32_t abort_code) : 
                    abort_code(abort_code) {}

                /** Destructor.
                 *  Virtual to allow for subclassing.
                 */
                virtual ~sdo_abort_exception() throw () {}

                /** Returns a pointer to the (constant) error description.
                 *  @return A pointer to a const char*. The underlying memory
                 *  is in possession of the Except object. Callers must
                 *  not attempt to free the memory.
                 */
                virtual const char* what() const throw () {
                    return nullptr;
                }
        };

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

        typedef struct emergency_message {
            struct timespec ts;
            uint16_t error_code;
            uint8_t error_register;
            std::vector<uint8_t> data;
        } emergency_message_t;

        //! read or write canopen element values
        typedef std::vector<uint8_t> element_t;

        /** Ethercat data types */
        typedef enum {
            ECT_BOOLEAN         = 0x0001,
            ECT_INTEGER8        = 0x0002,
            ECT_INTEGER16       = 0x0003,
            ECT_INTEGER32       = 0x0004,
            ECT_UNSIGNED8       = 0x0005,
            ECT_UNSIGNED16      = 0x0006,
            ECT_UNSIGNED32      = 0x0007,
            ECT_REAL32          = 0x0008,
            ECT_VISIBLE_STRING  = 0x0009,
            ECT_OCTET_STRING    = 0x000A,
            ECT_UNICODE_STRING  = 0x000B,
            ECT_TIME_OF_DAY     = 0x000C,
            ECT_TIME_DIFFERENCE = 0x000D,
            ECT_DOMAIN          = 0x000F,
            ECT_INTEGER24       = 0x0010,
            ECT_REAL64          = 0x0011,
            ECT_INTEGER64       = 0x0015,
            ECT_UNSIGNED24      = 0x0016,
            ECT_UNSIGNED64      = 0x001B,
            ECT_BYTE            = 0x001E,
            ECT_WORD            = 0x001F,
            ECT_DWORD           = 0x0020,
            ECT_BIT1            = 0x0030,
            ECT_BIT2            = 0x0031,
            ECT_BIT3            = 0x0032,
            ECT_BIT4            = 0x0033,
            ECT_BIT5            = 0x0034,
            ECT_BIT6            = 0x0035,
            ECT_BIT7            = 0x0036,
            ECT_BIT8            = 0x0037
        } ec_data_type;

        static element_t string_to_value(std::string string_val, uint16_t data_type, size_t bit_length) {
            element_t value;

            std::unique_ptr<string_util::py_value> pval(string_util::eval_full(string_val));
            string_util::py_int *pintval;
            string_util::py_long *plongval;
            string_util::py_float *pfloatval;
            string_util::py_special *pspval;
            bool abort;
            size_t bytelen;
            bytelen = (bit_length + 7) / 8;
            value.resize(bytelen);

            pintval = dynamic_cast<string_util::py_int *>(pval.get());
            plongval = dynamic_cast<string_util::py_long *>(pval.get());
            pfloatval = dynamic_cast<string_util::py_float *>(pval.get());
            pspval = dynamic_cast<string_util::py_special *>(pval.get());
            abort = false;

            switch(data_type) {
                case ECT_BOOLEAN:
                    if(!pspval) {
                        abort = true;
                        break;
                    }
                    (*(uint8_t *)&value[0]) = (bool)*pspval;
                    break;
                case ECT_INTEGER8:
                    if(!pintval) {
                        abort = true;
                        break;
                    }
                    (*(int8_t *)&value[0]) = (int)*pintval;
                    break;
                case ECT_INTEGER16:
                    if(!pintval) {
                        abort = true;
                        break;
                    }
                    (*(int16_t *)&value[0]) = (int)*pintval;
                    break;
                case ECT_INTEGER32:
                case ECT_INTEGER24:
                    if(!pintval) {
                        abort = true;
                        break;
                    }
                    (*(int32_t *)&value[0]) = (int)*pintval;
                    break;
                case ECT_INTEGER64: {
                    if(!pintval) {
                        abort = true;
                        break;
                    }
                    if(plongval)
                        (*(int64_t *)&value[0]) = (int64_t)*plongval;
                    else
                        (*(int64_t *)&value[0]) = (int)*pintval;
                    break;
                }
                case ECT_UNSIGNED8:
                    if(!pintval) {
                        abort = true;
                        break;
                    }
                    (*(uint8_t *)&value[0]) = (unsigned int)*pintval;
                    break;
                case ECT_UNSIGNED16:
                    if(!pintval) {
                        abort = true;
                        break;
                    }
                    (*(uint16_t *)&value[0]) = (unsigned int)*pintval;
                    break;
                case ECT_UNSIGNED32:
                case ECT_UNSIGNED24:
                    if(!pintval) {
                        abort = true;
                        break;
                    }
                    (*(uint32_t *)&value[0]) = (unsigned int)*pintval;
                    break;
                case ECT_UNSIGNED64: {
                    if(!pintval) {
                        abort = true;
                        break;
                    }
                    if(plongval)
                        (*(uint64_t *)&value[0]) = (int64_t)*plongval;
                    else
                        (*(uint64_t *)&value[0]) = (unsigned int)*pintval;
                    break;
                }
                case ECT_REAL32:
                    if(pfloatval) {
                        (*(float *)&value[0]) = (float)*pfloatval;
                        break;
                    } else if(pintval) {
                        (*(float *)&value[0]) = (float)*pintval;
                        break;
                    } else if(plongval) {
                        (*(float *)&value[0]) = (float)*plongval;
                        break;
                    }
                    abort = true;
                    break;
                case ECT_REAL64:
                    if(pfloatval) {
                        (*(float *)&value[0]) = (float)*pfloatval;
                        break;
                    } else if(pintval) {
                        (*(float *)&value[0]) = (float)*pintval;
                        break;
                    } else if(plongval) {
                        (*(float *)&value[0]) = (float)*plongval;
                        break;
                    }
                    abort = true;
                    break;
                case ECT_BIT1:
                case ECT_BIT2:
                case ECT_BIT3:
                case ECT_BIT4:
                case ECT_BIT5:
                case ECT_BIT6:
                case ECT_BIT7:
                case ECT_BIT8:
                    abort = true;
                    break;
                case ECT_VISIBLE_STRING: {
                    std::copy(string_val.begin(), string_val.end(), value.begin());
                } break;
                case ECT_OCTET_STRING: {
                    string_util::py_list* plist = dynamic_cast<string_util::py_list*>(pval.get());
                    if(!plist) {
                        break;
                    }
                    int num = 0;
                    uint8_t *tmp = &value[0];
                    for(string_util::py_list_value_t::iterator it = plist->value.begin();
                        it != plist->value.end();
                        ++it) {
                        string_util::py_int *pintval2 = dynamic_cast<string_util::py_int *>(*it);
                        tmp[num++] = (int)*pintval2;
                    }
                    break;
                }
                default:
                    throw std::runtime_error("unknown data type: " + std::to_string(data_type));
            }
            if(abort) {
                throw std::runtime_error("Conversion failed for data type: " + std::to_string(data_type));
            }
            return value;
        }

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

                //! pop next emergency message, throw exception if non present
                /*!
                 * \param msg return emergency message
                 */
                virtual void pop_emergency_message(emergency_message_t& msg) = 0;

        };

        inline base::~base() { }

    }; // namespace canopen_protocol

}; // namespace service_provider
#endif // __SERVICE_PROVIDER__CANOPEN_PROTOCOL__BASE__H__

