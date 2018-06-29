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
FLASH_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

include $(FLASH_DIR)../../bootloader/bootloader.mk

# the files to flash
ifdef PROJECT
  FLASH_MODULES = $(PROJECT)
  FLASH_FILES = build/$(PROJECT).$(FLASHTOOL_EXT)
else
  FLASH_MODULES = $(MODULES)
  FLASH_FILES = $(foreach module,$(MODULES),$(module)/build/$(module).$(FLASHTOOL_EXT))
endif

flash: $(FLASH_FILES)
	$(info )
ifeq ($(FLASHTOOL),SerialBoot)
	$(info Flashing ($(FLASHTOOL)):)
	$(FLASHTOOL_CMD) $(FLASHTOOL_ARGS)
else
	$(info ERROR: unable to flash the module (SerialBoot unavailable))
endif

