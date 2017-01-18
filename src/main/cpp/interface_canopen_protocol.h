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
#include "robotkernel/interface_base.h"

namespace interface_canopen_protocol {
    
class canopen_protocol : robotkernel::interface_base {
    public:
        //! default construction
        /*!
         * \param node configuration node
         */
        canopen_protocol(const YAML::Node& node);

        //! service callback read element
        /*!
         * \param message service message
         * \return success
         */
        int service_read_element(YAML::Node& message);
        static const std::string service_definition_read_element;

        //! service callback read object
        /*!
         * \param message service message
         * \return success
         */
        int service_read_object(YAML::Node& message);
        static const std::string service_definition_read_object;

        //! service callback write element
        /*!
         * \param message service message
         * \return success
         */
        int service_write_element(YAML::Node& message);
        static const std::string service_definition_write_element;

        //! service callback list object dictionary
        /*!
         * \param message service message
         * \return success
         */
        int service_object_dictionary_list(YAML::Node& message);
        static const std::string service_definition_object_dictionary_list;

};

} // namespace interface

#endif // __INTERFACE_CANOPEN_PROTOCOL_H__

