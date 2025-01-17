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

canopen_datatypes = {
        0x0000 : "",
        0x0001 : "Boolean",
        0x0002 : "Integer 8",
        0x0003 : "Integer 16",
        0x0004 : "Integer 32",
        0x0005 : "Unsigned 8",
        0x0006 : "Unsigned 16",
        0x0007 : "Unsigned 32",
        0x0008 : "Real 32",
        0x0009 : "Visible string",
        0x000A : "Octet string",
        0x000B : "Unicode string",
        0x000C : "Time of day",
        0x000D : "Time difference",
        0x000F : "Domain",
        0x0010 : "Integer 24",
        0x0011 : "Real 64",
        0x0012 : "Integer 40",
        0x0013 : "Integer 48",
        0x0014 : "Integer 56",
        0x0015 : "Integer 64",
        0x0016 : "Unsigned 24",
        0x0018 : "Unsigned 40",
        0x0019 : "Unsigned 48",
        0x001A : "Unsigned 56",
        0x001B : "Unsigned 64",
        0x001D : "Guid",
        0x001E : "Byte",
        0x001F : "Word",
        0x0020 : "Dword",
        0x0021 : "PDO mapping",
        0x0023 : "Identity",
        0x0025 : "Command",
        0x0027 : "PDO comm parameter",
        0x0028 : "Enum",
        0x0029 : "SM parameter",
        0x002A : "Record",
        0x002B : "Backup",
        0x002C : "Mdp",
        0x002D : "Bit-array 8",
        0x002E : "Bit-array 16",
        0x002F : "Bit-array 32",
        0x0030 : "Bit 1",
        0x0031 : "Bit 2",
        0x0032 : "Bit 3",
        0x0033 : "Bit 4",
        0x0034 : "Bit 5",
        0x0035 : "Bit 6",
        0x0036 : "Bit 7",
        0x0037 : "Bit 8",
        0x0260 : "Array of integer 16",
        0x0261 : "Array of integer 8",
        0x0262 : "Array of integer 32",
        0x0263 : "Array of integer 64",
        0x0281 : "Error handling",
        0x0282 : "Diagnosys history",
        0x0283 : "Sync status",
        0x0284 : "Sync settings",
        0x0285 : "FSoE frame",
        0x0286 : "FSoE comm parameter",
}

canopen_objcodes = {
        0x00 : "",
        0x07 : "Variable",
        0x08 : "Array",
        0x09 : "Record",
        }

