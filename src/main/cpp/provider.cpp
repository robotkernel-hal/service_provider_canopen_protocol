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

#include "robotkernel/kernel.h"
#include "robotkernel/exceptions.h"

SERVICE_PROVIDER_DEF(provider, service_provider::canopen_protocol::provider)

using namespace std;
using namespace std::placeholders;
using namespace robotkernel;
using namespace service_provider;
using namespace string_util;

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
   ECT_BIT1            = 0x0030,
   ECT_BIT2            = 0x0031,
   ECT_BIT3            = 0x0032,
   ECT_BIT4            = 0x0033,
   ECT_BIT5            = 0x0034,
   ECT_BIT6            = 0x0035,
   ECT_BIT7            = 0x0036,
   ECT_BIT8            = 0x0037
} ec_data_type;

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
            return string((char *)usdo, l);
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
	
canopen_protocol::handler::handler(const robotkernel::sp_service_requester_t& req) 
    : log_base("canopen_protocol", req->owner + "." + req->service_prefix + ".canopen_protocol") {
	robotkernel::kernel& k = *robotkernel::kernel::get_instance();

    _instance = std::dynamic_pointer_cast<service_provider::canopen_protocol::base>(req);
    if (!_instance)
        throw str_exception("wrong base class");

	std::stringstream base;
	base << _instance->owner << "." << _instance->service_prefix << ".canopen_protocol.";

	k.add_service(req->owner, base.str() + "read_element", 
			service_definition_read_element,
			std::bind(&canopen_protocol::handler::service_read_element, this, _1, _2));
	k.add_service(req->owner, base.str() + "read_object", 
			service_definition_read_object,
			std::bind(&canopen_protocol::handler::service_read_object, this, _1, _2));
	k.add_service(req->owner, base.str() + "write_element", 
			service_definition_write_element,
			std::bind(&canopen_protocol::handler::service_write_element, this, _1, _2));
	k.add_service(req->owner, base.str() + "object_dictionary_list", 
			service_definition_object_dictionary_list,
			std::bind(&canopen_protocol::handler::service_object_dictionary_list, 
				this, _1, _2));
}

canopen_protocol::handler::~handler() {
	robotkernel::kernel& k = *robotkernel::kernel::get_instance();

	std::stringstream base;
	base << _instance->owner << "." << _instance->service_prefix << ".canopen_protocol.";

	k.remove_service(base.str() + "read_element");
	k.remove_service(base.str() + "read_object");
	k.remove_service(base.str() + "write_element");
	k.remove_service(base.str() + "object_dictionary_list");
}

//! service callback read element
/*!
 * \param request service request data
 * \parma response service response data
 * \return success
 */
int canopen_protocol::handler::service_read_element(
    const service_arglist_t& request, service_arglist_t& response) {
    // request data
#define READ_ELEMENT_REQ_INDEX      0
#define READ_ELEMENT_REQ_SUB_INDEX  1
    uint16_t index    = request[READ_ELEMENT_REQ_INDEX];
    uint8_t sub_index = request[READ_ELEMENT_REQ_SUB_INDEX];

    std::string default_value = "", min_value = "", max_value = "", 
        resp_value = "", error_message = "";
    canopen_protocol::element_description_t elem_desc;            
    canopen_protocol::element_t value;
    
    try {
        _instance->get_element_description(index, sub_index, elem_desc);

        // decode data
        if (elem_desc.default_value.size() > 0)
            default_value = value_2_string(&elem_desc.default_value[0], 
                    elem_desc.default_value.size(), elem_desc.data_type, index);
        if (elem_desc.min_value.size() > 0)
            min_value = value_2_string(&elem_desc.min_value[0], 
                    elem_desc.min_value.size(), elem_desc.data_type, index);
        if (elem_desc.max_value.size() > 0)
            max_value = value_2_string(&elem_desc.max_value[0], 
                    elem_desc.max_value.size(), elem_desc.data_type, index);

        _instance->read_element(index, sub_index, value);

        // decode value
        resp_value = value_2_string(
                &value[0], value.size(), elem_desc.data_type, index);
    } catch (std::exception& e) {
        error_message = e.what();
    }

    // response data
#define READ_ELEMENT_RESP_NAME          0
#define READ_ELEMENT_RESP_VALUE_INFO    1
#define READ_ELEMENT_RESP_DATA_TYPE     2
#define READ_ELEMENT_RESP_BIT_LENGTH    3
#define READ_ELEMENT_RESP_OBJ_ACCESS    4
#define READ_ELEMENT_RESP_UNIT          5
#define READ_ELEMENT_RESP_DEFAULT_VALUE 6
#define READ_ELEMENT_RESP_MIN_VALUE     7
#define READ_ELEMENT_RESP_MAX_VALUE     8
#define READ_ELEMENT_RESP_VALUE         9
#define READ_ELEMENT_RESP_ERROR_MESSAGE 10
	response.resize(11);
    response[READ_ELEMENT_RESP_NAME]            = elem_desc.name;
    response[READ_ELEMENT_RESP_VALUE_INFO]      = elem_desc.value_info;
    response[READ_ELEMENT_RESP_DATA_TYPE]       = elem_desc.data_type;
    response[READ_ELEMENT_RESP_BIT_LENGTH]      = elem_desc.bit_length;
    response[READ_ELEMENT_RESP_OBJ_ACCESS]      = elem_desc.obj_access;
    response[READ_ELEMENT_RESP_UNIT]            = elem_desc.unit;
    response[READ_ELEMENT_RESP_DEFAULT_VALUE]   = default_value;
    response[READ_ELEMENT_RESP_MIN_VALUE]       = min_value;
    response[READ_ELEMENT_RESP_MAX_VALUE]       = max_value;
    response[READ_ELEMENT_RESP_VALUE]           = resp_value;
    response[READ_ELEMENT_RESP_ERROR_MESSAGE]   = error_message;

    return 0;
}

const std::string canopen_protocol::handler::service_definition_read_element =
    "request:\n"
    "   uint16_t: index\n"
    "   uint8_t: sub_index\n"
    "response:\n"
    "   string: name\n"
    "   uint8_t: value_info\n"
    "   uint16_t: data_type\n"
    "   uint16_t: bit_length\n"
    "   uint16_t: obj_access\n"
    "   uint16_t: unit\n"
    "   string: default_value\n"
    "   string: min_value\n"
    "   string: max_value\n"
    "   string: value\n"
    "   string: error_message\n";
        
//! service callback read object
/*!
 * \param request service request data
 * \parma response service response data
 * \return success
 */
int canopen_protocol::handler::service_read_object(const service_arglist_t& request, 
        service_arglist_t& response) {
    // request data
#define READ_OBJECT_REQ_INDEX   0
    uint16_t index = request[READ_OBJECT_REQ_INDEX];

    std::string error_message = "";
    canopen_protocol::object_description_t obj_desc;

    try {
        _instance->get_object_description(index, obj_desc);
    } catch (std::exception& e) {
        error_message = e.what();
    }

    // response data
#define READ_OBJECT_RESP_DATA_TYPE      0
#define READ_OBJECT_RESP_OBJCODE        1
#define READ_OBJECT_RESP_MAX_SUBINDICES 2
#define READ_OBJECT_RESP_NAME           3
#define READ_OBJECT_RESP_ERROR_MESSAGE  4
    response.resize(5);
    response[READ_OBJECT_RESP_DATA_TYPE]        = obj_desc.data_type;
    response[READ_OBJECT_RESP_OBJCODE]          = obj_desc.object_code;
    response[READ_OBJECT_RESP_MAX_SUBINDICES]   = obj_desc.max_subindices;
    response[READ_OBJECT_RESP_NAME]             = obj_desc.name;
    response[READ_OBJECT_RESP_ERROR_MESSAGE]    = error_message;
    
    return 0;
}

const std::string canopen_protocol::handler::service_definition_read_object =
    "request:\n"
    "   uint16_t: index\n"
    "response:\n"
    "   uint16_t: data_type\n"
    "   uint8_t: objcode\n"
    "   uint8_t: max_subindices\n"
    "   string: name\n"
    "   string: error_message\n";

//! service callback write element
/*!
 * \param request service request data
 * \parma response service response data
 * \return success
 */
int canopen_protocol::handler::service_write_element(const service_arglist_t& request, 
        service_arglist_t& response) {
    canopen_protocol::element_description_t elem_desc;            
    canopen_protocol::element_t value;

    py_value *pval;
    py_int *pintval;
    py_long *plongval;
    py_float *pfloatval;
    py_special *pspval;
    bool abort = false;
    size_t bytelen;

#define WRITE_ELEMENT_REQ_INDEX     0
#define WRITE_ELEMENT_REQ_SUB_INDEX 1
#define WRITE_ELEMENT_REQ_VALUE     2
    uint16_t index      = request[WRITE_ELEMENT_REQ_INDEX];
    uint8_t sub_index   = request[WRITE_ELEMENT_REQ_SUB_INDEX];
    string buf          = request[WRITE_ELEMENT_REQ_VALUE];
    
    // default response values
    string error_message = "";

    try {
        _instance->get_element_description(index, sub_index, elem_desc);
    } catch (std::exception& e) {
        error_message = e.what();
        goto func_exit;
    }
    
    bytelen = (elem_desc.bit_length + 7) / 8;
    value.resize(bytelen);
    
    pval      = eval_full(buf);
    pintval   = dynamic_cast<py_int *>(pval);
    plongval  = dynamic_cast<py_long *>(pval);
    pfloatval = dynamic_cast<py_float *>(pval);
    pspval    = dynamic_cast<py_special *>(pval);

    abort = false;

    switch (elem_desc.data_type) {
        case ECT_BOOLEAN:
            if (!pspval) {
                abort = true;
                break;
            }

            (*(uint8_t *)&value[0]) = (bool)*pspval;
            break;
        case ECT_INTEGER8:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(int8_t *)&value[0]) = (int)*pintval;
            break;
        case ECT_INTEGER16:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(int16_t *)&value[0]) = (int)*pintval;
            break;
        case ECT_INTEGER32:
        case ECT_INTEGER24:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(int32_t *)&value[0]) = (int)*pintval;
            break;
        case ECT_INTEGER64: {
            if (!pintval) {
                abort = true;
                break;
            }

            if (plongval) 
                (*(int64_t *)&value[0]) = (int64_t)*plongval;
            else
                (*(int64_t *)&value[0]) = (int)*pintval;
            break;
        }
        case ECT_UNSIGNED8:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(uint8_t *)&value[0]) = (unsigned int)*pintval;
            break;
        case ECT_UNSIGNED16:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(uint16_t *)&value[0]) = (unsigned int)*pintval;
            break;
        case ECT_UNSIGNED32:
        case ECT_UNSIGNED24:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(uint32_t *)&value[0]) = (unsigned int)*pintval;
            break;
        case ECT_UNSIGNED64: {
            if (!pintval) {
                abort = true;
                break;
            }

            if (plongval) 
                (*(uint64_t *)&value[0]) = (int64_t)*plongval;
            else
                (*(uint64_t *)&value[0]) = (unsigned int)*pintval;
            break;
        }
        case ECT_REAL32:
            if (pfloatval) {
                (*(float *)&value[0]) = (float)*pfloatval;
                break;
            } else if (pintval) {
                (*(float *)&value[0]) = (float)*pintval;
                break;
            } else if (plongval) {
                (*(float *)&value[0]) = (float)*plongval;
                break;
            }

            abort = true;
            break;
        case ECT_REAL64:
            if (pfloatval) {
                (*(float *)&value[0]) = (float)*pfloatval;
                break;
            } else if (pintval) {
                (*(float *)&value[0]) = (float)*pintval;
                break;
            } else if (plongval) {
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
        case ECT_VISIBLE_STRING:
            abort = true;

            error_message = string("not implemented data_type: ") +
                        data_type_to_string(elem_desc.data_type);
            break;
        case ECT_OCTET_STRING: {
            py_list *plist  = dynamic_cast<py_list *>(pval);
            if (!plist) {
                log(warning, "pylist is null on OCTET_STRING\n");
                break;
            }
            int num = 0;
            uint8_t *tmp = &value[0];
            for (py_list_value_t::iterator it = plist->value.begin();
                    it != plist->value.end(); ++it) {
                //py_long *plongval2     = dynamic_cast<py_long *>(*it);
                py_int *pintval2     = dynamic_cast<py_int *>(*it);
                tmp[num++] = (int)*pintval2;
            }
            break;
        }
        default:
            break;
    }
    
    if (pval)
        delete pval;

    if (!abort) {
        try {
            _instance->write_element(index, sub_index, value);
        } catch (std::exception& e) {
            error_message = e.what();
        }
    } 

func_exit:
    
    // response data
#define WRITE_ELEMENT_RESP_ERROR_MESSAGE    0
    response.resize(1);
    response[WRITE_ELEMENT_RESP_ERROR_MESSAGE] = error_message;

    return 0;
}

const std::string canopen_protocol::handler::service_definition_write_element =
    "request:\n"
    "   uint16_t: index\n"
    "   uint8_t: sub_index\n"
    "   string: value\n"
    "response:\n"
    "   string: error_message\n";

//! service callback list object dictionary
/*!
 * \param request service request data
 * \parma response service response data
 * \return success
 */
int canopen_protocol::handler::service_object_dictionary_list(
        const service_arglist_t& request, service_arglist_t& response) {
    object_dictionary_list_t list;

    // default response values
    std::vector<rk_type> indices_resp;
    string error_message = "";

    try {
        _instance->get_object_dictionary_list(list);
        log(info, "service provider got list with %d elements\n", list.size());
        indices_resp.assign(list.begin(), list.end());
        log(info, "service provider returning list with %d elements\n", indices_resp.size());
    } catch (std::exception& e) {
        error_message = e.what();
    }

#define OBJECT_DICTIONARY_LIST_RESP_INDICES     0
#define OBJECT_DICTIONARY_LIST_ERROR_MESSAGE    1
    response.resize(2);
    response[OBJECT_DICTIONARY_LIST_RESP_INDICES]   = indices_resp;
    response[OBJECT_DICTIONARY_LIST_ERROR_MESSAGE]  = error_message;

    return 0;
}
        
const std::string canopen_protocol::handler::service_definition_object_dictionary_list =
    "response:\n"
    "   vector/uint16_t: indices\n"
    "   string: error_message\n";

