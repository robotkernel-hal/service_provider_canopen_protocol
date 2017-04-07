ACLOCAL_AMFLAGS = -I m4

libdir = $(prefix)/lib
lib_LTLIBRARIES = libservice_provider_canopen_protocol.la

extraincludedir = $(includedir)/service_provider/canopen_protocol
extrainclude_HEADERS = include/service_provider/canopen_protocol/base.h

libservice_provider_canopen_protocol_la_SOURCES  = provider.cpp
libservice_provider_canopen_protocol_la_CXXFLAGS = --std=c++11 -fno-strict-aliasing -Wall 			\
												   -I$(srcdir) -I$(builddir) -I$(srcdir)/include 	\
												   @ROBOTKERNEL_CFLAGS@ 							\
												   @LIBSTRING_UTIL_CFLAGS@ 							\
												   @YAML_CPP_CFLAGS@
libservice_provider_canopen_protocol_la_LDFLAGS  = -Wall -release $(PACKAGE_VERSION)

