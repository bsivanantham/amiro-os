################################################################################
# AMiRo-OS is an operating system designed for the Autonomous Mini Robot       #
# (AMiRo) platform.                                                            #
# Copyright (C) 2016..2018  Thomas Schöpping et al.                            #
#                                                                              #
# This program is free software: you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation, either version 3 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.        #
#                                                                              #
# This research/work was supported by the Cluster of Excellence Cognitive      #
# Interaction Technology 'CITEC' (EXC 277) at Bielefeld University, which is   #
# funded by the German Research Foundation (DFG).                              #
################################################################################



# the path to this makefile
BOOTLOADER_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# the interface for the bootloader
BOOTLOADERINC = $(BOOTLOADER_DIR)AMiRo-BLT/Target/Source/AMiRo

include $(BOOTLOADER_DIR)AMiRo-BLT/Target/Modules/moduleids.mk

# some flashing parameters
ifeq ($(OS),Windows_NT)
	FLASH_PORT ?= COM4
else
	FLASH_PORT ?= /dev/ttyUSB0
endif
FLASH_BAUDRATE ?= 115200

# set the flashing tool, the command to call it, arguments, and file extensions
ifneq ($(wildcard $(BOOTLOADER_DIR)AMiRo-BLT/Host/Source/SerialBoot/build/SerialBoot),)
	# if AMiRo-BLT project is available, use SerialBoot
	FLASHTOOL = SerialBoot
	FLASHTOOL_CMD = $(BOOTLOADER_DIR)AMiRo-BLT/Host/Source/SerialBoot/build/SerialBoot
	FLASHTOOL_EXT = srec
	ifdef PROJECT
		FLASHTOOL_ARGS = -d$(FLASH_PORT) -b$(FLASH_BAUDRATE) $(foreach module,$(FLASH_MODULES),-T$($(module)_ID_HEX) build/$(module).$(FLASHTOOL_EXT))
	else
		FLASHTOOL_ARGS = -d$(FLASH_PORT) -b$(FLASH_BAUDRATE) $(foreach module,$(FLASH_MODULES),-T$($(module)_ID_HEX) $(FLASH_DIR)$(module)/build/$(module).$(FLASHTOOL_EXT))
	endif
else
	# the default tool to use is stm32flash
	ifneq ($(words $(FLASH_MODULES)),1)
		FLASH_MODULES := $(firstword $(FLASH_MODULES))
	endif
	FLASHTOOL = stm32flash
	FLASHTOOL_CMD = stm32flash
	FLASHTOOL_EXT = hex
	ifdef PROJECT
		FLASHTOOL_ARGS = $(FLASH_PORT) -i "rts,-dtr,-rts":"rts,dtr,-rts" -R -b $(FLASH_BAUDRATE) -w build/$(FLASH_MODULES).$(FLASHTOOL_EXT)
	else
		FLASHTOOL_ARGS = $(FLASH_PORT) -i "rts,-dtr,-rts":"rts,dtr,-rts" -R -b $(FLASH_BAUDRATE) -w $(FLASH_MODULES)/build/$(FLASH_MODULES).$(FLASHTOOL_EXT)
	endif
endif

