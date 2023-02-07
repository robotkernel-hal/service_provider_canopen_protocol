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

from builtins import object
from .canopen_object import canopen_object
import helpers
import time
import datetime, copy
import logging

logger = logging.getLogger()

class canopen_emergency(object):
    def __init__(self, timestamp, error_code, error_register, data):
        self.timestamp = timestamp
        self.error_code = error_code
        self.error_register = error_register
        self.data = copy.copy(data)

class canopen_device(helpers.svc_wrapper):
    def __init__(self, service_prefix, app, widget, modname, devname):
        logger.info("registering service {prefix}.{modname}.{devname}.canopen_protocol (service_prefix={prefix}, modname={modname}, devname={devname}".format(prefix=service_prefix,
                                                                                                                                                              modname=modname,
                                                                                                                                                              devname=devname))
        helpers.svc_wrapper.__init__(self, app.clnt,
                "{}.{}.{}.canopen_protocol".format(service_prefix, modname, devname))

        self.service_prefix = service_prefix # beware, self.prefix is used in the parent class
        self.modname = modname
        self.devname = devname
        self.widget = widget
        self.canopen_dictionary = dict()
        self.canopen_emergencies = []
        self.svc_call_pending = False

    def read_object(self, index):
        self.svc_read_object.req.index = index
        self.svc_read_object.call()
        return self.svc_read_object.resp

    # # FIXME: this is probably duplicate to the canopen_element.read() method,
    # # and is otherwise unused - it probably belongs here
    # def read_element(self, index, sub_index):
    #     self.svc_read_element.req.index = index
    #     self.svc_read_element.req.sub_index = sub_index
    #     self.svc_read_element.call()
    #     return self.svc_read_element.resp

    # FIXME: This one is used, and should perhaps moved to the
    # canopen_element class.

    def read_object(self, index, callback=None):

        #non-blocking read on data, with callback (see get_data)
        self.svc_call_pending = True
        self.svc_read_object.req.index = index
        
        def cb_read_object(_time):
            try:
                self.svc_read_object.utf8_decode_char_fields(False)
                
                data = self.svc_read_object.resp
                if callback is not None:
                    callback(data)
                    
                self.svc_call_pending = False
                self.queue_draw()
            except:
                print(traceback.format_exc())
            return False

        # old GTK2 code: set async finish handler
        #self.svc_read_object.gobject_on_async_finish(cb_read, time.time())
        # new GTK2 code
        self.svc_read_object.call_async()
        self.svc_read_object.mainloop_on_async_finish(self._mainloop, cb_read_object, time.time())


    def read_element(self, index, sub_index, callback=None):
        #non-blocking read on data, with callback (see get_data)
        # FIXME: This is duplicated code in canopen_device.read_element()
        # The code related to canopen_device should probably
        # be moved to a read_element_async() method there.
        self.svc_call_pending = True
        self.svc_read_element.req.index = index
        self.svc_read_element.req.sub_index = sub_index

        def cb_read_element(_time):
            # callback after canopen returned with data
            data = self.svc_read_element.resp
            print("cb_read(): calling callback[1]!")
            if callback is not None:
                print("cb_read(): calling callback[2]!")
                callback(data)
            self.svc_call_pending = False
            self.queue_draw()            
        
        # experimental: adapt to new LN API for GTK3
        #self.svc_read_element.gobject_on_async_finish(cb_read, time.time())

        self.svc_read_element.call_async()
        self.svc_read_element.mainloop_on_async_finish(self._mainloop, cb_read_element, time.time())

    
    def write_element(self, index, sub_index, value):
        self.svc_write_element.req.index = index
        self.svc_write_element.req.sub_index = sub_index
        self.svc_write_element.req.value = value
        self.svc_write_element.call()
        self.canopen_dictionary[index].subindices[sub_index].valid = False
        self.canopen_dictionary[index].subindices[sub_index].get_data()
        return self.svc_write_element.resp

    def pop_emergency_message(self):
        self.svc_pop_emergency_message.call()
        return self.svc_pop_emergency_message.resp

    def _evaluate_string_list(self, ret):
        return eval(ret["modules"])

    def upload_emergencies(self):
        ans = self.pop_emergency_message()
        print(ans)

        if ans.error_message_len != 0:
            return None

        ts_float = ans.timestamp_nsec * 1E-9 + ans.timestamp_sec
        ts = datetime.datetime.utcfromtimestamp(ts_float)

        return canopen_emergency(ts, ans.error_code, ans.error_register, ans.data)
        #if not ans.error_message_len

    def list_dictionary(self):
        if not (self.devname.endswith(".mailbox") or self.devname.endswith(".eeprom")):
            return []
        
        if not len(self.canopen_dictionary): #only upon first call
            logger.debug("calling service {service_prefix}.{modname}.{devname}.canopen_protocol::object_dictionary_list (service_prefix={service_prefix}, modname={modname}, devname={devname}".format(service_prefix=self.service_prefix,
                                                                                                                                                              modname=self.modname,
                                                                                                                                                              devname=self.devname))
            try:
                self.svc_object_dictionary_list.call()
                canopen_dictionary = self.svc_object_dictionary_list.resp.indices
            except helpers.ServiceNotFoundException:
                canopen_dicitonary = []
                logger.warning("no dictionary found for device {}, assuming empty dictionary".format(self.devname))
            for idn in list(canopen_dictionary):
                self.canopen_dictionary[idn] = canopen_object(self, idn)
        return list(self.canopen_dictionary.keys())

    def list_processdata(self, ids):
        for id in ids:
            yield self.canopen_ids[id].get_data()

    def queue_draw(self):
        self.widget.queue_draw()

