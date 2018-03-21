AMiRo-OS is an operating system for the base version of the Autonomous Mini
Robot (AMiRo) [1]. It utilizes ChibiOS (a real-time operating system for
embedded devices developed by Giovanni di Sirio; see <http://chibios.org>) as
system kernel and extends it with platform specific configurations and further
functionalities and abstractions.

Copyright (C) 2016..2018  Thomas Schöpping et al.
(a complete list of all authors is given below)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

This research/work was supported by the Cluster of Excellence
Cognitive Interaction Technology 'CITEC' (EXC 277) at Bielefeld
University, which is funded by the German Research Foundation (DFG).

Authors:
 - Thomas Schöpping          <tschoepp[at]cit-ec.uni-bielefeld.de>
 - Marc Rothmann

References:
 [1] S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A
     modular & customizable open-source mini robot platform," 2016 20th
     International Conference on System Theory, Control and Computing (ICSTCC),
     Sinaia, 2016, pp. 687-692.



################################################################################
#                                                                              #
#        RRRRRRRR   EEEEEEEE     AAA     DDDDDDDD   MM     MM  EEEEEEEE        #
#        RR     RR  EE          AA AA    DD     DD  MMM   MMM  EE              #
#        RR     RR  EE         AA   AA   DD     DD  MMMM MMMM  EE              #
#        RRRRRRRR   EEEEEE    AA     AA  DD     DD  MM MMM MM  EEEEEE          #
#        RR   RR    EE        AAAAAAAAA  DD     DD  MM     MM  EE              #
#        RR    RR   EE        AA     AA  DD     DD  MM     MM  EE              #
#        RR     RR  EEEEEEEE  AA     AA  DDDDDDDD   MM     MM  EEEEEEEE        #
#                                                                              #
################################################################################

This file will help you to setup all required software on your system, compile
the source code, and flash it to the AMiRo modules.

================================================================================

CONTENTS:

  1  Required software
    1.1  Git
    1.2  Bootloader & Tools
    1.3  System Kernel
    1.4  Low-Level Drivers
  2  Recommended software
    2.1  gtkterm and hterm
    2.2  QtCreator IDE
    2.3  Doxygen & Graphviz
  3  Building and flashing

================================================================================



1 - REQUIRED SOFTWARE
---------------------

In order to compile the source code, you need to install the GNU ARM Embedded
Toolchain. Since this project uses GNU Make for configuring and calling the
compiler, this tool is requried too. AMiRo-OS uses ChibiOS as system kernel,
so you need a copy of that project as well.


1.1 - Git
---------

Since all main- and subprojects are available as Git repositories, installing a
recent version of the tool is mandatory.


1.2 Bootloader & Tools
----------------------

AMiRo-OS can take advantage of an installed bootloader if such exists and
provides an interface. By default, AMiRo-BLT is included as a Git submodule and
can easily be initialized via the ./setup.sh script. If requried, you can
replace the used bootloader by adding an according subfolder in the ./bootloader
directory. Note that you will have to adapt the makefiles and scripts, and
probably the operating system as well.
AMiRo-BLT furthermore has its own required and recommended software tools as
described in its README.txt file. Follow th instructions to initialize the
development environment manually or use the ./setup.sh script.


1.3 System Kernel
-----------------

Since AMiRo-OS uses ChibiOS as underlying system kernel, you need to acquire a
copy of it as well. For the sake of compatibility, it is included in AMiRo-OS as
a Git submodule. It is highly recommended to use the ./setup.sh script for
initialization. Moreover, you have to apply the patches to ChibiOS in order to
make AMiRo-OS work properly. It is recommended to use the .setup.sh script for this
purpose.
If you would like to use a different kernel, you can add a subfolder in the
./kernel/ directory and adapt the scripts and operating system source code.


1.4 Low-Level Drivers
---------------------

Any required low-level drivers for the AMiRo hardware is available in an
additional project: AMiRo-LLD. It is included as a Git subodule and can be
initialized via the ./setup.sh script.



2 - RECOMMENDED SOFTWARE
------------------------

AMiRo-OS can take advanatge of an installed bootloader, which is recommended for
the best experience. In order to use all features of AMiRo-OS it is also
recommended to install either the 'hterm' or 'gtkterm' application for accessing
the robot. To ease further development, this project offers support for the
QtCreator IDE.


2.1 - gtkterm and hterm
-----------------------

Depending on your operating system it is recommended to install 'gtkterm' for
Linux (available in the Ubuntu repositories), or 'hterm' for Windows. For
gtkterm you need to modify the configuration file ~/.gtktermrc (generated
automatically when you start the application for the first time) as follows:

  port	= /dev/ttyUSB0
  speed	= 115200
  bits	= 8
  stopbits	= 1
  parity	= none
  flow	= none
  wait_delay	= 0
  wait_char	= -1
  rs485_rts_time_before_tx	= 30
  rs485_rts_time_after_tx	= 30
  echo	= False
  crlfauto	= True

For hterm you need to configure the tool analogously. With either tool the robot
can be reset by toggling the RTS signal on and off again, and you can access the
system shell of AMiRo-OS.


2.2 - QtCreator IDE
-------------------

In order to setup QtCreator projects for the three AMiRo base modules, you can
use the provided ./setup.sh script. Further instructions for a more advanced
configuration of the IDE are provided in the ./tools/qtcreator/README.txt file.


2.3  Doxygen & Graphviz
-----------------------

In order to generate the documentation from the source code, Doxygen and
Graphviz are requried. It is recommended to install these tool using the
default versions for your system. Ubuntu users should simply run
  >$ sudo apt-get install doxygen graphviz



3 - BUILDING AND FLASHING
-------------------------

Each time you modify any part of AMiRo-OS, you need to recompile the whole
project for the according AMiRo module. Therefore you can use the ./Makefile by
simply executing 'make' and follow the instructions. Alternatively, you can
either use the makefiles provided per module in ./os/modules/<ModuleToCompile>
or - if you want to compile all modules at once - the makefile in the
./os/modules folder. After the build process has finished successfully, you
always have to flash the generated program to the robot. Therefore you need an
appropriate tool, such as stm32flash (if you don't use a bootloader) or
SerialBoot (highly recommended; provided by AMiRo-BLT). Similarly to the
compilation procedure as described above, you can flash either each module
separately, or all modules at once by executing 'make flash' from the according
directory.

When using SerialBoot, please note that you must connect the programming cable
either to the DiWheelDrive or the PowerManagement module for flashing the
operating system. All other modules are powered off after reset so that only
these two offer a running bootloader, which is required for flashing.

================================================================================

