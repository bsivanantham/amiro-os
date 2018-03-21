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



define HELP_TEXT
################################################################################
#                                                                              #
# Copyright (c) 2016..2018  Thomas Schöpping                                   #
#                                                                              #
# This is free software; see the source for copying conditions. There is NO    #
# warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  #
# The development of this software was supported by the Excellence Cluster     #
# EXC 227 Cognitive Interaction Technology. The Excellence Cluster EXC 227 is  #
# a grant of the Deutsche Forschungsgemeinschaft (DFG) in the context of the   #
# German Excellence Initiative.                                                #
#                                                                              #
################################################################################

  AMiRo-OS Makefile help
  ======================

ARGUMENTS:

  help:
      Prints this text.

  all:
      Builds the binaries for all modules.

  flash:
      Flashes all binaries to the hardware.
      If the binaries do not exist, they are created beforehand.

  clean:
      Deletes all temporary and binary files of all modules.

  <module>:
      Builds the binary only for the specified module.
      Possible values for <module> are:
        - DiWheelDrive
        - PowerManagement
        - LightRing


EXAMPLES:

  >$$ make DiWheelDrive
      This command will generate the binary file for the DiWheelDrive module.

  >$$ make DiWheelDrive LightRing
      This command will generate the binary files for the two modules
      DiWheelDrive and LightRing.

  >$$ make flash -j
      This command will first build all missing binary files and flash all
      modules as soon as the binaries are ready.
      By the additional argument '-j' the build process will be parallelized.

  >$$ make clean && make all && make flash
      This command will first clean all thee projects. In a second step the
      binaries for al three modules are build from scratch. Finally all modules
      are updated with the latest software.
      The following command can be used as a shorter and faster version:
          >$$ make clean && make flash -j

################################################################################
endef
export HELP_TEXT

OS_BASE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

help:
	@echo "$$HELP_TEXT"
	@exit

include $(OS_BASE_DIR)/os/modules/Makefile
