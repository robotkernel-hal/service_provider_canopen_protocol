'''
(C) Robert Burger <robert.burger@dlr.de>

This file is part of Robotkernel-GUI.

Robotkernel-GUI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Robotkernel-GUI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Robotkernel-GUI.  If not, see <http://www.gnu.org/licenses/>.
'''
from __future__ import print_function
from __future__ import absolute_import

from builtins import map
from builtins import range
from builtins import object
import gi
#gi.require_version('Gtk', '3.0')
#gi.require_version('GLib', '2.0')
#from gi.repository import Gtk
from gi.repository import GObject
import time
import traceback
import logging


from .canopen_element import *

logger = logging.getLogger()

class canopen_object(object):
    def __init__(self, device, idn):
        self.canopen_device = device
        self.idn = idn
        self.name = None
        self.value = None
        self.objcode = 0
        self.data_type = 0
        self.max_subindices = 0
        self.fd_get_data = None
        self.valid = False
        self.subindices = { }

    def update_callback(self, force_update=False):
        self.fd_get_data = None
        self.get_data(force_update)
        return False

    def get_data(self, force_update=False):
        # get data if nesecary from canopen bus,
        # this will be called twice by the treeview to get name and value seperated
        if not self.valid or self.name is None or self.value is None:
            if not self.canopen_device.svc_call_pending:
                self.read()
            elif not self.fd_get_data:
                self.fd_get_data = GObject.timeout_add(10, self.update_callback, (force_update))

        return (self.idn, self.name, self.value, self.valid)

    def read(self):
        def cb_read(starttime):
            #callback after canopen returned with data
            try:
                self.canopen_device.svc_read_object.utf8_decode_char_fields(False)
                
                data = self.canopen_device.svc_read_object.resp
                # FIXME: Replace this with a loop
                list(map(lambda x: setattr(self, x, getattr(data, x)), data.__dict__))
                try:
                    self.name = data.name.decode('latin1')
                except AttributeError as exc:
                    logger.warning("canopen_object.getdata(): data.name field"
                                   " is already of type string, using field directly")
                    self.name = data.name
                    
                self.objcode = data.objcode
                self.max_subindices = data.max_subindices
                self.subindices.clear()
                for i in range(0, self.max_subindices+1):
                    self.subindices[i] = canopen_element(self.canopen_device, self.idn, i)
                self.value = 0
                self.valid = True
                self.canopen_device.svc_call_pending = False
                self.canopen_device.queue_draw()
            except:
                print(traceback.format_exc())
            return False

        #non-blocking read on data, with callback (see get_data)
        self.canopen_device.svc_call_pending = True
        self.canopen_device.svc_read_object.req.index = self.idn
        self.canopen_device.svc_read_object.call_async()
        # old GTK2 code: set async finish handler
        #self.canopen_device.svc_read_object.gobject_on_async_finish(cb_read, time.time())
        # new GTK2 code
        self.canopen_device.svc_read_object.mainloop_on_async_finish(self.canopen_device.async_loop, cb_read, time.time())

