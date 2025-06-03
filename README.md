That service provider suppliles services for all robotkernel modules
with support for canopen protocol devices. The service provider will
provide acyclic calls to access the canopen dictionary entries.

# Configuration

No specific configuration is needed for the service provider. The
robotkernel just needs to know which service provider to load. To load
der *service_provider_canopen_device* just add it to your config file.

```yaml
service_providers:
- name: canopen
  so_file: libservice_provider_canopen_protocol.so
```

# Services

**get object dictionary list**
:   Returns a list *indices* with all available ids on the device. On
    error, the *error_message* field will be filled with the error
    cause.

```yaml
response:
- vector/uint16_t: indices
- string: error_message
```

**read_object**
:   Returns the description of the object with given *index*. On error,
    the *error_message* field will be filled with the error cause.

```yaml
request:
- uint16_t: index
response:
- uint16_t: data_type
- uint8_t: objcode
- uint8_t: max_subindices
- string: name
- string: error_message
```

**read element**
:   Reads one element value of an id with given *index* and *sub_index*.

```yaml
request:
- uint16_t: index
- uint8_t: sub_index
response:
- string: name
- uint8_t: value_info
- uint16_t: data_type
- uint16_t: bit_length
- uint16_t: obj_access
- uint16_t: unit
- string: default_value
- string: min_value
- string: max_value
- string: value
- string: error_message
```

**write element**
:   Writes *value to one element (*sub index*) of an*index*. The
    datatype of*value*must match to the datatype of the element. On
    error, the*error_message\'\' field will be filled with the error
    cause.

```yaml
request:
- uint16_t: index
- uint8_t: sub_index
- string: value
response:
- string: error_message
```

All of these services will be available through a robotkernel brigdge
(e.g. [bridge_ln](robotkernel-5/bridge_ln "wikilink"),
[bridge_jsonrpc](robotkernel-5/bridge_jsonrpc "wikilink"),
[bridge_cli](robotkernel-5/bridge_cli "wikilink"), \...)

[Serivice Provider Canopen Protocol](Category:Robotkernel-5 "wikilink")
