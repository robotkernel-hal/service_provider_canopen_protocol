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

#ifndef __INTERFACE_CANOPEN_PROTOCOL_H__
#define __INTERFACE_CANOPEN_PROTOCOL_H__

#include "module_intf.h"

#define LN_UNREGISTER_SERVICE_IN_BASE_DETOR  
#include "ln_messages.h"
#undef LN_UNREGISTER_SERVICE_IN_BASE_DETOR

#define INTFNAME "[interface_canopen_protocol] "

namespace interface {
    
class canopen_protocol : 
    public ln_service_read_element_base,
    public ln_service_read_object_base,
    public ln_service_write_element_base,
    public ln_service_object_dictionary_list_base
{
    std::string _mod_name;  //! request module name
    std::string _dev_name;  //! service device name
    int _slave_id;          //! request slave id

    public:
        //! default construction
        /*!
         * \param mod_name module name to send requests to
         * \param dev_name device name for service
         * \param slave_id slave id to send requests to
         */
        canopen_protocol(const std::string& mod_name, 
                const std::string& dev_name, const int& slave_id);

        //! service callback read element
        int on_read_element(ln::service_request& req,
                ln_service_robotkernel_canopen_protocol_read_element& svc);
        //! service callback read object
        int on_read_object(ln::service_request& req,
                ln_service_robotkernel_canopen_protocol_read_object& svc);
        //! service callback write element
        int on_write_element(ln::service_request& req,
                ln_service_robotkernel_canopen_protocol_write_element& svc);
        //! service callback list object dictionary
        int on_object_dictionary_list(ln::service_request& req,
                ln_service_robotkernel_canopen_protocol_object_dictionary_list& svc);
};

} // namespace interface

#endif // __INTERFACE_CANOPEN_PROTOCOL_H__

