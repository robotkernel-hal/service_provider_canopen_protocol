//! robotkernel interface canopen protocol service definitions
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

#ifndef SERVICE_PROVIDER__CANOPEN_PROTOCOL__MDS__H
#define SERVICE_PROVIDER__CANOPEN_PROTOCOL__MDS__H

#include <string>

namespace service_provider {

namespace canopen_protocol {

extern const std::string service_definition_read_element;
extern const std::string service_definition_read_object;
extern const std::string service_definition_write_element;
extern const std::string service_definition_object_dictionary_list;
extern const std::string service_definition_pop_emergency_message;

} // namepace canopen_protocol

} // namespace service_provider

#endif // SERVICE_PROVIDER__CANOPEN_PROTOCOL__MDS__H


