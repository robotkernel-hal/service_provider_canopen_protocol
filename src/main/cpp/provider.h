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

#include <string.h>

#include "robotkernel/service_provider_base.h"
#include "robotkernel/service_provider_intf.h"
#include "robotkernel/service.h"
#include "robotkernel/kernel.h"
#include "robotkernel/log_base.h"

#include "service_provider/canopen_protocol/base.h"

namespace service_provider {
#if EMACS
}
#endif

namespace canopen_protocol {

    // forward declaration
    class handler;

    class provider : public robotkernel::service_provider_base<handler, base> 
    {
        public:
            //! default construction
            /*!
            */
            provider() 
                : service_provider_base("canopen_protocol") {};
    };

    class handler : public robotkernel::log_base
    {
        public:
            typedef std::shared_ptr<service_provider::canopen_protocol::base> sp_cp_base_t;
            sp_cp_base_t _instance;

            //! handler construction
            handler(const robotkernel::sp_service_interface_t& req);

            //! handler destruction
            ~handler();

            //! service callback read element
            /*!
             * \param request service request data
             * \parma response service response data
             * \return success
             */
            int service_read_element(const robotkernel::service_arglist_t& request, 
                    robotkernel::service_arglist_t& response);
            static const std::string service_definition_read_element;

            //! service callback read object
            /*!
             * \param request service request data
             * \parma response service response data
             * \return success
             */
            int service_read_object(const robotkernel::service_arglist_t& request, 
                    robotkernel::service_arglist_t& response);
            static const std::string service_definition_read_object;

            //! service callback write element
            /*!
             * \param request service request data
             * \parma response service response data
             * \return success
             */
            int service_write_element(const robotkernel::service_arglist_t& request, 
                    robotkernel::service_arglist_t& response);
            static const std::string service_definition_write_element;

            //! service callback list object dictionary
            /*!
             * \param request service request data
             * \parma response service response data
             * \return success
             */
            int service_object_dictionary_list(const robotkernel::service_arglist_t& request, 
                    robotkernel::service_arglist_t& response);
            static const std::string service_definition_object_dictionary_list;
    };

} // namepace canopen_protocol

#if EMACS
{
#endif
} // namespace service_provider

#endif // __SERVICE_PROVIDER__CANOPEN_PROTOCOL_H__

