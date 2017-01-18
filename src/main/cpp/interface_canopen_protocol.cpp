//! robotkernel interface canopen protocol
/*!
 * author: Robert Burger
 *
 * $Id$
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
#include <string_util/string_util.h>

#include "interface_canopen_protocol.h"
#include "robotkernel/kernel.h"
#include "robotkernel/exceptions.h"

INTERFACE_DEF(canopen_protocol, interface_canopen_protocol::canopen_protocol)

using namespace std;
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

typedef std::map<uint32_t, std::string> abort_code_map_t;
const abort_code_map_t::value_type raw_abort_code[] = {
    abort_code_map_t::value_type( 0x00000000, "No error" ),
    abort_code_map_t::value_type( 0x05030000, "Toggle bit not changed" ),
    abort_code_map_t::value_type( 0x05040000, "SDO protocol timeout" ),
    abort_code_map_t::value_type( 0x05040001, "Client/Server command specifier not valid or unknown" ),
    abort_code_map_t::value_type( 0x05040005, "Out of memory" ),
    abort_code_map_t::value_type( 0x06010000, "Unsupported access to an object" ),
    abort_code_map_t::value_type( 0x06010001, "Attempt to read to a write only object" ),
    abort_code_map_t::value_type( 0x06010002, "Attempt to write to a read only object" ),
    abort_code_map_t::value_type( 0x06010003, "Subindex can not be written, SI0 must be 0 for write access" ),
    abort_code_map_t::value_type( 0x06010004, "SDO Complete access not supported for variable length objects" ),
    abort_code_map_t::value_type( 0x06010005, "Object length exceeds mailbox size" ),
    abort_code_map_t::value_type( 0x06010006, "Object mapped to RxPDO, SDO download blocked" ),
    abort_code_map_t::value_type( 0x06020000, "The object does not exist in the object directory" ),
    abort_code_map_t::value_type( 0x06040041, "The object can not be mapped into the PDO" ),
    abort_code_map_t::value_type( 0x06040042, "The number and length of the objects to be mapped would exceed the PDO length" ),
    abort_code_map_t::value_type( 0x06040043, "General parameter incompatibility reason" ),
    abort_code_map_t::value_type( 0x06040047, "General internal incompatibility in the device" ),
    abort_code_map_t::value_type( 0x06060000, "Access failed due to a hardware error" ),
    abort_code_map_t::value_type( 0x06070010, "Data type does not match, length of service parameter does not match" ),
    abort_code_map_t::value_type( 0x06070012, "Data type does not match, length of service parameter too high" ),
    abort_code_map_t::value_type( 0x06070013, "Data type does not match, length of service parameter too low" ),
    abort_code_map_t::value_type( 0x06090011, "Subindex does not exist" ),
    abort_code_map_t::value_type( 0x06090030, "Value range of parameter exceeded (only for write access)" ),
    abort_code_map_t::value_type( 0x06090031, "Value of parameter written too high" ),
    abort_code_map_t::value_type( 0x06090032, "Value of parameter written too low" ),
    abort_code_map_t::value_type( 0x06090036, "Maximum value is less than minimum value" ),
    abort_code_map_t::value_type( 0x08000000, "General error" ),
    abort_code_map_t::value_type( 0x08000020, "Data cannot be transferred or stored to the application" ),
    abort_code_map_t::value_type( 0x08000021, "Data cannot be transferred or stored to the application because of local control" ),
    abort_code_map_t::value_type( 0x08000022, "Data cannot be transferred or stored to the application because of the present device state" ),
    abort_code_map_t::value_type( 0x08000023, "Object dictionary dynamic generation fails or no object dictionary is present" ),
    abort_code_map_t::value_type( 0xffffffff, "Unknown" )
};

const int num_elems = sizeof raw_abort_code / sizeof raw_abort_code[0];
abort_code_map_t abort_code_map(raw_abort_code, raw_abort_code + num_elems);

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

//! default construction
/*!
 * \param node configuration node
 */
canopen_protocol::canopen_protocol(const YAML::Node& node)
    : interface_base("canopen_protocol", node) {
    kernel& k = *kernel::get_instance();

    stringstream base;
    base << mod_name << "." << dev_name 
        << ".canopen_protocol.";

    k.add_service(mod_name, base.str() + "read_element", 
            service_definition_read_element,
            boost::bind(&canopen_protocol::service_read_element, this, _1));
    k.add_service(mod_name, base.str() + "read_object", 
            service_definition_read_object,
            boost::bind(&canopen_protocol::service_read_object, this, _1));
    k.add_service(mod_name, base.str() + "write_element", 
            service_definition_write_element,
            boost::bind(&canopen_protocol::service_write_element, this, _1));
    k.add_service(mod_name, base.str() + "object_dictionary_list", 
            service_definition_object_dictionary_list,
            boost::bind(&canopen_protocol::service_object_dictionary_list, 
                this, _1));

//    register_write_element(k.clnt, base.str() + "canopen_protocol.write_element");
}

//! service callback read element
/*!
 * \param message service message
 * \return success
 */
int canopen_protocol::service_read_element(YAML::Node& message) {
    canopen_element_description desc;
    canopen_element_value value;
    unsigned state = 0;
    memset(&desc, 0, sizeof(desc));
    memset(&value, 0, sizeof(value));

    desc.slave_id   = slave_id;
    desc.index      = get_as<uint16_t>(message["request"], "index");
    desc.sub_index  = get_as<uint8_t>(message["request"], "sub_index");
    desc.name       = NULL;
    desc.name_len   = 0;

    // default response data
    message["response"]["state"]            = 0;
    message["response"]["name"]             = "";
    message["response"]["value_info"]       = 0;
    message["response"]["data_type"]        = 0;
    message["response"]["bit_length"]       = 0;
    message["response"]["obj_access"]       = 0;
    message["response"]["value"]            = "";
    message["response"]["error_message"]    = "";
    
    // execute module request read element description
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC, (void *)&desc);

    if (state != 0) {
        message["response"]["state"] = state;
        message["response"]["error_message"] = 
            "MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC failed!";
        return 0;
    }

    if (desc.name && desc.name_len) {
        message["response"]["name"] = string(desc.name, desc.name_len);
    }

    message["response"]["value_info"] = desc.value_info;
    message["response"]["data_type"]  = desc.data_type;
    message["response"]["bit_length"] = desc.bit_length;
    message["response"]["obj_access"] = desc.obj_access;

    value.slave_id  = slave_id;
    value.index     = get_as<uint16_t>(message["request"], "index");
    value.sub_index = get_as<uint8_t>(message["request"], "sub_index");
    value.value_len = (desc.bit_length + 7) / 8;
    value.value     = new uint8_t[value.value_len]();
    
    // execute module request read element value
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE, (void *)&value);

    if (state == 0) {
        message["response"]["value"] = value_2_string(
                value.value, value.value_len, desc.data_type);
    } else {
        message["response"]["state"] = state;

        if (abort_code_map.find(state) != abort_code_map.end()) {
            message["response"]["error_message"] = 
                string("MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE failed: ") + 
                abort_code_map[state];
        } else {
            message["response"]["error_message"] = 
                "MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE failed!";
        }
    }

    return 0;
}

const std::string canopen_protocol::service_definition_read_element =
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
 * \param message service message
 * \return success
 */
int canopen_protocol::service_read_object(YAML::Node& message) {
    canopen_object_description desc;
    memset(&desc, 0, sizeof(desc));
    unsigned state = 0;
    desc.slave_id = slave_id;
    desc.index = get_as<uint16_t>(message["request"], "index");

    // default response data
    message["response"]["state"] = 0;
    message["response"]["data_type"] = 0;
    message["response"]["objcode"] = 0;
    message["response"]["max_subindices"] = 0;
    message["response"]["name"] = "";
    message["response"]["error_message"] = "";
    
    // execute request
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_OBJECT_DESC, (void *)&desc);

    if (state == 0) {
        message["response"]["data_type"] = desc.data_type;
        message["response"]["objcode"] = desc.object_code;
        message["response"]["max_subindices"] = desc.max_subindices;
        
        if (desc.name && desc.name_len) {
            message["response"]["name"] = string(desc.name, desc.name_len);
        }
    } else {
        message["response"]["state"] = state;
        message["response"]["error_message"] = 
            "MOD_REQUEST_CANOPEN_READ_OBJECT_DESC failed!";
    }
    
    return 0;
}

const std::string canopen_protocol::service_definition_read_object =
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
 * \param message service message
 * \return success
 */
int canopen_protocol::service_write_element(YAML::Node& message) {
    canopen_element_description desc;
    unsigned state = 0;
    memset(&desc, 0, sizeof(desc));
    desc.slave_id   = slave_id;
    desc.index      = get_as<uint16_t>(message["request"], "index");
    desc.sub_index  = get_as<uint8_t>(message["requset"], "sub_index");
    
    // default response values
    message["response"]["state"] = 0;
    message["response"]["error_message"] = "";
    
    // execute procedure command    
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC, (void *)&desc);

    if (state != 0) {
        message["response"]["state"] = state;
        message["response"]["error_message"] = 
            "MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC failed!";

        return 0;
    }

    canopen_element_value value;
    value.slave_id  = slave_id;
    value.index     = get_as<uint16_t>(message["request"], "index");
    value.sub_index = get_as<uint8_t>(message["request"], "sub_index");
    value.value_len = (desc.bit_length + 7) / 8;
    value.value     = new uint8_t[value.value_len]();
    
    string buf = get_as<string>(message["request"], "value");
    py_value *pval      = eval_full(buf);
    py_int *pintval     = dynamic_cast<py_int *>(pval);
    py_long *plongval   = dynamic_cast<py_long *>(pval);
    py_float *pfloatval = dynamic_cast<py_float *>(pval);
    py_special *pspval  = dynamic_cast<py_special *>(pval);

    bool abort = false;

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

            message["response"]["state"] = -1;
            message["response"]["error_message"] = 
                string("not implemented data_type: ") +
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
                py_long *plongval2     = dynamic_cast<py_long *>(*it);
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
            message["response"]["state"] = state;
            message["response"]["error_message"] = 
                "MOD_REQUEST_CANOPEN_WRITE_ELEMENT_VALUE failed!";
        }
    } 
    
    return 0;
}

const std::string canopen_protocol::service_definition_write_element =
    "request\n"
    "   uint16_t: index\n"
    "   uint8_t: sub_index\n"
    "   string: value\n"
    "response\n"
    "   int32_t: state\n"
    "   string: error_message\n";

//! service callback list object dictionary
/*!
 * \param message service message
 * \return success
 */
int canopen_protocol::service_object_dictionary_list(YAML::Node& message) {
    canopen_object_dictionary_list list;
    list.slave_id = slave_id;
    list.indices_cnt = 0;
    list.indices = NULL;
    unsigned state = 0;

    // default response values
    std::vector<uint16_t> values;
    message["response"]["state"] = 0;
    message["response"]["indices"] = values;
    message["response"]["error_message"] = "";

    // receive cnt first
    state = kernel::request_cb(mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST, (void *)&list);

    if (state == 0) {
        if (list.indices_cnt > 0) {
            // now allocate buffer and receive index list
            values.resize(list.indices_cnt);
            list.indices = &values[0];

            state = kernel::request_cb(mod_name.c_str(), 
                    MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST, (void *)&list);
        
            if (state == 0)
                message["response"]["indices"] = values;
            else {
                message["response"]["state"] = state;
                message["response"]["error_message"] =
                    "MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST failed!";
            }
        }
    } else {
        message["response"]["state"] = state;
        message["response"]["error_message"] = 
            "MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST failed!";
    }

    return 0;
}
        
const std::string canopen_protocol::service_definition_object_dictionary_list =
    "response:\n"
    "   int64_t: state\n"
    "   uint16_t*: indices\n"
    "   string: error_message\n";

