# service_provider_canopen_protocol

This supplies a service provider for all robotkernel modules with support for canopen protocol devices. The service provider will provide acyclic calls to access the canopen dictionary entries.

- get object dictionary list: returns a list with all available ids on the device
- get object description: returns the description of the id
- get element description: returns the description of one element of an id
- read element: reads one element value of an id
- write element: wristes one element value of an id
