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
        0x0001 : "BOOLEAN",
        0x0002 : "INTEGER_8",
        0x0003 : "INTEGER_16",
        0x0004 : "INTEGER_32",
        0x0005 : "UNSIGNED_8",
        0x0006 : "UNSIGNED_16",
        0x0007 : "UNSIGNED_32",
        0x0008 : "REAL_32",
        0x0009 : "VISIBLE_STRING",
        0x000A : "OCTET_STRING",
        0x000B : "UNICODE_STRING",
        0x000C : "TIME_OF_DAY",
        0x000D : "TIME_DIFFERENCE",
        0x000F : "DOMAIN",
        0x0010 : "INTEGER_24",
        0x0011 : "REAL_64",
        0x0012 : "INTEGER_40",
        0x0013 : "INTEGER_48",
        0x0014 : "INTEGER_56",
        0x0015 : "INTEGER_64",
        0x0016 : "UNSIGNED_24",
        0x0018 : "UNSIGNED_40",
        0x0019 : "UNSIGNED_48",
        0x001A : "UNSIGNED_56",
        0x001B : "UNSIGNED_64",
        0x001D : "GUID",
        0x001E : "BYTE",
        0x001F : "WORD",
        0x0020 : "DWORD",
        0x0021 : "PDOMAPPING",
        0x0023 : "IDENTITY",
        0x0025 : "COMMAND",
        0x0027 : "PDOCOMPAR",
        0x0028 : "ENUM",
        0x0029 : "SMPAR",
        0x002A : "RECORD",
        0x002B : "BACKUP",
        0x002C : "MDP",
        0x002D : "BITARR8",
        0x002E : "BITARR16",
        0x002F : "BITARR32",
        0x0030 : "BIT_1",
        0x0031 : "BIT_2",
        0x0032 : "BIT_3",
        0x0033 : "BIT_4",
        0x0034 : "BIT_5",
        0x0035 : "BIT_6",
        0x0036 : "BIT_7",
        0x0037 : "BIT_8",
        0x0260 : "ARRAY_OF_INT",
        0x0261 : "ARRAY_OF_SINT",
        0x0262 : "ARRAY_OF_DINT",
        0x0263 : "ARRAY_OF_UDINT",
        0x0281 : "ERRORHANDLING",
        0x0282 : "DIAGHISTORY",
        0x0283 : "SYNCSTATUS",
        0x0284 : "SYNCSETTINGS",
        0x0285 : "FSOEFRAME",
        0x0286 : "FSOECOMMPAR",
}

canopen_objcodes = {
        0x00 : "",
        0x07 : "VARIABLE",
        0x08 : "ARRAY",
        0x09 : "RECORD",
        }

