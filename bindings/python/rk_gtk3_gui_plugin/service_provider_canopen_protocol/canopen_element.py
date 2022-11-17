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

from builtins import map
from builtins import object
import time
import gi


#gi.require_version('Gtk', '3.0')
gi.require_version('GLib', '2.0')
#from gi.repository import Gtk
from gi.repository import GObject

class canopen_element(object):
    def __init__(self, device, index, sub_index):
        self.canopen_device = device
        self.index = index
        self.sub_index = sub_index
        self.name = None
        self.value = None
        self.objcode = 0
        self.data_type = 0
        self.fd_get_data = None
        self.valid = False
        self.state = 0

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

        return (self.name, self.value, self.valid)

    def read(self):
        def cb_read(starttime):
            # callback after canopen returned with data
            data = self.canopen_device.svc_read_element.resp
            # FIXME: Replace this with a loop
            list(map(lambda x: setattr(self, x, getattr(data, x)), data.__dict__))
            self.valid = True
            self.canopen_device.svc_call_pending = False
            self.canopen_device.queue_draw()
            return False

        #non-blocking read on data, with callback (see get_data)
        self.canopen_device.svc_call_pending = True
        self.canopen_device.svc_read_element.req.index = self.index
        self.canopen_device.svc_read_element.req.sub_index = self.sub_index
        self.canopen_device.svc_read_element.call_async()
        
        # experimental: adapt to new LN API for GTK3
        #self.canopen_device.svc_read_element.gobject_on_async_finish(cb_read, time.time())

        self.canopen_device.svc_read_element.mainloop_on_async_finish(self.canopen_device.async_loop, cb_read, time.time())

