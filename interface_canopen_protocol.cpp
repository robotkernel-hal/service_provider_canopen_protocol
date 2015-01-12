//! robotkernel module class
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

#include <string_util/string_util.h>

#include "interface_canopen_protocol.h"
#include "robotkernel/exceptions.h"
#undef BUILD_DATE
#undef PACKAGE
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef VERSION
#include "config.h"

using namespace std;
using namespace robotkernel;
using namespace interface;

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
} ec_datatype;

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
            return format_string("0x%02X", *(uint8_t *)usdo); 
        case ECT_UNSIGNED16:
            return format_string("0x%04X", *(uint16_t *)usdo); 
        case ECT_UNSIGNED32:
        case ECT_UNSIGNED24:
            return format_string("0x%08X", *(uint32_t *)usdo); 
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
 * \param mod_name module name to register for
 */
canopen_protocol::canopen_protocol(const std::string& mod_name, const std::string& dev_name, const int& slave_id) 
    : _mod_name(mod_name), _dev_name(dev_name), _slave_id(slave_id) {
    kernel& k = *kernel::get_instance();
    if (!k.clnt)
        throw robotkernel::str_exception("[interface_sercos_protocol|%s] no ln_connection!\n", 
                mod_name.c_str());

    stringstream base;
    base << k.clnt->name << "." << _mod_name << "." << _dev_name << ".";

    register_read_element(k.clnt, base.str() + "canopen_protocol.read_element");
    register_read_object(k.clnt, base.str() + "canopen_protocol.read_object");
    register_write_element(k.clnt, base.str() + "canopen_protocol.write_element");
    register_object_dictionary_list(k.clnt, base.str() + "canopen_protocol.object_dictionary_list");
}

int canopen_protocol::on_read_element(ln::service_request& req, 
        ln_service_robotkernel_canopen_protocol_read_element& svc) {

    canopen_element_description desc;
    memset(&desc, 0, sizeof(desc));
    desc.slave_id = _slave_id;
    desc.index = svc.req.index;
    desc.sub_index = svc.req.sub_index;
    
    // execute procedure command    
    svc.resp.state = kernel::request_cb(_mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC, (void *)&desc);

    svc.resp.name       = strdup(desc.name);
    svc.resp.name_len   = strlen(svc.resp.name);
    svc.resp.value_info = desc.value_info;
    svc.resp.data_type  = desc.data_type;
    svc.resp.bit_length = desc.bit_length;
    svc.resp.obj_access = desc.obj_access;

    
    canopen_element_value value;
    value.slave_id  = _slave_id;
    value.index     = svc.req.index;
    value.sub_index = svc.req.sub_index;
    value.value_len = (desc.bit_length + 7) / 8;
    value.value     = new uint8_t[value.value_len]();
    
    // execute procedure command    
    svc.resp.state = kernel::request_cb(_mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_VALUE, (void *)&value);

    svc.resp.value = strdup(value_2_string(value.value, value.value_len, desc.data_type).c_str());
    svc.resp.value_len = strlen(svc.resp.value);

    req.respond();

    if (value.value)
        delete[] value.value;
    if (svc.resp.value)
        free(svc.resp.value);
    if (svc.resp.name)
        free(svc.resp.name);

    return 0;
}
        
int canopen_protocol::on_read_object(ln::service_request& req, ln_service_robotkernel_canopen_protocol_read_object& svc) {
    canopen_object_description desc;
    memset(&desc, 0, sizeof(desc));
    desc.slave_id = _slave_id;
    desc.index = svc.req.index;

    // execute request
    svc.resp.state = kernel::request_cb(_mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_OBJECT_DESC, (void *)&desc);
    
    svc.resp.data_type      = desc.data_type;
    svc.resp.objcode        = desc.object_code;
    svc.resp.max_subindices = desc.max_subindices;
    svc.resp.name           = desc.name;
    svc.resp.name_len       = strlen(desc.name);

    req.respond();
    return 0;
}

int canopen_protocol::on_write_element(ln::service_request& req, ln_service_robotkernel_canopen_protocol_write_element& svc) {
    canopen_element_description desc;
    memset(&desc, 0, sizeof(desc));
    desc.slave_id = _slave_id;
    desc.index = svc.req.index;
    desc.sub_index = svc.req.sub_index;
    
    // execute procedure command    
    svc.resp.state = kernel::request_cb(_mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_READ_ELEMENT_DESC, (void *)&desc);

    canopen_element_value value;
    value.slave_id  = _slave_id;
    value.index     = svc.req.index;
    value.sub_index = svc.req.sub_index;
    value.value_len = (desc.bit_length + 7) / 8;
    value.value     = new uint8_t[value.value_len]();
    
    string buf(svc.req.value, svc.req.value_len);
    py_value *pval = eval_full(buf);
    py_int *pintval = dynamic_cast<py_int *>(pval);
    py_long *plongval = dynamic_cast<py_long *>(pval);
    py_float *pfloatval = dynamic_cast<py_float *>(pval);

    bool abort = false;

    switch (desc.data_type) {
        case ECT_BOOLEAN:
            if (!pintval) {
                abort = true;
                break;
            }

            (*(uint8_t *)value.value) = (bool)*pintval;
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
            if (!pfloatval) {
                abort = true;
                break;
            }

            (*(float *)value.value) = (float)*pfloatval;
            break;
        case ECT_REAL64:
            if (!pfloatval) {
                abort = true;
                break;
            }

            (*(double *)value.value) = (double)*pfloatval;
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
        case ECT_OCTET_STRING: {
//            string ans = "";
//
//            for (int i = 0 ; i < l ; i++)
//                ans += format_string("0x%2.2x ", usdo[i]);
//
//            return ans;
        }
        default:
            break;
    }
    
    if (pval)
        delete pval;

    if (!abort)
        // execute procedure command    
        svc.resp.state = kernel::request_cb(_mod_name.c_str(), 
                MOD_REQUEST_CANOPEN_WRITE_ELEMENT_VALUE, (void *)&value);
    else 
        svc.resp.state = -1;
    
    req.respond();
    
    if (value.value)
        delete[] value.value;

    return 0;
}
        
int canopen_protocol::on_object_dictionary_list(ln::service_request& req, ln_service_robotkernel_canopen_protocol_object_dictionary_list& svc) {
    canopen_object_dictionary_list list;
    list.slave_id = _slave_id;
    list.indices_cnt = 0;
    list.indices = NULL;

    // receive cnt first
    svc.resp.state = kernel::request_cb(_mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST, (void *)&list);

    if (list.indices_cnt > 0) {
        // now allocate buffer and receive index list
        list.indices = new uint16_t[list.indices_cnt];
    
        svc.resp.state = kernel::request_cb(_mod_name.c_str(), 
            MOD_REQUEST_CANOPEN_OBJECT_DICTIONARY_LIST, (void *)&list);
    }

    // answer service request
    svc.resp.indices = list.indices;
    svc.resp.indices_len = list.indices_cnt;
    req.respond();
    
    if (list.indices)
        delete[] list.indices;
    
    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

//! interface register
/*!
 * \param mod_name module name to register
 * \return interface handle
 */
INTERFACE_HANDLE intf_register(const char *mod_name, const char *dev_name, int slave_id) {
    canopen_protocol *s = NULL;

    klog(interface_info, INTFNAME "%s: build by: " BUILD_USER "@" BUILD_HOST "\n", mod_name);
    klog(interface_info, INTFNAME "%s: build date: " BUILD_DATE "\n", mod_name);

    // parsing sercos ring configuration
    try {
        s = new canopen_protocol(string(mod_name), string(dev_name), slave_id);
    } catch(exception& e) {
        klog(interface_error, INTFNAME "%s: error constructing intercae:\n%s", mod_name, e.what());
        goto ErrorExit;
    }

    return (INTERFACE_HANDLE)s;

ErrorExit:
    if (s)
        delete s;

    return (INTERFACE_HANDLE)NULL;
}

//! interface unregister
/*!
 * \param hdl interface handle
 */
void intf_unregister(INTERFACE_HANDLE hdl) {
    // cast struct
    canopen_protocol *s = (canopen_protocol *)hdl;

    if (s)
        delete s;
}

#if 0
{
#endif
#ifdef __cplusplus
}
#endif

