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



################################################################################
# Build global options                                                         #
# NOTE: Can be overridden externally.                                          #
#                                                                              #

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O2 -fomit-frame-pointer -falign-functions=16 -fstack-usage
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = -std=c11
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -fno-rtti -std=c++17
endif

# Enable this if you want the linker to remove unused code and data
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT =
endif

# Enable this if you want link time optimizations (LTO)
ifeq ($(USE_LTO),)
  USE_LTO = yes
endif

# If enabled, this option allows to compile the application in THUMB mode.
ifeq ($(USE_THUMB),)
  USE_THUMB = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = no
endif

#                                                                              #
# Build global options                                                         #
################################################################################

################################################################################
# Architecture or project specific options                                     #
#                                                                              #

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x400
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU on Cortex-M4.
# Possible selections are:
#   no     - no FPU is used (probably equals 'soft')
#   soft   - does not use the FPU, thus all floating point operations are emulated
#   softfp - uses the FPU, but uses the integer registers only
#   hard   - uses the FPU and passes data via the FPU registers
ifeq ($(USE_FPU),)
  USE_FPU = softfp
endif

#                                                                              #
# Architecture or project specific options                                     #
################################################################################

################################################################################
# Project, sources and paths                                                   #
#                                                                              #

# Define project name here
PROJECT := $(patsubst $(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST))))..)/%,%,$(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

# Imported source files and paths
include ../../../kernel/kernel.mk
CHIBIOS := $(AMIROOS_KERNEL)
AMIROOS = ../..
# Startup files
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk
# HAL-OSAL files
include $(CHIBIOS)/os/hal/hal.mk
include $(AMIROOS)/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F4xx/platform.mk
include $(AMIROOS)/hal/ports/STM32/platform.mk
include ./board.mk
include $(CHIBIOS)/os/hal/osal/rt/osal.mk
# RTOS files
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/port_v7m.mk
# Other files (optional).
include $(CHIBIOS)/test/rt/test.mk
# AMiRo-BLT files
include ../../../bootloader/bootloader.mk
# AMiRo-LLD files
include ../../../periphery-lld/periphery-lld.mk
# AMiRo-OS files
include $(AMIROOS)/modules/modules.mk
include $(AMIROOS)/core/core.mk
include $(AMIROOS)/unittests/unittests.mk

# Define linker script file here
LDSCRIPT= $(BOARDLD)/STM32F405xG.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC = $(STARTUPSRC) \
       $(KERNSRC) \
       $(PORTSRC) \
       $(OSALSRC) \
       $(HALSRC) \
       $(PLATFORMSRC) \
       $(BOARDSRC) \
       $(MODULESCSRC) \
       $(TESTSRC) \
       $(PERIPHERYLLDCSRC) \
       $(AMIROOSCORECSRC) \
       $(UNITTESTSCSRC) \
       $(CHIBIOS)/os/various/evtimer.c \
       $(CHIBIOS)/os/various/syscalls.c \
       $(CHIBIOS)/os/hal/lib/streams/chprintf.c \
       module.c \
       $(APPSCSRC)

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC = $(AMIROOSCORECPPSRC) \
         $(APPSCPPSRC)

# C sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACSRC =

# C++ sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACPPSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCPPSRC =

# List ASM source files here
ASMSRC =
ASMXSRC = $(STARTUPASM) \
          $(PORTASM) \
          $(OSALASM)

INCDIR = $(CHIBIOS)/os/license \
         $(STARTUPINC) \
         $(KERNINC) \
         $(PORTINC) \
         $(OSALINC) \
         $(HALINC) \
         $(PLATFORMINC) \
         $(BOARDINC) \
         $(MODULESINC) \
         $(TESTINC) \
         $(BOOTLOADERINC) \
         $(CHIBIOS)/os/hal/lib/streams \
         $(CHIBIOS)/os/various \
         $(PERIPHERYLLDINC) \
         $(AMIROOS) \
         $(AMIROOSCOREINC) \
         $(UNITTESTSINC) \
         $(APPSINC)

#                                                                              #
# Project, sources and paths                                                   #
################################################################################

################################################################################
# Compiler settings                                                            #
# NOTE: Some can be overridden externally.                                     #
#                                                                              #

MCU  = cortex-m4

#TRGT = arm-elf-
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
# Enable loading with g++ only if you need C++ runtime support.
# NOTE: You can use C++ even without C++ support if you are careful. C++
#       runtime support makes code size explode.
LD   = $(TRGT)gcc
#LD   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
AR   = $(TRGT)ar
OD   = $(TRGT)objdump
SZ   = $(TRGT)size
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary
SREC = $(CP) -O srec --srec-len=248

# ARM-specific options here
ifeq ($(AOPT),)
  AOPT =
endif

# THUMB-specific options here
ifeq ($(TOPT),)
  TOPT = -mthumb -DTHUMB
endif

# Define C warning options here
ifeq ($(CWARN),)
  CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes
endif

# Define C++ warning options here
ifeq ($(CPPWARN),)
  CPPWARN = -Wall -Wextra -Wundef
endif

#                                                                              #
# Compiler settings                                                            #
################################################################################

################################################################################
# Start of user section                                                        #
#                                                                              #

# List all user C define here, like -D_DEBUG=1
UDEFS +=

# Define ASM defines here
UADEFS +=

# List all user directories here
UINCDIR +=

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS +=

#                                                                              #
# End of user defines                                                          #
################################################################################

# allow for custom build directory
ifneq ($(BUILDDIR),)
  BUILDDIR := $(BUILDDIR)/$(PROJECT)
endif

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC
include $(RULESPATH)/rules.mk

include $(AMIROOS)/modules/flash.mk
