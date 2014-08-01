libdir = $(prefix)/lib/robotkernel/interfaces
lib_LTLIBRARIES = libinterface_canopen_protocol.la
BUILT_SOURCES = ln_messages.h
CLEANFILES = ln_messages.h

libinterface_canopen_protocol_la_SOURCES  = interface_canopen_protocol.cpp
libinterface_canopen_protocol_la_CXXFLAGS = -fno-strict-aliasing -Wall -I$(top_srcdir)/src \
										   -I$(top_builddir) @YAMLCPP_CFLAGS@ @LN_CFLAGS@ 
libinterface_canopen_protocol_la_LDFLAGS  = -Wall @PTHREAD_LIBS@ @RT_LIBS@ -release $(PACKAGE_VERSION)

VPATH = $(srcdir):$(srcdir)/message_definitions

LN_MESSAGES = \
			  canopen_protocol/read_object \
			  canopen_protocol/read_element \
			  canopen_protocol/write_element \
			  canopen_protocol/object_dictionary_list

ln_messages.h: Makefile $(LN_MESSAGES)
	@echo creating ln_messages.h
	@LN_GENERATE@ -o $@ $(LN_MESSAGES)
	touch $@

install-exec-hook:
	mkdir -p $(prefix)/share/message_definitions
	cp -r $(srcdir)/message_definitions/* $(prefix)/share/message_definitions/
	
