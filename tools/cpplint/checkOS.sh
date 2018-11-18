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

#!/bin/bash

# initial info text
printf "######################################################################\n"
printf "#                                                                    #\n"
printf "#              Welcome to the AMiRo-OS CppLint checker               #\n"
printf "#                                                                    #\n"
printf "######################################################################\n"
printf "#                                                                    #\n"
printf "# Copyright (c) 2016..2018  Thomas Schöpping                         #\n"
printf "#                                                                    #\n"
printf "# This is free software; see the source for copying conditions.      #\n"
printf "# There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR  #\n"
printf "# A PARTICULAR PURPOSE. The development of this software was         #\n"
printf "# supported by the Excellence Cluster EXC 227 Cognitive Interaction  #\n"
printf "# Technology. The Excellence Cluster EXC 227 is a grant of the       #\n"
printf "# Deutsche Forschungsgemeinschaft (DFG) in the context of the German #\n"
printf "# Excellence Initiative.                                             #\n"
printf "#                                                                    #\n"
printf "######################################################################\n"
printf "\n"

# print note
printf "\n"
printf "NOTE: This tool is rather a preview and was not optimized nor tested!\n"
printf "      Expect bugs and other issues.\n"
printf "\n"

# check the OS
ROOT_PATH="../../"
read -p "root path:   " -i "$ROOT_PATH" -e ROOT_PATH
read -p "output file: " -e OUTPUT_FILE

if [ -z "$OUTPUT_FILE" ]; then
  OUTPUT=""
else
  OUTPUT="2> ${OUTPUT_FILE}"
fi

python/cpplint.py --linelength=120 --extensions=h,c,hpp,cpp `find ${ROOT_PATH} \( -iname "*.h" -o -iname "*-c" -o -iname "*.hpp" -o -iname "*.cpp" \) -follow -type f -print` ${OUTPUT}

unset ROOT_PATH
unset OUTPUT_FILE
unset OUTPUT
