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
from __future__ import absolute_import

from builtins import map
from builtins import str
import os

import logging

logger = logging.getLogger()


import gi
gi.require_version('Gtk', '3.0')
gi.require_version('GLib', '2.0')
from gi.repository import Gtk
from gi.repository import GObject

from .canopen_treeview import canopen_treeview
from .canopen_device import canopen_device

import helpers

class canopen_protocol_view(helpers.service_provider_view, helpers.builder_base, canopen_treeview):
    def __init__(self, parent, container):
        logger.info("initializing canopen_protocol_view() with canopen_protocol_view.ui")
        
        fn = os.path.join(os.path.dirname(__file__), 'canopen_protocol_view.ui')
        helpers.builder_base.__init__(self, fn, 'canopen_protocol_box')
        helpers.service_provider_view.__init__(self, parent.app, parent, self.canopen_protocol_box, 'object_dictionary_list')

        container.pack_start(self.canopen_protocol_box, True, True, 0)

        self.devices = {}
        self.current_device = None

        self.canopen_protocol_emergencies_store = store = Gtk.ListStore(GObject.TYPE_PYOBJECT) # soem id type
        view = self.canopen_protocol_emergencies_tv
        view.set_model(store)
        view.set_border_width(4)
        view.set_headers_visible(True)

        # ------------------ index column ------------------
        def cb_datetime(column, cell, store, iter):
            row = store[iter]
            emergency = row[0]
            cell.set_property("xalign", 0.0)
            cell.set_property('text', str(row[0].timestamp))
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Datetime" , Gtk.CellRendererText(), cb_datetime)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        # ------------------ error_code column ------------------
        def cb_error_code(column, cell, store, iter):
            row = store[iter]
            emergency = row[0]
            cell.set_property("xalign", 0.0)
            cell.set_property('text', "0x%04X" % (row[0].error_code))
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Error-Code" , Gtk.CellRendererText(), cb_error_code)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        # ------------------ error_register column ------------------
        def cb_error_register(column, cell, store, iter):
            row = store[iter]
            cell.set_property("xalign", 0.0)
            cell.set_property('text', "0x%04X" % (row[0].error_register))
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Error-Register" , Gtk.CellRendererText(), cb_error_register)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        # ------------------ data column ------------------
        def cb_data(column, cell, store, iter):
            row = store[iter]
            cell.set_property("xalign", 0.0)
            cell.set_property('text', ', '.join(["%02X" % x for x in row[0].data]))
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Data" , Gtk.CellRendererText(), cb_data)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        #self.canopen_protocol_vpaned.set_position(500)
        #setattr(self.canopen_protocol_vpaned.get_child1(), "resize", True)
        #setattr(self.canopen_protocol_vpaned.get_child2(), "resize", False)
        self.canopen_protocol_refresh_btn.connect("clicked", self.on_refresh)
        self.active_color = helpers.gui_utils.get_active_color(self.app.window)
        self.create_treeview(self.canopen_protocol_tv)
        self.tv = self.treeview_dictionary


    # experimental for debugging
    def on_key_value_tv_button_press_event(self, btn, ev):
        logger.warning("handler on_key_value_tv_button_press_event "
                       "for object canopen_protocol_tv is not implemented - ignored")
        return True

    def on_canopen_protocol_refresh_emergencies_btn_clicked(self, btn):
        if self.current_device is None:
            return

        while True:
            e = self.current_device.upload_emergencies()
            if not e:
                break;

            itr = self.canopen_protocol_emergencies_store.append([e, ])

    def on_refresh(self, btn):
        if self.current_device is None:
            return

        for index in self.current_device.canopen_dictionary:
            for sub_index in self.current_device.canopen_dictionary[index].subindices:
                self.current_device.canopen_dictionary[index].subindices[sub_index].valid = False

        self.treeview_dictionary.queue_draw()

    def show(self, modname, devname):
        #if self.current_device and self.current_device.modname == modname and self.current_device.devname == devname:
        #    return

        modidx = (modname, devname)
        if modidx not in self.devices:
            self.devices[modidx] = canopen_device(self.service_prefix, self.app, self.treeview_dictionary, modname, devname)

        self.current_device = self.devices[modidx]
        helpers.service_provider_view.show(self)
        self.show_indices()

    def show_indices(self):
        self.treestore_dictionary.clear()
        self.current_device.list_dictionary()

        # FIXME: Replace this with a loop
        list(map(lambda x: self.treestore_dictionary.insert(None, -1, [x, "", self.current_device]),
                self.current_device.canopen_dictionary))

