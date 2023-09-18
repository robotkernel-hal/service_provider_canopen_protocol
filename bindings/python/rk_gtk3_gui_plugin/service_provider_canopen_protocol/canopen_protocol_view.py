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
        def cb_datetime(column, cell, store, tree_iter):
            row = store[tree_iter]
            emergency = row[0]
            cell.set_property("xalign", 0.0)
            cell.set_property('text', str(row[0].timestamp))
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Datetime" , Gtk.CellRendererText(), cb_datetime)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        # ------------------ error_code column ------------------
        def cb_error_code(column, cell, store, tree_iter):
            row = store[tree_iter]
            emergency = row[0]
            cell.set_property("xalign", 0.0)
            cell.set_property('text', "0x%04X" % (row[0].error_code))
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Error-Code" , Gtk.CellRendererText(), cb_error_code)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        # ------------------ error_register column ------------------
        def cb_error_register(column, cell, store, tree_iter):
            row = store[tree_iter]
            cell.set_property("xalign", 0.0)
            cell.set_property('text', "0x%04X" % (row[0].error_register))
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Error-Register" , Gtk.CellRendererText(), cb_error_register)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        # ------------------ data column ------------------
        def cb_data(column, cell, store, tree_iter):
            row = store[tree_iter]
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
        self.active_color = helpers.gui_utils.get_active_color_str(self.app.window)
        self.create_treeview(self.canopen_protocol_tv)
        self.tv = self.treeview_dictionary
        self.tv.expand_all()


    # experimental for debugging
    #def on_key_value_tv_button_press_event(self, btn, ev):
    #    logger.warning("handler on_key_value_tv_button_press_event "
    #                   "for object canopen_protocol_tv is not implemented - ignored")
    #    return True

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
        #self.treeview_dictionary.expand_all()

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

        d = self.current_device.canopen_dictionary
        commun = {key: value for i, (key, value) in enumerate(d.items()) if (key >= 0x1000 and key <  0x1600) or (key >= 0x1800 and key < 0x1A00) or (key > 0x1C00 and key < 0x2000)}
        rxpdos = {key: value for i, (key, value) in enumerate(d.items()) if key >= 0x1600 and key <  0x1800}
        txpdos = {key: value for i, (key, value) in enumerate(d.items()) if key >= 0x1A00 and key <  0x1C00}
        user   = {key: value for i, (key, value) in enumerate(d.items()) if key >= 0x2000 and key <  0x6000}
        profile= {key: value for i, (key, value) in enumerate(d.items()) if key >= 0x6000 and key <  0xA000}
        diag   = {key: value for i, (key, value) in enumerate(d.items()) if key >= 0xA000 and key <  0xB000}
        device = {key: value for i, (key, value) in enumerate(d.items()) if key >= 0xF000}
        rest   = {key: value for i, (key, value) in enumerate(d.items()) if key <  0x1000 or (key >= 0xB000 and key < 0xF000)}

        # FIXME: Replace this with a loop
        list(map(lambda x: self.treestore_dictionary.insert(None, -1, [x, "", self.current_device]), rest))
                #self.current_device.canopen_dictionary))

        commun_it = self.treestore_dictionary.insert(None, -1, [0x1000, "Communication", "0x1000-0x1FFF"])
        list(map(lambda x: self.treestore_dictionary.insert(commun_it, -1, [x, "", self.current_device]), commun))

        rxpdos_it = self.treestore_dictionary.insert(commun_it, -1, [0x1600, "RxPDOs", "0x1600-0x17FF"])
        list(map(lambda x: self.treestore_dictionary.insert(rxpdos_it, -1, [x, "", self.current_device]), rxpdos))
        
        txpdos_it = self.treestore_dictionary.insert(commun_it, -1, [0x1A00, "TxPDOs", "0x1A00-0x1BFF"])
        list(map(lambda x: self.treestore_dictionary.insert(txpdos_it, -1, [x, "", self.current_device]), txpdos))

        user_it = self.treestore_dictionary.insert(None, -1, [0x2000, "Vendor-specific", "0x2000-0x5FFF"])
        list(map(lambda x: self.treestore_dictionary.insert(user_it, -1, [x, "", self.current_device]), user))
        
        profile_it = self.treestore_dictionary.insert(None, -1, [0x6000, "Profile-specific", "0x6000-0x9FFF"])
        list(map(lambda x: self.treestore_dictionary.insert(profile_it, -1, [x, "", self.current_device]), profile))
        
        diag_it = self.treestore_dictionary.insert(None, -1, [0xA000, "Diagnosis", "0xA000-0xAFFF"])
        list(map(lambda x: self.treestore_dictionary.insert(diag_it, -1, [x, "", self.current_device]), diag))
        
        device_it = self.treestore_dictionary.insert(None, -1, [0xF000, "Device", "0xF000-0xFFFF"])
        list(map(lambda x: self.treestore_dictionary.insert(device_it, -1, [x, "", self.current_device]), device))
