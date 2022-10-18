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

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('GLib', '2.0')
from gi.repository import Gtk
from gi.repository import GObject

from canopen_types import *
import helpers
from helpers.treestore_helpers import search, match_func

class canopen_emergency_treeview(object):
    def __init__(self):
        pass

    def create_treeview(self, view):
        self.treestore_dictionary = store = Gtk.TreeStore(str, GObject.TYPE_PYOBJECT) # soem id type
        view = self.treeview_dictionary = view
        view.set_model(store)
        view.set_border_width(4)
        view.set_headers_visible(True)

        cell_colors = { True: self.active_color, False: "grey" }

        # ------------------ index column ------------------
        def cb_idn(column, cell, store, iter):
            cell.set_property("xalign", 1.0)
            if store.iter_parent(iter):
                cell.set_property('text', '%d'  % store[iter][0])
            else:
                cell.set_property('text', '0x%04X'  % store[iter][0])
            return True

        col_cnt = view.insert_column_with_data_func(-1, "IDn" , Gtk.CellRendererText(), cb_idn)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        # ------------------ name column -------------------
        def cb_name(column, cell, store, iter):
            parent_iter = store.iter_parent(iter)
            if not helpers.treestore_helpers.is_row_visible(store, iter, self.treeview_dictionary):
                return

            row = store[iter]
            device = row[2]

            if parent_iter:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                object       = device.canopen_dictionary[index]
                element      = object.subindices[sub_index]
                element.get_data()
                cell.set_property("text", element.name)
            else:
                index        = row[0]
                object       = device.canopen_dictionary[index]
                object.get_data()

                if object.objcode == 8 or object.objcode == 9:
                    for sub in range(0, object.max_subindices + 1):
                        sub_iter = search([row, ], match_func, ([0, sub], ))
                        if sub_iter is None:
                            self.treestore_dictionary.insert(iter, -1, [sub, "", device])

                cell.set_property("text", object.name)
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Name", Gtk.CellRendererText(), cb_name)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_expand(True)
        column.set_resizable(True)

        def cb_datatype(column, cell, store, iter):
            parent_iter = store.iter_parent(iter)
            if not helpers.treestore_helpers.is_row_visible(store, iter, self.treeview_dictionary):
                return

            row = store[iter]
            device = row[2]

            if parent_iter:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                object       = device.canopen_dictionary[index]
                element      = object.subindices[sub_index]
                element.get_data()
                if element.data_type in canopen_datatypes:
                    cell.set_property("text", canopen_datatypes[element.data_type])
                else:
                    cell.set_property("text", "unknown 0x%X" % element.data_type)
            else:
                index        = row[0]
                object       = device.canopen_dictionary[index]
                object.get_data()

                if object.objcode == 8 or object.objcode == 9:
                    for sub in range(0, object.max_subindices + 1):
                        sub_iter = search([row, ], match_func, ([0, sub], ))
                        if sub_iter is None:
                            self.treestore_dictionary.insert(iter, -1, [sub, "", device])

                if object.data_type in canopen_datatypes:
                    cell.set_property("text", canopen_datatypes[object.data_type])
                else:
                    cell.set_property("text", "unknown 0x%X" % object.data_type)

            return True

        col_cnt = view.insert_column_with_data_func(-1, "Datatype", Gtk.CellRendererText(), cb_datatype)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)

        def cb_objcode(column, cell, store, iter):
            parent_iter = store.iter_parent(iter)
            if not helpers.treestore_helpers.is_row_visible(store, iter, self.treeview_dictionary):
                return

            row = store[iter]
            device = row[2]

            if parent_iter:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                object       = device.canopen_dictionary[index]
                element      = object.subindices[sub_index]
                element.get_data()
                if element.objcode in canopen_objcodes:
                    cell.set_property("text", canopen_objcodes[element.objcode])
                else:
                    cell.set_property("text", "unknown 0x%X" % element.objcode)
            else:
                index        = row[0]
                object       = device.canopen_dictionary[index]
                object.get_data()

                if object.objcode == 8 or object.objcode == 9:
                    for sub in range(0, object.max_subindices + 1):
                        sub_iter = search([row, ], match_func, ([0, sub], ))
                        if sub_iter is None:
                            self.treestore_dictionary.insert(iter, -1, [sub, "", device])

                if object.objcode in canopen_objcodes:
                    cell.set_property("text", canopen_objcodes[object.objcode])
                else:
                    cell.set_property("text", "unknown 0x%X" % object.objcode)
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Object code", Gtk.CellRendererText(), cb_objcode)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)

        # ------------------ data column -------------------
        def cb_data(column, cell, store, iter):
            parent_iter = store.iter_parent(iter)
            if not helpers.treestore_helpers.is_row_visible(store, iter, self.treeview_dictionary):
                return

            row = store[iter]
            device = row[2]

            if parent_iter:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                object       = device.canopen_dictionary[index]
                element      = object.subindices[sub_index]
                element.get_data()
                cell.set_property("text", element.value)
                cell.set_property("foreground", cell_colors[element.valid])
            else:
                index        = row[0]
                object       = device.canopen_dictionary[index]
                object.get_data()
                if object.objcode == 7:
                    element      = object.subindices[0]
                    element.get_data()
                    cell.set_property("text", element.value)
                else:
                    cell.set_property("text", "")

            return True

        cell_renderer = Gtk.CellRendererText()
        cell_renderer.set_property("editable", True)
        cell_renderer.connect("edited", self.edit_canopen_value)

        col_cnt = view.insert_column_with_data_func(-1, "Value", cell_renderer, cb_data)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)
        column.set_expand(True)
        column.set_resizable(True)
        store.set_sort_column_id(0, 0)

        return (view, store)

    def edit_canopen_value(self, cr, path, newvalue):
        store = self.treestore_dictionary
        iter = store.get_iter(path)
        parent_iter = store.iter_parent(iter)

        if parent_iter:
            idx = store[parent_iter][0]
            device = store[parent_iter][2]
            sub_idx = store[iter][0]
            device.write_element(idx, sub_idx, newvalue)
        else:
            idx = store[iter][0]
            device = store[iter][2]
            device.write_element(idx, 0, newvalue)

        return True
