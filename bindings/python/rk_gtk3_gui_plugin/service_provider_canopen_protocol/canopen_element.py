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
       # get data if necessary from canopen bus,
        # this will be called twice by the treeview to get name and value seperated
        if not self.valid or self.name is None or self.value is None:
            if not self.canopen_device.svc_call_pending:
                self.read()
            elif not self.fd_get_data:
                self.fd_get_data = GObject.timeout_add(10, self.update_callback, (force_update))

        return (self.name, self.value, self.valid)

    def read(self):
        def cb_read(data):
            print("cb_read(): callback called!")
            #list(map(lambda x: setattr(self, x, getattr(data, x)), data.__dict__))
            for x in data.__dict__:
                setattr(self, x, getattr(data, x))
            self.valid = True
            return False
        
        self.canopen_device.read_element(self.index, self.sub_index, callback=cb_read)

