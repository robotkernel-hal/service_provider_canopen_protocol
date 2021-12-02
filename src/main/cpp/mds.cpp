#include "mds.h"
#include <list>

const std::string service_provider::canopen_protocol::service_definition_read_element =
"name: service_provider/canopen_protocol/read_element\n"
"request:\n"
"- uint16_t: index\n"
"- uint8_t: sub_index\n"
"response:\n"
"- string: name\n"
"- uint8_t: value_info\n"
"- uint16_t: data_type\n"
"- uint16_t: bit_length\n"
"- uint16_t: obj_access\n"
"- uint16_t: unit\n"
"- string: default_value\n"
"- string: min_value\n"
"- string: max_value\n"
"- string: value\n"
"- string: error_message\n";
        
const std::string service_provider::canopen_protocol::service_definition_read_object =
"name: service_provider/canopen_protocol/read_object\n"
"request:\n"
"- uint16_t: index\n"
"response:\n"
"- uint16_t: data_type\n"
"- uint8_t: objcode\n"
"- uint8_t: max_subindices\n"
"- string: name\n"
"- string: error_message\n";

const std::string service_provider::canopen_protocol::service_definition_write_element =
"name: service_provider/canopen_protocol/write_element\n"
"request:\n"
"- uint16_t: index\n"
"- uint8_t: sub_index\n"
"- string: value\n"
"response:\n"
"- string: error_message\n";

const std::string service_provider::canopen_protocol::service_definition_object_dictionary_list =
"name: service_provider/canopen_protocol/object_dictionary_list\n"
"response:\n"
"- vector/uint16_t: indices\n"
"- string: error_message\n";

const std::string service_provider::canopen_protocol::service_definition_pop_emergency_message =
"name: service_provider/canopen_protocol/pop_emergency_message\n"
"response:\n"
"- uint64_t: timestamp_sec\n"
"- uint64_t: timestamp_nsec\n"
"- uint16_t: error_code\n"
"- uint8_t: error_register\n"
"- vector/uint8_t: data\n"
"- string: error_message";

typedef void (*get_sd_t)(std::list<std::string>& sd_list);
extern "C" void get_sd(std::list<std::string>& sd_list) {
    sd_list.push_back(service_provider::canopen_protocol::service_definition_read_element);
    sd_list.push_back(service_provider::canopen_protocol::service_definition_read_object);
    sd_list.push_back(service_provider::canopen_protocol::service_definition_write_element);
    sd_list.push_back(service_provider::canopen_protocol::service_definition_object_dictionary_list);
    sd_list.push_back(service_provider::canopen_protocol::service_definition_pop_emergency_message);
}

