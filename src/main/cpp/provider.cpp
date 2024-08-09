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

#include <string.h>

#include "string_util/string_util.h"

#include "provider.h"
#include "service_definitions.h"

#include "robotkernel/kernel.h"
#include "robotkernel/exceptions.h"

SERVICE_PROVIDER_DEF(provider, service_provider::canopen_protocol::provider)

using namespace std;
using namespace std::placeholders;
using namespace robotkernel;
using namespace service_provider;
using namespace canopen_protocol ;
using namespace string_util;

string data_type_to_string(uint16_t dtype) {
    switch(dtype) {
        case ECT_BOOLEAN:
            return string("BOOLEAN");
        case ECT_INTEGER8:
            return string("INTEGER8");
        case ECT_INTEGER16:
            return string("INTEGER16");
        case ECT_INTEGER32:
            return string("INTEGER32");
        case ECT_INTEGER24:
            return string("INTEGER24");
        case ECT_INTEGER64:
            return string("INTEGER64");
        case ECT_UNSIGNED8:
            return string("UNSIGNED8");
        case ECT_UNSIGNED16:
            return string("UNSIGNED16");
        case ECT_UNSIGNED32:
            return string("UNSIGNED32");
        case ECT_UNSIGNED24:
            return string("UNSIGNED24");
        case ECT_UNSIGNED64:
            return string("UNSIGNED64");
        case ECT_REAL32:
            return string("REAL32");
        case ECT_REAL64:
            return string("REAL64");
        case ECT_BYTE:
            return string("BYTE");
        case ECT_WORD:
            return string("WORD");
        case ECT_DWORD:
            return string("DWORD");
        case ECT_BIT1:
            return string("BIT1");
        case ECT_BIT2:
            return string("BIT2");
        case ECT_BIT3:
            return string("BIT3");
        case ECT_BIT4:
            return string("BIT4");
        case ECT_BIT5:
            return string("BIT5");
        case ECT_BIT6:
            return string("BIT6");
        case ECT_BIT7:
            return string("BIT7");
        case ECT_BIT8:
            return string("BIT8");
        case ECT_VISIBLE_STRING:
            return string("VISIBLE_STRING");
        case ECT_OCTET_STRING:
            return string("OCTET_STRING");
        default:
            break;
    }

    return format_string("Type 0x%4.4X", dtype);
}            

typedef const std::map<uint32_t, std::string> abort_code_map_t;
abort_code_map_t abort_code_map = {
    { 0x00000000, "No error" },
    { 0x05030000, "Toggle bit not changed" },
    { 0x05040000, "SDO protocol timeout" },
    { 0x05040001, "Client/Server command specifier not valid or unknown" },
    { 0x05040005, "Out of memory" },
    { 0x06010000, "Unsupported access to an object" },
    { 0x06010001, "Attempt to read to a write only object" },
    { 0x06010002, "Attempt to write to a read only object" },
    { 0x06010003, "Subindex can not be written, SI0 must be 0 for write access" },
    { 0x06010004, "SDO Complete access not supported for variable length objects" },
    { 0x06010005, "Object length exceeds mailbox size" },
    { 0x06010006, "Object mapped to RxPDO, SDO download blocked" },
    { 0x06020000, "The object does not exist in the object directory" },
    { 0x06040041, "The object can not be mapped into the PDO" },
    { 0x06040042, "The number and length of the objects to be mapped would exceed the PDO length" },
    { 0x06040043, "General parameter incompatibility reason" },
    { 0x06040047, "General internal incompatibility in the device" },
    { 0x06060000, "Access failed due to a hardware error" },
    { 0x06070010, "Data type does not match, length of service parameter does not match" },
    { 0x06070012, "Data type does not match, length of service parameter too high" },
    { 0x06070013, "Data type does not match, length of service parameter too low" },
    { 0x06090011, "Subindex does not exist" },
    { 0x06090030, "Value range of parameter exceeded (only for write access)" },
    { 0x06090031, "Value of parameter written too high" },
    { 0x06090032, "Value of parameter written too low" },
    { 0x06090036, "Maximum value is less than minimum value" },
    { 0x08000000, "General error" },
    { 0x08000020, "Data cannot be transferred or stored to the application" },
    { 0x08000021, "Data cannot be transferred or stored to the application because of local control" },
    { 0x08000022, "Data cannot be transferred or stored to the application because of the present device state" },
    { 0x08000023, "Object dictionary dynamic generation fails or no object dictionary is present" },
    { 0xffffffff, "Unknown" } };

string to_unicode(const char* input, int input_len) {
    stringstream ss;
    for(const char* cp = input; cp != input + input_len; cp++) {
        if((strchr(" ,.*:;/_-+=[](){}^!?$", *cp) || isdigit((unsigned char)*cp) || isalpha((unsigned char)*cp)) && (unsigned char)*cp <= 127 && *cp) {
            ss << *cp;
            continue;
        }
        if(*cp == '\r') ss << "\\r";
        else if(*cp == '\n') ss << "\\n";
        else if(*cp == '<') ss << "<";
        else if(*cp == '>') ss << ">";
        else if(*cp == '\t') ss << "\\t";
        else if(*cp == '\\') ss << "\\\\";
        else if(*cp == '\'') ss << "\\'";
        else if(*cp == '"') ss << "\"";
        else if(*cp == '|') ss << "|";
        else if(*cp == '%') ss << "%";
        else { }
    }	
    return ss.str();
}

string value_2_string(uint8_t *usdo, int l, uint16_t dtype, uint16_t index) {
    if (    (   ((index & 0x1600) == 0x1600) ||
                ((index & 0x1A00) == 0x1A00)) &&
            ( dtype == ECT_UNSIGNED32))
        return format_string("0x%08X", *(uint32_t *)usdo);

    if (    (   ( index           == 0x1C12) ||
                ( index           == 0x1C13)) &&
            ( dtype == ECT_UNSIGNED16))
        return format_string("0x%04X", *(uint16_t *)usdo);

    switch (dtype) {
        case ECT_BOOLEAN:
            if (*(uint8_t *)usdo)
                return string("True");
            else
                return string("False");
        case ECT_INTEGER8:
            return format_string("%d", *(int8_t *)usdo); 
        case ECT_INTEGER16:
            return format_string("%d", *(int16_t *)usdo); 
        case ECT_INTEGER32:
        case ECT_INTEGER24:
            return format_string("%d", *(int32_t *)usdo); 
        case ECT_INTEGER64:
            return format_string("%lld", *(int64_t *)usdo); 
        case ECT_UNSIGNED8:
            return format_string("%u", *(uint8_t *)usdo); 
        case ECT_UNSIGNED16:
            return format_string("%u", *(uint16_t *)usdo); 
        case ECT_UNSIGNED32:
        case ECT_UNSIGNED24:
            return format_string("%u", *(uint32_t *)usdo); 
        case ECT_UNSIGNED64:
            return format_string("%llu", *(uint64_t *)usdo); 
        case ECT_REAL32:
            return format_string("%f", *(float *)usdo);
        case ECT_REAL64:
            return format_string("%f", *(double *)usdo);
        case ECT_BYTE:
            return format_string("0x%02X", *(uint8_t *)usdo);
        case ECT_WORD:
            return format_string("0x%04X", *(uint16_t *)usdo);
        case ECT_DWORD:
            return format_string("0x%08X", *(uint32_t *)usdo);
        case ECT_BIT1:
        case ECT_BIT2:
        case ECT_BIT3:
        case ECT_BIT4:
        case ECT_BIT5:
        case ECT_BIT6:
        case ECT_BIT7:
        case ECT_BIT8:
            return format_string("0x%X", *(uint8_t *)usdo);
        case ECT_VISIBLE_STRING:
            return to_unicode((char *)usdo, l);
        default:
        case ECT_OCTET_STRING: {
            string ans = "[ ";

            for (int i = 0; i < l; ++i) {
                ans += format_string("0x%2.2x ", usdo[i]);
                if (i < l)
                    ans += ", ";
            }

            ans += "]";

            return ans;
        }
    }
} 
    
canopen_protocol::handler::handler(const robotkernel::sp_service_interface_t& req) 
    : log_base(req->owner, "canopen_protocol", req->device_name) 
{
    _instance = std::dynamic_pointer_cast<service_provider::canopen_protocol::base>(req);
    if (!_instance)
        throw str_exception("wrong base class");

    add_svc_read_element(req->owner, _instance->device_name + ".read_element");
    add_svc_read_object(req->owner, _instance->device_name + ".read_object");
    add_svc_write_element(req->owner, _instance->device_name + ".write_element");
    add_svc_object_dictionary_list(req->owner, _instance->device_name + ".object_dictionary_list");
    add_svc_pop_emergency_message(req->owner, _instance->device_name + ".pop_emergency_message");
}

canopen_protocol::handler::~handler() {}

//! svc_read_element
/*!
 * \param[in]   req     Service request data.
 * \param[out]  resp    Service response data.
 */
void canopen_protocol::handler::svc_read_element(const struct svc_req_read_element& req, struct svc_resp_read_element& resp) {
    canopen_protocol::element_description_t elem_desc;            
    canopen_protocol::element_t value;
    
    try {
        _instance->get_element_description(req.index, req.sub_index, elem_desc);

        resp.name       = elem_desc.name;
        resp.value_info = elem_desc.value_info;
        resp.data_type  = elem_desc.data_type;
        resp.bit_length = elem_desc.bit_length;
        resp.obj_access = elem_desc.obj_access;
        resp.unit       = elem_desc.unit;

        // decode data
        if (elem_desc.default_value.size() > 0) {
            resp.default_value = value_2_string(&elem_desc.default_value[0], 
                    elem_desc.default_value.size(), elem_desc.data_type, req.index);
        }

        if (elem_desc.min_value.size() > 0) {
            resp.min_value = value_2_string(&elem_desc.min_value[0], 
                    elem_desc.min_value.size(), elem_desc.data_type, req.index);
        }

        if (elem_desc.max_value.size() > 0) {
            resp.max_value = value_2_string(&elem_desc.max_value[0], 
                    elem_desc.max_value.size(), elem_desc.data_type, req.index);
        }
    } catch (std::exception& e) {
        resp.error_message = e.what();
    }

    try {
        _instance->read_element(req.index, req.sub_index, value);

        // decode value
        resp.value = value_2_string(&value[0], value.size(), elem_desc.data_type, req.index);
    } catch (sdo_abort_exception& e) {
        resp.error_message = format_string("got sdo abort : %08X\n", e.abort_code);
    } catch (std::exception& e) {
        resp.error_message += e.what();
    }
}

//! svc_read_object
/*!
 * \param[in]   req     Service request data.
 * \param[out]  resp    Service response data.
 */
void canopen_protocol::handler::svc_read_object(const struct svc_req_read_object& req, struct svc_resp_read_object& resp) {
    canopen_protocol::object_description_t obj_desc;

    try {
        _instance->get_object_description(req.index, obj_desc);
        resp.data_type      = obj_desc.data_type;
        resp.objcode        = obj_desc.object_code;
        resp.max_subindices = obj_desc.max_subindices;
        resp.name           = obj_desc.name;
    } catch (std::exception& e) {
        resp.error_message = e.what();
    }
}

//! svc_write_element
/*!
 * \param[in]   req     Service request data.
 * \param[out]  resp    Service response data.
 */
void canopen_protocol::handler::svc_write_element(const struct svc_req_write_element& req, struct svc_resp_write_element& resp) {
    canopen_protocol::element_description_t elem_desc;            
    canopen_protocol::element_t value;

    try {
        _instance->get_element_description(req.index, req.sub_index, elem_desc);
    } catch (std::exception& e) {
        resp.error_message = e.what();
        return;
    }
    value = string_to_value(req.value,elem_desc.data_type,elem_desc.bit_length) ;
        try {
            _instance->write_element(req.index, req.sub_index, value);
        } catch (std::exception& e) {
            resp.error_message = e.what();
        }
}

//! svc_object_dictionary_list
/*!
 * \param[in]   req     Service request data.
 * \param[out]  resp    Service response data.
 */
void canopen_protocol::handler::svc_object_dictionary_list(const struct svc_req_object_dictionary_list& req, struct svc_resp_object_dictionary_list& resp) {
    try {
        object_dictionary_list_t tmp_list;
        _instance->get_object_dictionary_list(tmp_list);
        resp.indices.assign(tmp_list.begin(), tmp_list.end());
    } catch (std::exception& e) {
        resp.error_message = e.what();
    }
}
        
//! svc_pop_emergency_message
/*!
 * \param[in]   req     Service request data.
 * \param[out]  resp    Service response data.
 */
void canopen_protocol::handler::svc_pop_emergency_message(const struct svc_req_pop_emergency_message& req, struct svc_resp_pop_emergency_message& resp) {
    emergency_message_t msg;
    
    try {
        _instance->pop_emergency_message(msg);

        resp.timestamp_sec  = msg.ts.tv_sec;
        resp.timestamp_nsec = msg.ts.tv_nsec;
        resp.error_code     = msg.error_code;
        resp.error_register = msg.error_register;
        resp.data.assign(msg.data.begin(), msg.data.end());
    } catch (std::exception& e) {
        // no message available
        resp.error_message = "no emergency messages present";
    }
}

