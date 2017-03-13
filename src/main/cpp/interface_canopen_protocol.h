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

#ifndef __INTERFACE_CANOPEN_PROTOCOL_H__
#define __INTERFACE_CANOPEN_PROTOCOL_H__

#include <string.h>

#include "module_intf.h"
#include "robotkernel/service_provider_base.h"
#include "robotkernel/service_provider_intf.h"
#include "robotkernel/service.h"
#include "robotkernel/kernel.h"
#include "robotkernel/log_base.h"

#include "service_provider/canopen_protocol/base.h"

namespace interface_canopen_protocol {
	extern const char* canopen_protocol_sp_magic;

	// forward declaration
	class canopen_protocol_handler;

	class canopen_protocol : 
		public robotkernel::service_provider_base<canopen_protocol_handler> {
		public:
			//! default construction
			/*!
			 * \param node configuration node
			 */
			canopen_protocol() 
				: service_provider_base("canopen_protocol") {};

			//! service provider magic 
			/*!
			 * \return return service provider magic string
			 */
			const char* get_sp_magic() 
			{ return canopen_protocol_sp_magic; };
	};

	class canopen_protocol_handler : public robotkernel::log_base {
		public:
            service_provider::canopen_protocol::base *instance;
			std::string mod_name;	//!< slave owner module
			std::string dev_name;	//!< service device name
			int slave_id;			//!< slave identifier

			//! handler construction
			canopen_protocol_handler(std::string mod_name, std::string dev_name, 
					int slave_id);

			//! handler destruction
			~canopen_protocol_handler();

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

} // namespace interface

#endif // __INTERFACE_CANOPEN_PROTOCOL_H__

