ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libservice_provider_canopen_protocol.la 

BUILT_SOURCES = $(top_builddir)/src/service_definitions.h
CLEANFILES = $(top_builddir)/src/service_definitions.h

extraincludedir = $(includedir)/service_provider_canopen_protocol
extrainclude_HEADERS = $(top_srcdir)/include/service_provider_canopen_protocol/base.h

libservice_provider_canopen_protocol_la_SOURCES  = provider.cpp
libservice_provider_canopen_protocol_la_CXXFLAGS = -fno-strict-aliasing -Wall 			\
												   -I$(srcdir) -I$(top_srcdir)/include -I$(top_builddir)/include 	\
												   @ROBOTKERNEL_CFLAGS@
libservice_provider_canopen_protocol_la_LDFLAGS  = -Wall -release $(PACKAGE_VERSION)

VPATH = $(srcdir):$(top_srcdir)/share/service_definitions

SERVICE_DEFINITIONS = service_provider_canopen_protocol/read_element \
					  service_provider_canopen_protocol/read_object \
					  service_provider_canopen_protocol/write_element \
					  service_provider_canopen_protocol/object_dictionary_list \
					  service_provider_canopen_protocol/pop_emergency_message

$(top_builddir)/src/service_definitions.h: Makefile $(SERVICE_DEFINITIONS)
	@echo creating $@
	rk_generate --indir $(top_srcdir)/share/service_definitions --out $@ $(SERVICE_DEFINITIONS)
	touch $@


