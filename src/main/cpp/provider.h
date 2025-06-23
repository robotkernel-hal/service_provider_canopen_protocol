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

#ifndef __SERVICE_PROVIDER__CANOPEN_PROTOCOL_H__
#define __SERVICE_PROVIDER__CANOPEN_PROTOCOL_H__

// Standard includes
#include <string.h>

// Robotkernel includes
#include "robotkernel/service_provider_base.h"
#include "robotkernel/service.h"
#include "robotkernel/robotkernel.h"
#include "robotkernel/log_base.h"

// Service provider includes
#include "service_provider/canopen_protocol/base.h"
#include "service_definitions.h"

namespace service_provider {
namespace canopen_protocol {

// forward declaration
class handler;

class provider : 
    public robotkernel::service_provider_base<handler, base>
{
    public:
        //! default construction
        /*!
        */
        provider(const std::string& name) 
            : service_provider_base(name, "canopen_protocol") {};
};

class handler : 
    public robotkernel::log_base,
    public svc_base_read_element,
    public svc_base_write_element,
    public svc_base_read_object,
    public svc_base_object_dictionary_list,
    public svc_base_pop_emergency_message
{
    public:
        typedef std::shared_ptr<service_provider::canopen_protocol::base> sp_cp_base_t;
        sp_cp_base_t _instance;

        //! handler construction
        handler(const robotkernel::sp_service_interface_t& req);

        //! handler destruction
        ~handler();

        //! svc_read_element
        /*!
         * \param[in]   req     Service request data.
         * \param[out]  resp    Service response data.
         */
        virtual void svc_read_element(const struct svc_req_read_element& req, struct svc_resp_read_element& resp);

        //! svc_read_object
        /*!
         * \param[in]   req     Service request data.
         * \param[out]  resp    Service response data.
         */
        virtual void svc_read_object(const struct svc_req_read_object& req, struct svc_resp_read_object& resp);

        //! svc_write_element
        /*!
         * \param[in]   req     Service request data.
         * \param[out]  resp    Service response data.
         */
        virtual void svc_write_element(const struct svc_req_write_element& req, struct svc_resp_write_element& resp);

        //! svc_object_dictionary_list
        /*!
         * \param[in]   req     Service request data.
         * \param[out]  resp    Service response data.
         */
        virtual void svc_object_dictionary_list(const struct svc_req_object_dictionary_list& req, struct svc_resp_object_dictionary_list& resp);

        //! svc_pop_emergency_message
        /*!
         * \param[in]   req     Service request data.
         * \param[out]  resp    Service response data.
         */
        virtual void svc_pop_emergency_message(const struct svc_req_pop_emergency_message& req, struct svc_resp_pop_emergency_message& resp);
};

} // namepace canopen_protocol
} // namespace service_provider

#endif // __SERVICE_PROVIDER__CANOPEN_PROTOCOL_H__

