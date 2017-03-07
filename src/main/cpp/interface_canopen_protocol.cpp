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

#include "interface_canopen_protocol.h"

#include "robotkernel/kernel.h"
#include "robotkernel/exceptions.h"

SERVICE_PROVIDER_DEF(canopen_protocol, interface_canopen_protocol::canopen_protocol)

using namespace std;
using namespace std::placeholders;
using namespace robotkernel;
using namespace interface_canopen_protocol;

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

string value_2_string(uint8_t *usdo, int l, uint16_t dtype) {
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
	
const char* interface_canopen_protocol::canopen_protocol_sp_magic = "canopen_protocol";

//! default construction
/*!
 * \param node configuration node
 */
canopen_protocol::canopen_protocol()
    : service_provider_base("canopen_protocol") {
}
			
//! add slave
/*!
 * \param mod_name slave owning module
 * \param dev_name name of device
 * \param slave_id id in module
 */
void canopen_protocol::add_slave(
		const char *mod_name, const char *dev_name, int slave_id) {
	canopen_protocol_handler *handler = new canopen_protocol_handler(
			string(mod_name), string(dev_name), slave_id);

	handler_map[make_pair(string(mod_name), slave_id)] = handler;
}
	
//! remove registered slave
/*!
 * \param mod_name slave owning module
 * \param slave_id id in module
 */
void canopen_protocol::remove_slave(const char *mod_name, int slave_id) {
	for (std::map<std::pair<std::string, int>, canopen_protocol_handler *>::iterator it =
			handler_map.begin(); it != handler_map.end(); ++it) {
		if (it->first.first != string(mod_name))
			continue; // skip this, not owr module

		if (it->first.second != slave_id) 
			continue; // skip this, not owr slave

		delete it->second;
		it = handler_map.erase(it);
	}
}

//! remove all slaves from module
/*!
 * \param mod_name module owning slaves
 */
void canopen_protocol::remove_module(const char *mod_name) {
	for (std::map<std::pair<std::string, int>, canopen_protocol_handler *>::iterator it =
			handler_map.begin(); it != handler_map.end(); ++it) {
		if (it->first.first != string(mod_name))
			continue; // skip this, not owr module

		delete it->second;
		it = handler_map.erase(it);
	}
}

canopen_protocol_handler::canopen_protocol_handler(std::string mod_name, std::string dev_name, 
		int slave_id) : log_base(mod_name, "canopen_protocol"), mod_name(mod_name), dev_name(dev_name), slave_id(slave_id) {
	robotkernel::kernel& k = *robotkernel::kernel::get_instance();

	std::stringstream base;
	base << mod_name << "." << dev_name 
		<< ".canopen_protocol.";

	k.add_service(mod_name, base.str() + "read_element", 
			service_definition_read_element,
			std::bind(&canopen_protocol_handler::service_read_element, this, _1, _2));
	k.add_service(mod_name, base.str() + "read_object", 
			service_definition_read_object,
			std::bind(&canopen_protocol_handler::service_read_object, this, _1, _2));
	k.add_service(mod_name, base.str() + "write_element", 
			service_definition_write_element,
			std::bind(&canopen_protocol_handler::service_write_element, this, _1, _2));
	k.add_service(mod_name, base.str() + "object_dictionary_list", 
			service_definition_object_dictionary_list,
			std::bind(&canopen_protocol_handler::service_object_dictionary_list, 
				this, _1, _2));
}

canopen_protocol_handler::~canopen_protocol_handler() {
	robotkernel::kernel& k = *robotkernel::kernel::get_instance();

	std::stringstream base;
	base << mod_name << "." << dev_name 
		<< ".canopen_protocol.";

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
int canopen_protocol_handler::service_read_element(
    const service_arglist_t& request, service_arglist_t& response) {
    canopen_element_description desc;
    canopen_element_value value;
    memset(&desc, 0, sizeof(desc));
    memset(&value, 0, sizeof(value));

    // request data
#define READ_ELEMENT_REQ_INDEX      0
#define READ_ELEMENT_REQ_SUB_INDEX  1
    desc.slave_id   = slave_id;
    desc.index      = request[READ_ELEMENT_REQ_INDEX];
    desc.sub_index  = request[READ_ELEMENT_REQ_SUB_INDEX];
    desc.name       = NULL;
    desc.name_len   = 0;

    // default response data
    int64_t state           = 0;
    string name             = "";
    uint8_t value_info      = 0;
    uint16_t data_type      = 0;
    uint16_t bit_length     = 0;
    uint16_t obj_access     = 0;
    string resp_value       = "";
    string error_message    = "";
    
    // execute module request read element description
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC, (void *)&desc);

    if (state != 0) {
        error_message = 
            "MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC failed!";

        goto func_exit;
    }

    if (desc.name && desc.name_len) {
        name = string(desc.name, desc.name_len);
    }

    value_info = desc.value_info;
    data_type  = desc.data_type;
    bit_length = desc.bit_length;
    obj_access = desc.obj_access;

    value.slave_id  = slave_id;
    value.index     = request[READ_ELEMENT_REQ_INDEX];
    value.sub_index = request[READ_ELEMENT_REQ_SUB_INDEX];
    value.value_len = (desc.bit_length + 7) / 8;
    value.value     = new uint8_t[value.value_len]();
    
    // execute module request read element value
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE, (void *)&value);

    if (state == 0) {
        resp_value = value_2_string(
                value.value, value.value_len, desc.data_type);
    } else {
        if (abort_code_map.find(state) != abort_code_map.end()) {
            try {
                error_message = 
                    string("MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE failed: ") + 
                    abort_code_map.at(state);
            } catch (exception& e) {
                error_message = 
                    format_string("MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE failed: "
                            "state 0x%X\n", state); 
            }
        } else {
            error_message = 
                "MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE failed!";
        }
    }

func_exit:
    // response data
#define READ_ELEMENT_RESP_STATE         0
#define READ_ELEMENT_RESP_NAME          1
#define READ_ELEMENT_RESP_VALUE_INFO    2
#define READ_ELEMENT_RESP_DATA_TYPE     3
#define READ_ELEMENT_RESP_BIT_LENGTH    4
#define READ_ELEMENT_RESP_OBJ_ACCESS    5
#define READ_ELEMENT_RESP_VALUE         6
#define READ_ELEMENT_RESP_ERROR_MESSAGE 7
    response[READ_ELEMENT_RESP_STATE]           = state;
    response[READ_ELEMENT_RESP_NAME]            = name;
    response[READ_ELEMENT_RESP_VALUE_INFO]      = value_info;
    response[READ_ELEMENT_RESP_DATA_TYPE]       = data_type;
    response[READ_ELEMENT_RESP_BIT_LENGTH]      = bit_length;
    response[READ_ELEMENT_RESP_OBJ_ACCESS]      = obj_access;
    response[READ_ELEMENT_RESP_VALUE]           = resp_value;
    response[READ_ELEMENT_RESP_ERROR_MESSAGE]   = error_message;

    return 0;
}

const std::string canopen_protocol_handler::service_definition_read_element =
    "request:\n"
    "   uint16_t: index\n"
    "   uint8_t: sub_index\n"
    "response:\n"
    "   int64_t: state\n"
    "   string: name\n"
    "   uint8_t: value_info\n"
    "   uint16_t: data_type\n"
    "   uint16_t: bit_length\n"
    "   uint16_t: obj_access\n"
    "   string: value\n"
    "   string: error_message\n";
        
//! service callback read object
/*!
 * \param request service request data
 * \parma response service response data
 * \return success
 */
int canopen_protocol_handler::service_read_object(const service_arglist_t& request, 
        service_arglist_t& response) {
    canopen_object_description desc;
    memset(&desc, 0, sizeof(desc));
    desc.slave_id = slave_id;
    // request data
#define READ_OBJECT_REQ_INDEX   0
    desc.index = request[READ_OBJECT_REQ_INDEX];

    // default response data
    int64_t state = 0;
    uint16_t data_type = 0;
    uint8_t objcode = 0;
    uint8_t max_subindices = 0;
    string name = "";
    string error_message = "";
    
    // execute request
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_OBJECT_DESC, (void *)&desc);

    if (state == 0) {
        data_type       = desc.data_type;
        objcode         = desc.object_code;
        max_subindices  = desc.max_subindices;
        
        if (desc.name && desc.name_len) {
            name = string(desc.name, desc.name_len);
        }
    } else {
        error_message = "MOD_REQUEST_CANOPEN_READ_OBJECT_DESC failed!";
    }

    // response data
#define READ_OBJECT_RESP_STATE          0
#define READ_OBJECT_RESP_DATA_TYPE      1
#define READ_OBJECT_RESP_OBJCODE        2
#define READ_OBJECT_RESP_MAX_SUBINDICES 3
#define READ_OBJECT_RESP_NAME           4
#define READ_OBJECT_RESP_ERROR_MESSAGE  5
    response.resize(6);
    response[READ_OBJECT_RESP_STATE]            = state;
    response[READ_OBJECT_RESP_DATA_TYPE]        = data_type;
    response[READ_OBJECT_RESP_OBJCODE]          = objcode;
    response[READ_OBJECT_RESP_MAX_SUBINDICES]   = max_subindices;
    response[READ_OBJECT_RESP_NAME]             = name;
    response[READ_OBJECT_RESP_ERROR_MESSAGE]    = error_message;
    
    return 0;
}

const std::string canopen_protocol_handler::service_definition_read_object =
    "request:\n"
    "   uint16_t: index\n"
    "response:\n"
    "   int64_t: state\n"
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
int canopen_protocol_handler::service_write_element(const service_arglist_t& request, 
        service_arglist_t& response) {
    canopen_element_value value;
    canopen_element_description desc;
    memset(&desc, 0, sizeof(desc));
    desc.slave_id   = slave_id;
#define WRITE_ELEMENT_REQ_INDEX     0
#define WRITE_ELEMENT_REQ_SUB_INDEX 1
#define WRITE_ELEMENT_REQ_VALUE     2
    desc.index      = request[WRITE_ELEMENT_REQ_INDEX];
    desc.sub_index  = request[WRITE_ELEMENT_REQ_SUB_INDEX];
    string buf      = request[WRITE_ELEMENT_REQ_VALUE];
    py_value *pval;
    py_int *pintval;
    py_long *plongval;
    py_float *pfloatval;
    py_special *pspval;
    bool abort = false;

    // default response values
    int32_t state = 0;
    string error_message = "";
    
    // execute procedure command    
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC, (void *)&desc);

    if (state != 0) {
        error_message = 
            "MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC failed!";

        goto func_exit;
    } 

    value.slave_id      = slave_id;
    value.index         = request[WRITE_ELEMENT_REQ_INDEX];
    value.sub_index     = request[WRITE_ELEMENT_REQ_SUB_INDEX];
    value.value_len     = (desc.bit_length + 7) / 8;
    value.value         = new uint8_t[value.value_len]();
    
    pval      = eval_full(buf);
    pintval   = dynamic_cast<py_int *>(pval);
    plongval  = dynamic_cast<py_long *>(pval);
    pfloatval = dynamic_cast<py_float *>(pval);
    pspval    = dynamic_cast<py_special *>(pval);

    abort = false;

    switch (desc.data_type) {
        case ECT_BOOLEAN:
            if (!pspval) {
                abort = true;
                break;
            }

            (*(uint8_t *)value.value) = (bool)*pspval;
            break;
        case ECT_INTEGER8:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(int8_t *)value.value) = (int)*pintval;
            break;
        case ECT_INTEGER16:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(int16_t *)value.value) = (int)*pintval;
            break;
        case ECT_INTEGER32:
        case ECT_INTEGER24:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(int32_t *)value.value) = (int)*pintval;
            break;
        case ECT_INTEGER64: {
            if (!pintval) {
                abort = true;
                break;
            }

            if (plongval) 
                (*(int64_t *)value.value) = (int64_t)*plongval;
            else
                (*(int64_t *)value.value) = (int)*pintval;
            break;
        }
        case ECT_UNSIGNED8:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(uint8_t *)value.value) = (unsigned int)*pintval;
            break;
        case ECT_UNSIGNED16:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(uint16_t *)value.value) = (unsigned int)*pintval;
            break;
        case ECT_UNSIGNED32:
        case ECT_UNSIGNED24:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(uint32_t *)value.value) = (unsigned int)*pintval;
            break;
        case ECT_UNSIGNED64: {
            if (!pintval) {
                abort = true;
                break;
            }

            if (plongval) 
                (*(uint64_t *)value.value) = (int64_t)*plongval;
            else
                (*(uint64_t *)value.value) = (unsigned int)*pintval;
            break;
        }
        case ECT_REAL32:
            if (pfloatval) {
                (*(float *)value.value) = (float)*pfloatval;
                break;
            } else if (pintval) {
                (*(float *)value.value) = (float)*pintval;
                break;
            } else if (plongval) {
                (*(float *)value.value) = (float)*plongval;
                break;
            }

            abort = true;
            break;
        case ECT_REAL64:
            if (pfloatval) {
                (*(float *)value.value) = (float)*pfloatval;
                break;
            } else if (pintval) {
                (*(float *)value.value) = (float)*pintval;
                break;
            } else if (plongval) {
                (*(float *)value.value) = (float)*plongval;
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

            state = -1;
            error_message = string("not implemented data_type: ") +
                        data_type_to_string(desc.data_type);
            break;
        case ECT_OCTET_STRING: {
            py_list *plist  = dynamic_cast<py_list *>(pval);
            if (!plist) {
                log(warning, "pylist is null on OCTET_STRING\n");
                break;
            }
            int num = 0;
            for (py_list_value_t::iterator it = plist->value.begin();
                    it != plist->value.end(); ++it) {
                //py_long *plongval2     = dynamic_cast<py_long *>(*it);
                py_int *pintval2     = dynamic_cast<py_int *>(*it);
                value.value[num++] = (int)*pintval2;
            }
            break;
        }
        default:
            break;
    }
    
    if (pval)
        delete pval;

    if (!abort) {
        // execute procedure command    
        state = kernel::request_cb(mod_name.c_str(), 
                MOD_REQUEST_CANOPEN_WRITE_ELEMENT_VALUE, (void *)&value);

        if (state != 0) {
            error_message = 
                "MOD_REQUEST_CANOPEN_WRITE_ELEMENT_VALUE failed!";
        }
    } 

func_exit:
    
    // response data
#define WRITE_ELEMENT_RESP_STATE            0
#define WRITE_ELEMENT_RESP_ERROR_MESSAGE    1
    response.resize(2);
    response[WRITE_ELEMENT_RESP_STATE] = state;
    response[WRITE_ELEMENT_RESP_ERROR_MESSAGE] = error_message;

    return 0;
}

const std::string canopen_protocol_handler::service_definition_write_element =
    "request:\n"
    "   uint16_t: index\n"
    "   uint8_t: sub_index\n"
    "   string: value\n"
    "response:\n"
    "   int32_t: state\n"
    "   string: error_message\n";

//! service callback list object dictionary
/*!
 * \param request service request data
 * \parma response service response data
 * \return success
 */
int canopen_protocol_handler::service_object_dictionary_list(
        const service_arglist_t& request, service_arglist_t& response) {
    canopen_object_dictionary_list list;
    list.slave_id = slave_id;
    list.indices_cnt = 0;
    list.indices = NULL;

    // default response values
    int64_t state = 0;
    std::vector<uint16_t> indices;
    string error_message = "";

    // receive cnt first
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST, (void *)&list);

    if (state == 0) {
        if (list.indices_cnt > 0) {
            // now allocate buffer and receive index list
            indices.resize(list.indices_cnt);
            list.indices = &indices[0];

            state = kernel::request_cb(mod_name.c_str(), 
                    MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST, (void *)&list);
        
            if (state != 0) {
                error_message = "MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST failed!";
            }
        }
    } else {
        error_message = "MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST failed!";
    }

#define OBJECT_DICTIONARY_LIST_RESP_STATE       0
#define OBJECT_DICTIONARY_LIST_RESP_INDICES     1
#define OBJECT_DICTIONARY_LIST_ERROR_MESSAGE    2
    response.resize(3);
    response[OBJECT_DICTIONARY_LIST_RESP_STATE]     = state;
    response[OBJECT_DICTIONARY_LIST_RESP_INDICES]   = indices;
    response[OBJECT_DICTIONARY_LIST_ERROR_MESSAGE]  = error_message;

    return 0;
}
        
const std::string canopen_protocol_handler::service_definition_object_dictionary_list =
    "response:\n"
    "   int64_t: state\n"
    "   vector/uint16_t: indices\n"
    "   string: error_message\n";

