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



all: modules

include $(dir $(abspath $(lastword $(MAKEFILE_LIST))))modules.mk

CLEANTRGS = $(MODULES:%=clean_%)
FLASHTRGS = $(MODULES:%=flash_%)

.PHONY: $(MODULES)

modules: $(MODULES)
$(MODULES):
	$(MAKE) -C $(MODULES_DIR)$@

flash: $(FLASHTRGS)
$(FLASHTRGS):
	$(MAKE) -C $(MODULES_DIR)$(@:flash_%=%) flash

clean: $(CLEANTRGS)
$(CLEANTRGS):
	$(MAKE) -C $(MODULES_DIR)$(@:clean_%=%) clean
