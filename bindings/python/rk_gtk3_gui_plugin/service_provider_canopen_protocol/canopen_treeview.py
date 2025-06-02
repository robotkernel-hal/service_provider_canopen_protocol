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

from builtins import range
from builtins import object
import gi
gi.require_version('Gtk', '3.0')
gi.require_version('GLib', '2.0')
from gi.repository import Gtk
from gi.repository import GObject

import helpers
from .canopen_types import *
from helpers.treestore_helpers import search, match_func
import logging

logger = logging.getLogger()


class canopen_treeview(object):
    def __init__(self):
        pass

    def create_treeview(self, view):
        self.treestore_dictionary = store = Gtk.TreeStore(int, str, GObject.TYPE_PYOBJECT, bool) # soem id type
        view = self.treeview_dictionary = view
        view.set_model(store)
        view.set_border_width(4)
        view.set_headers_visible(True)

        cell_colors = { True: self.active_color, False: "grey" }

        # ------------------ index column ------------------
        def cb_idn(column, cell, store, tree_iter):
            parent_iter = store.iter_parent(tree_iter)

            cell.set_property("xalign", 0.0)
            if parent_iter and type(store[parent_iter][2]) != str:
                cell.set_property('text', '%d'  % store[tree_iter][0])
            else:
                if type(store[tree_iter][2]) == str:
                    cell.set_property('text', store[tree_iter][2])
                else:
                    cell.set_property('text', '0x%04X'  % store[tree_iter][0])
            return True

        col_cnt = view.insert_column_with_data_func(-1, "IDn" , Gtk.CellRendererText(), cb_idn)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.GROW_ONLY)

        # ------------------ name column -------------------
        def cb_name(column, cell, store, tree_iter):
            parent_iter = store.iter_parent(tree_iter)
            if not helpers.treestore_helpers.is_row_visible(store, tree_iter, self.treeview_dictionary):
                return

            row = store[tree_iter]
            device = row[2]

            if parent_iter and type(store[parent_iter][2]) != str:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                can_object   = device.canopen_dictionary[index]
                element      = can_object.subindices[sub_index]
                element.get_data()
                cell.set_property("text", element.name)
            elif store[tree_iter][2] == None or type(store[tree_iter][2]) == str:
                cell.set_property("text", store[tree_iter][1])
            else:
                path         = store.get_path(tree_iter)
                index        = row[0]
                can_object       = device.canopen_dictionary[index]
                can_object.get_data()

                if can_object.objcode == 8 or can_object.objcode == 9:
                    sub_iter = search([row, ], match_func, ([0, 0], ))
                    if sub_iter is None:
                        self.treestore_dictionary.insert(tree_iter, -1, [0, "", device, True])

                    if self.treeview_dictionary.row_expanded(path):
                        for sub in range(1, can_object.max_subindices + 1):
                            element      = can_object.subindices[sub]
                            element.get_data()
                            if element.data_type != 0:
                                sub_iter = search([row, ], match_func, ([0, sub], ))
                                if sub_iter is None:
                                    self.treestore_dictionary.insert(tree_iter, -1, [sub, "", device, False])

                cell.set_property("text", can_object.name)
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Name", Gtk.CellRendererText(), cb_name)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_expand(True)
        column.set_resizable(True)

        def cb_datatype(column, cell, store, tree_iter):
            parent_iter = store.iter_parent(tree_iter)
            if not helpers.treestore_helpers.is_row_visible(store, tree_iter, self.treeview_dictionary):
                return

            row = store[tree_iter]
            device = row[2]

            if parent_iter and type(store[parent_iter][2]) != str:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                can_object       = device.canopen_dictionary[index]
                element      = can_object.subindices[sub_index]
                element.get_data()
                if element.data_type in canopen_datatypes:
                    cell.set_property("text", canopen_datatypes[element.data_type])
                else:
                    cell.set_property("text", "unknown 0x%X" % element.data_type)
            elif type(store[tree_iter][2]) == str:
                cell.set_property("text", "")
            else:
                path         = store.get_path(tree_iter)
                index        = row[0]
                can_object       = device.canopen_dictionary[index]
                can_object.get_data()

                if can_object.objcode == 8 or can_object.objcode == 9:
                    sub_iter = search([row, ], match_func, ([0, 0], ))
                    if sub_iter is None:
                        self.treestore_dictionary.insert(tree_iter, -1, [0, "", device, True])

                    if self.treeview_dictionary.row_expanded(path):
                        for sub in range(1, can_object.max_subindices + 1):
                            element      = can_object.subindices[sub]
                            element.get_data()
                            if element.data_type != 0:
                                sub_iter = search([row, ], match_func, ([0, sub], ))
                                if sub_iter is None:
                                    self.treestore_dictionary.insert(tree_iter, -1, [sub, "", device, False])

                if can_object.data_type in canopen_datatypes:
                    cell.set_property("text", canopen_datatypes[can_object.data_type])
                else:
                    cell.set_property("text", "unknown 0x%X" % can_object.data_type)

            return True

        col_cnt = view.insert_column_with_data_func(-1, "Datatype", Gtk.CellRendererText(), cb_datatype)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)

        def cb_objcode(column, cell, store, tree_iter):
            parent_iter = store.iter_parent(tree_iter)
            if not helpers.treestore_helpers.is_row_visible(store, tree_iter, self.treeview_dictionary):
                return

            row = store[tree_iter]
            device = row[2]

            if parent_iter and type(store[parent_iter][2]) != str:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                can_object       = device.canopen_dictionary[index]
                element      = can_object.subindices[sub_index]
                element.get_data()
                if element.objcode in canopen_objcodes:
                    cell.set_property("text", canopen_objcodes[element.objcode])
                else:
                    cell.set_property("text", "unknown 0x%X" % element.objcode)
            elif type(store[tree_iter][2]) == str:
                cell.set_property("text", "")
            else:
                path         = store.get_path(tree_iter)
                index        = row[0]
                can_object       = device.canopen_dictionary[index]
                can_object.get_data()

                if can_object.objcode == 8 or can_object.objcode == 9:
                    sub_iter = search([row, ], match_func, ([0, 0], ))
                    if sub_iter is None:
                        self.treestore_dictionary.insert(tree_iter, -1, [0, "", device, True])

                    if self.treeview_dictionary.row_expanded(path):
                        for sub in range(1, can_object.max_subindices + 1):
                            element      = can_object.subindices[sub]
                            element.get_data()
                            if element.data_type != 0:
                                sub_iter = search([row, ], match_func, ([0, sub], ))
                                if sub_iter is None:
                                    self.treestore_dictionary.insert(tree_iter, -1, [sub, "", device, False])

                if can_object.objcode in canopen_objcodes:
                    cell.set_property("text", canopen_objcodes[can_object.objcode])
                else:
                    cell.set_property("text", "unknown 0x%X" % can_object.objcode)
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Objectcode", Gtk.CellRendererText(), cb_objcode)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)
        
        def cb_objaccess(column, cell, store, tree_iter):
            def decode_access(acc):
                ret = ""
                if (acc & 0x7) == 0x7:
                    ret += "R "
                elif (acc & 0x7) == 0x3:
                    ret += "R(PO,SO) "
                elif (acc & 0x7) == 0x1:
                    ret += "R(PO) "
                elif (acc & 0x7) == 0x5:
                    ret += "R(PO,O) "
                elif (acc & 0x7) == 0x2:
                    ret += "R(SO) "
                elif (acc & 0x7) == 0x4:
                    ret += "R(O) "
                elif (acc & 0x7) == 0x6:
                    ret += "R(SO,O) "

                if (acc & 0x38) == 0x38:
                    ret += "W "
                elif (acc & 0x38) == 0x18:
                    ret += "W(PO,SO) "
                elif (acc & 0x38) == 0x8:
                    ret += "W(PO) "
                elif (acc & 0x38) == 0x28:
                    ret += "W(PO,O) "
                elif (acc & 0x38) == 0x10:
                    ret += "W(SO) "
                elif (acc & 0x38) == 0x20:
                    ret += "W(O) "
                elif (acc & 0x38) == 0x30:
                    ret += "W(SO,O) "

                if (acc & 0x40) == 0x40:
                    ret += "RxPDO "
                
                if (acc & 0x80) == 0x80:
                    ret += "TxPDO "

                if (acc & 0x100) == 0x100:
                    ret += "Backup "

                if (acc & 0x200) == 0x200:
                    ret += "Setting "

                return ret

            parent_iter = store.iter_parent(tree_iter)
            if not helpers.treestore_helpers.is_row_visible(store, tree_iter, self.treeview_dictionary):
                return

            row = store[tree_iter]
            device = row[2]

            if parent_iter and type(store[parent_iter][2]) != str:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                can_object       = device.canopen_dictionary[index]
                element      = can_object.subindices[sub_index]
                element.get_data()
                cell.set_property("text", decode_access(int(element.obj_access)))
            elif type(store[tree_iter][2]) == str:
                cell.set_property("text", "")
            else:
                path         = store.get_path(tree_iter)
                index        = row[0]
                can_object       = device.canopen_dictionary[index]
                can_object.get_data()

                if can_object.objcode == 8 or can_object.objcode == 9:
                    sub_iter = search([row, ], match_func, ([0, 0], ))
                    if sub_iter is None:
                        self.treestore_dictionary.insert(tree_iter, -1, [0, "", device, True])

                    if self.treeview_dictionary.row_expanded(path):
                        for sub in range(1, can_object.max_subindices + 1):
                            element      = can_object.subindices[sub]
                            element.get_data()
                            if element.data_type != 0:
                                sub_iter = search([row, ], match_func, ([0, sub], ))
                                if sub_iter is None:
                                    self.treestore_dictionary.insert(tree_iter, -1, [sub, "", device, False])
                    cell.set_property("text", "")
                elif can_object.objcode == 7:
                    try:
                        element      = can_object.subindices[0]
                        element.get_data()
                        cell.set_property("text", decode_access(int(element.obj_access)))
                    except KeyError:
                        logger.warning("treeview.cb_data(): key 0 not found")
                        return False
                else:
                    cell.set_property("text", "")
            return True

        col_cnt = view.insert_column_with_data_func(-1, "Access", Gtk.CellRendererText(), cb_objaccess)
        column  = view.get_column(col_cnt - 1)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)

        # ------------------ data column -------------------
        def cb_data(column, cell, store, tree_iter):
            parent_iter = store.iter_parent(tree_iter)
            if not helpers.treestore_helpers.is_row_visible(store, tree_iter, self.treeview_dictionary):
                return

            row = store[tree_iter]
            device = row[2]

            if parent_iter and type(store[parent_iter][2]) != str:
                index        = store[parent_iter][0]
                sub_index    = row[0]
                can_object       = device.canopen_dictionary[index]
                element      = can_object.subindices[sub_index]
                element.get_data()
                cell.set_property("text", element.value)
                cell.set_property("foreground", cell_colors[element.valid])
            elif type(store[tree_iter][2]) == str:
                cell.set_property("text", "")
            else:
                index        = row[0]
                can_object       = device.canopen_dictionary[index]
                can_object.get_data()
                if can_object.objcode == 7:
                    try:
                        element      = can_object.subindices[0]
                    except KeyError:
                        logger.warning("treeview.cb_data(): key 0 not found")
                        return False

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
        tree_iter = store.get_iter(path)
        parent_iter = store.iter_parent(tree_iter)

        if parent_iter:
            is_variable = store[tree_iter][3]
            if is_variable:
                idx = store[tree_iter][0]
                device = store[tree_iter][2]
                device.write_element(idx, 0, newvalue)
            else:
                idx = store[parent_iter][0]
                device = store[parent_iter][2]
                sub_idx = store[tree_iter][0]
                device.write_element(idx, sub_idx, newvalue)
        else:
            idx = store[tree_iter][0]
            device = store[tree_iter][2]
            device.write_element(idx, 0, newvalue)

        return True
