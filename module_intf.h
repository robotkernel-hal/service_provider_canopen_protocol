//! robotkernel interface canopen protocol requests
/*!
 * author: Robert Burger
 *
 * $Id$
 */

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

#ifndef __INTERFACE_CANOPEN_PROTOCOL_MODULE_INTF_H__
#define __INTERFACE_CANOPEN_PROTOCOL_MODULE_INTF_H__

#define CANOPEN_MAXNAME 40

#define MOD_REQUEST_CANOPEN_PROTOCOL_MAGIC  0x21
#define MOD_REQUEST_CANOPEN_PROTOCOL(x, s) \
    __MOD_REQUEST((MOD_REQUEST_CANOPEN_PROTOCOL_MAGIC), (x), __MOD_REQUEST_TYPE(s))

//! retrieve canopen index list of object dictionary 
typedef struct canopen_object_dictionary_list {
    int       slave_id;             //! [in]     slave id

    int       indices_cnt;          //! [in/out] number of indices
                                    //           if 0 return indices_cnt without indices
    uint16_t *indices;              //! [out]    indices values
} canopen_object_dictionary_list_t;

#define MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST  \
    MOD_REQUEST_CANOPEN_PROTOCOL(0x0003, canopen_object_dictionary_list_t)

//! read canopen object description
typedef struct canopen_object_description {
    int      slave_id;              //! [in]     slave id
    uint16_t index;                 //! [in]     canopen index

    uint16_t data_type;             //! [out]    datatype
    uint8_t  object_code;           //! [out]    object code
    uint8_t  max_subindices;        //! [out]    maximum number of subindices
    int      name_len;              //! [out]    length of name
    char    *name;                  //! [out]    object name, allocated by callee
                                    //           must be freed by caller
} canopen_object_description_t;

#define MOD_REQUEST_CANOPEN_READ_OBJECT_DESC        \
    MOD_REQUEST_CANOPEN_PROTOCOL(0x0001, canopen_object_description_t)

//! read canopen element description
typedef struct canopen_element_description {
    int      slave_id;              //! [in]     slave id
    uint16_t index;                 //! [in]     index
    uint8_t  sub_index;             //! [in]     sub index

    uint8_t  value_info;            //! [out]    value infos
    uint16_t data_type;             //! [out]    element data type
    uint16_t bit_length;            //! [out]    length in bits
    uint16_t obj_access;            //! [out]    object access
    int      name_len;              //! [out]    length of name
    char    *name;                  //! [out]    element name, allocated by callee
                                    //           must be freed by caller
} canopen_element_description_t;

#define MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC       \
    MOD_REQUEST_CANOPEN_PROTOCOL(0x0002, canopen_element_description_t)

//! read or write canopen element values
typedef struct canopen_element_value {
    int      slave_id;              //! [in]     slave id    
    uint16_t index;                 //! [in]     index
    uint8_t  sub_index;             //! [in]     sub index

    int      value_len;             //! [in/out] length of value
    uint8_t *value;                 //! [in/out] value
} canopen_element_value_t;

#define MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE      \
    MOD_REQUEST_CANOPEN_PROTOCOL(0x0004, canopen_element_value_t)

#define MOD_REQUEST_CANOPEN_WRITE_ELEMENT_VALUE     \
    MOD_REQUEST_CANOPEN_PROTOCOL(0x0005, canopen_element_value_t)

#endif // __INTERFACE_CANOPEN_PROTOCOL_MODULE_INTF_H__

