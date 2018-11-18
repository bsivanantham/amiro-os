AMiRo-OS support the QtCreator IDE and provides tools and manuals to setup the
according project files.

Copyright (C) 2016..2018  Thomas Schöpping

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

This file will help you to setup QtCreator projects for the operating system of
the three basic AMiRo modules and more. It also provides instructions how to
integrate the GCC ARM embedded toolchain into the IDE.

================================================================================

CONTENTS:

  1  Setting up QtCreator Projects
  2  Integration of the Toolchain
    2.1  Toolkit Setup
    2.2  Project Integration

================================================================================



1 - SETTING UP QtCreator PROJECTS
---------------------------------

To automatically generate all required files for QtCreator, run the provided
./setup.sh script and follow the instruction. You then just have to open the
*.creator files with the IDE.



2 - INTEGRATION OF THE TOOLCHAIN
--------------------------------

In order to integrate the compiler and flashing tools into your IDE, some
further actions are required.


2.1 Toolkit Setup
-----------------

Start by launching the QtCreator application, open the Tools->Options settings
and select "Build & Run". Under the tab "Compilers" add the installed
arm-none-eabi-g++ compiler as another GCC. Under the tab "Debuggers" add the
according arm-none-eabi-gdb debugger. Under the tab "Kits" you need to manually
create a new kit with the following settings:

  Name: AMiRo
  Device type: Desktop
  Sysroot: <gcc-arm-none-eabi include path>
  Compiler: <the compiler added above>
  Debugger: <the debugger added above>
  QT version: none

Keep any other fields at their default values. If there is a warning sign shown
beside the new kit, check the settings for errors.


2.2 Project Integration
-----------------------

For convenience, the absolute path to your module (e.g.
/home/juser/amiro-os/os/modules/PowerManagement) will be referenced in the
following as "MOD_PATH".

After you have sucessfully created the project files (see section 1), open the
projects by selecting the *.creator files with QtCreator. Select the "Projects"
icon at the very left and switch to the "Build & Run" tab. Change the used kit
to "AMiRo" (right above the "Build" and "Run" buttons).

Next, add a new build configuration and call it "make":

  Build directory: MOD_PATH
  Build Steps: make --directory=MOD_PATH all
  Clean Steps: make --directory=MOD_PATH clean

Optionally, you can add "-j" to the build setp in order to parallelize the
compilation process. Add yet another build configuration by clonung the "make"
configuration and name it "make flash". For this copy, replace "all" at the end
of the build step with "flash". Finally remove the "Default" configuration and
repeat these steps for any further projects.

================================================================================

