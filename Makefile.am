ACLOCAL_AMFLAGS = -I m4 

libdir = $(prefix)/lib
lib_LTLIBRARIES = libinterface_canopen_protocol.la
BUILT_SOURCES = ln_messages.h
CLEANFILES = ln_messages.h

libinterface_canopen_protocol_la_SOURCES  = interface_canopen_protocol.cpp

libinterface_canopen_protocol_la_CXXFLAGS = -fno-strict-aliasing \
											-Wall \
											-I$(top_srcdir)/src \
											-I$(top_builddir) \
											@ROBOTKERNEL_CFLAGS@ 

libinterface_canopen_protocol_la_LDFLAGS  = -Wall -release $(PACKAGE_VERSION)

VPATH = $(srcdir):$(srcdir)/share/message_definitions

LN_MESSAGES = robotkernel/canopen_protocol/read_object \
			  robotkernel/canopen_protocol/read_element \
			  robotkernel/canopen_protocol/write_element \
			  robotkernel/canopen_protocol/object_dictionary_list

ln_messages.h: Makefile $(LN_MESSAGES)
	@echo creating ln_messages.h
	@LN_GENERATE@ -o $@ $(LN_MESSAGES)
	touch $@

install-exec-hook:
	mkdir -p $(prefix)/share
	-cp -rf $(srcdir)/share/* $(prefix)/share/

