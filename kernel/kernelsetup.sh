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

################################################################################
# GENERIC FUNCTIONS                                                            #
################################################################################

### print an error message #####################################################
# Prints a error <message> to standard output.
#If variable 'LOG_FILE' is specified, the message is also appended to the given file.
#
# usage:      printError <message>
# arguments:  <message>
#                 Message string to print.
# return:     n/a
#
function printError {
  local string="ERROR:   $1"
  # if a log file is specified
  if [ -n "$LOG_FILE" ]; then
    printf "[$(date '+%Y-%m-%d %H:%M:%S')] $string" >> $LOG_FILE
  fi
  printf "$(tput setaf 1)>>> $string$(tput sgr 0)" 1>&2
}

### print a warning message ####################################################
# Prints a warning <message> to standard output.
#If variable 'LOG_FILE' is specified, the message is also appended to the given file.
#
# usage:      printMessage <message>
# arguments:  <message>
#                 Message string to print.
# return:     n/a
#
function printWarning {
  local string="WARNING: $1"
  # if a log file is specified
  if [ -n "$LOG_FILE" ]; then
    printf "[$(date '+%Y-%m-%d %H:%M:%S')] $string" >> $LOG_FILE
  fi
  printf "$(tput setaf 3)>>> $string$(tput sgr 0)"
}

### print an information message ###############################################
# Prints an information <message> to standard output.
#If variable 'LOG_FILE' is specified, the message is also appended to the given file.
#
# usage:      printInfo <message>
# arguments:  <message>
#                 Message string to print.
# return:     n/a
#
function printInfo {
  local string="INFO:    $1"
  # if a log file is specified
  if [ -n "$LOG_FILE" ]; then
    printf "[$(date '+%Y-%m-%d %H:%M:%S')] $string" >> $LOG_FILE
  fi
  printf "$(tput setaf 2)>>> $string$(tput sgr 0)"
}

### print a message to file ####################################################
# Appends a <message> to a log file, specified by the variable 'LOG_FILE'.
#
# usage       printLog <message>
# arguments:  <message>
#                 Message string to print.
# return:     n/a
#
function printLog {
  local string="LOG:     $1"
  # if a log file is specified
  if [ -n "$LOG_FILE" ]; then
    printf "[$(date '+%Y-%m-%d %H:%M:%S')] $string" >> $LOG_FILE
  fi
}

### exit the script normally ###################################################
# Prints a delimiter and exits the script normally (returns 0).
#
# usage:      quitScript
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#
function quitScript {
  printInfo "exiting $(realpath ${BASH_SOURCE[0]})\n"
  printf "\n"
  printf "######################################################################\n"
  exit 0
}

### read a user input ##########################################################
# Reads a single character user input from a set up <options> and stores it in
# a given <return> variable.
#
# usage:      readUserInput <options> <return>
# arguments:  <options>
#                 String definiing the set of valid characters.
#                 If the string is empty, the user can input any character.
#             <return>
#                 Variable to store the selected character to.
# return:     n/a
#
function readUserInput {
  local input=""
  # read user input
  while [ -z $input ] || ( [ -n "$1" ] && [[ ! $input =~ ^[$1]$ ]] ); do
    read -p "your selection: " -n 1 -e input
    if [ -z $input ] || ( [ -n "$1" ] && [[ ! $input =~ ^[$1]$ ]] ); then
      printWarning "[$input] is no valid action\n"
    fi
  done
  printLog "[$input] has been selected\n"
  eval $2="$input"
}

### check whether argument is an option ########################################
# Checks a <string> whether it is an option.
# Options are defined to either start with '--' followed by any string, or
# to start with a single '-' followed by a single character, or
# to start with a single '-' followed by a single character, a '=' and any string.
# Examples: '--option', '--option=arg', '-o', '-o=arg', '--'
#
# usage:      parseIsOption <string>
# arguments:  <string>
#                 A string to check whether it is an option.
# return:     0
#                 <string> is an option.
#             -1
#                 <string> is not an option.
#
function parseIsOption {
  if [[ "$1" =~ ^-(.$|.=.*) ]] || [[ "$1" =~ ^--.* ]]; then
    return 0
  else
    return -1
  fi
}

### set the log file ###########################################################
# Sets a specified <infile> as log file and checks whether it already exists.
# If so, the log may either be appended to the file, its content can be cleared,
# or no log is generated at all.
# The resulting path is stored in <outvar>.
#
# usage:      setLogFile [--option=<option>] [--quiet] <infile> <outvar>
# arguments:  --option=<option>
#                 Select what to do if <file> already exists.
#                 Possible values are 'a', 'c', 'r' and 'n'.
#                 - a: append (starts with a separator)
#                 - c: continue (does not insert a seperator)
#                 - r: delete and restart
#                 - n: no log
#                 If no option is secified but <file> exists, an interactive selection is provided.
#             --quiet
#                 Suppress all messages.
#             <infile>
#                 Path of the wanted log file.
#             <outvar>
#                 Variable to store the path of the log file to.
# return:     0
#                 No error or warning occurred.
#             -1
#                 Error: invalid input
#
function setLogFile {
  local filepath=""
  local option=""
  local quiet=false

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -o=*|--option=*)
          option=${1#*=}; shift 1;;
        -o*|--option*)
          option="$2"; shift 2;;
        -q|--quiet)
          quiet=true; shift 1;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done
  filepath=$(realpath ${otherargs[0]})

  # if file already exists
  if [ -e $filepath ]; then
    # if no option was specified, ask what to do
    if [ -z "$option" ]; then
      printWarning "log file $filepath already esists\n"
      local userinput=""
      printf "Select what to do:\n"
      printf "  [A] - append log\n"
      printf "  [R] - restart log (delete existing file)\n"
      printf "  [N] - no log\n"
      readUserInput "AaRrNn" userinput
      option=${userinput,,}
    fi
    # evaluate option
    case "$option" in
      a|c)
        if [ $quiet = false ]; then
          printInfo "appending log to $filepath\n"
        fi
        if [ $option != c ]; then
          printf "\n" >> $filepath
          printf "######################################################################\n" >> $filepath
          printf "\n" >> $filepath
        fi
        ;;
      r)
        echo -n "" > $filepath
        if [ $quiet = false ]; then
          printInfo "content of $filepath wiped\n"
        fi
        ;;
      n)
        if [ $quiet = false ]; then
          printInfo "no log file will be generated\n"
        fi
        filepath=""
        ;;
      *) # sanity check (return error)
        printError "unexpected argument: $option\n"; return -1;;
    esac
  else
    if [ $quiet = false ]; then
      printInfo "log file set to $filepath\n"
    fi
  fi

  eval ${otherargs[1]}="$filepath"

  return 0
}

################################################################################
# SPECIFIC FUNCTIONS                                                           #
################################################################################

### print welcome text #########################################################
# Prints a welcome message to standard out.
#
# usage:      printWelcomeText
# arguments:  n/a
# return:     n/a
#
function printWelcomeText {
  printf "######################################################################\n"
  printf "#                                                                    #\n"
  printf "#              Welcome to the ChibiOS submodule setup!               #\n"
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
}

### print help #################################################################
# Prints a help text to standard out.
#
# usage:      printHelp
# arguments:  n/a
# return:     n/a
#
function printHelp {
  printInfo "printing help text\n"
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [-i|--init] [-p|--patch] [-d|--documentation=<option>] [-w|--wipe] [-q|--quit] [--log=<file>]\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          -i, --init\n"
  printf "              Initialize ChibiOS submodule.\n"
  printf "          -p, --patch,\n"
  printf "              Apply patches to CHibiOS.\n"
  printf "          -d, --documentation <option>\n"
  printf "              Possible options are: 'g' and 'o' (can be combined).\n"
  printf "              - g: Generate HTML documentation of ChibiOS.\n"
  printf "              - o: Open HTML documentation of ChibiOS.\n"
  printf "          -w, --wipe\n"
  printf "              Wipe ChibiOS submodule.\n"
  printf "          -q, --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file>\n"
  printf "              Specify a log file.\n"
}

### initialize ChibiOS submodule ###############################################
# Initializes the ChibiOS Git submodule.
#
# usage:      initChibiOS
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: Aborted by user.
#             -1
#                 Error: Unexpected user input.
#
function initChibiOS {
  printInfo "initializing ChibiOS submodule...\n"
  local userdir=$(pwd)
  local kerneldir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local chibiosdir=${kerneldir}/ChibiOS

  # if the kernel folder is not empty
  if [ ! -z "$(ls -A $chibiosdir)" ]; then
    printWarning "$chibiosdir is not empty. Delete and reinitialize? [y/n]\n"
    local userinput=""
    readUserInput "YyNn" userinput
    case "$userinput" in
      Y|y)
        wipeChibiOS
        ;;
      N|n)
        printWarning "ChibiOS initialization aborted by user\n"
        return 1
        ;;
      *) # sanity check (return error)
        printError "unexpected input: $userinput\n"; return -1;;
    esac
  fi

  # initialize submodule to default branch
  cd $kerneldir
  git submodule update --init $chibiosdir 2>&1 | tee -a $LOG_FILE
  while [ ${PIPESTATUS[0]} -ne 0 ]; do
    printWarning "initialitaion failed. Retry? [y/n]\n"
    local userinput=""
    readUserInput "YyNn" userinput
    case "$userinput" in
      Y|y)
        git submodule update --init $chibiosdir 2>&1 | tee -a $LOG_FILE;;
      N|n)
        printWarning "ChibiOS initialization aborted by user\n"
        cd $userdir
        return 1
        ;;
      *) # sanity check (return error)
        printError "unexpected input: $userinput\n"; return -1;;
    esac
  done
  cd $userdir

  return 0
}

### patch ChibiOS ##############################################################
# Applies patches to ChibiOS submodule.
#
# usage:      patchChibiOS
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: ChibiOS not initialized yet.
#             2
#                 Warning: Setup aborted by user.
#             -1
#                 Error: Unexpected user input.
#
function patchChibiOS {
  printInfo "applying patches to ChibiOS\n"
  local userdir=$(pwd)
  local kerneldir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local chibiosdir=${kerneldir}/ChibiOS
  local git_branch_patched="AMiRo-OS"

  # if the ChibiOS folder is empty
  if [ -z "$(ls -A $chibiosdir)" ]; then
    printWarning "$chibiosdir is empty. Please initialize first.\n"
    return 1
  else 
    # get some information from Git
    cd $chibiosdir
    local git_branch_current=$(git rev-parse --abbrev-ref HEAD)
    local git_branches=$(git for-each-ref --format="%(refname)")
    local git_dirtyfiles=($(git ls-files -dmo --exclude-standard --exclude=/doc))
    cd $userdir

    local issues=0
    # if the current branch is already $git_branch_patched
    if [ "$git_branch_current" = "$git_branch_patched" ]; then
      issues=$((issues + 1))
      printWarning "current branch is already $git_branch_patched\n"
    fi
    # if the current branch is bot $git_branch_patched, but another branch $git_branch_patched already exists
    if [ "$git_branch_current" != "$git_branch_patched" ] && [[ "$git_branches" = *"$git_branch_patched"* ]]; then
      issues=$((issues + 1))
      printWarning "another branch $git_branch_patched already exists\n"
    fi
    # if there are untracked, modified, or deleted files
    if [ ${#git_dirtyfiles[@]} != 0 ]; then
      issues=$((issues + 1))
      printWarning "there are ${#git_dirtyfiles[@]} untracked, modified, or deleted files\n"
    fi
    if [ $issues -gt 0 ]; then
      local userinput=""
      printWarning "$issues issues detected. Do you want to continue? [y/n]\n"
      readUserInput "YyNn" userinput
      case "$userinput" in
        Y|y)
          ;;
        N|n)
          printfWarning "ChibiOS patching aborted by user\n"
          return 2
          ;;
        *) # sanity check (return error)
          printError "unexpected input: $userinput\n"; return -1;;
      esac
    fi

    # create a new branch and apply the patches
    local patches=${kerneldir}/patches/*.patch
    cd $chibiosdir
    git checkout -b "$git_branch_patched" 2>&1 | tee -a $LOG_FILE
    for patch in $patches; do
      cp $patch .
      patch=$(basename $patch)
      git apply --whitespace=nowarn --ignore-space-change --ignore-whitespace < $patch 2>&1 | tee -a $LOG_FILE
      rm $patch
#      # These lines are disabled for safety reasons:
#      #   Filed commits are detected as valid changes by the super-project.
#      #   This may lead to errorneous updates of the super-project, as to point to one of these commit hashes.
#      #   Since these commits are not pushed upstream, initialization of the super-project will therefore fail, because
#      #   the referenced hashed (after patching) do not exist in a clean copy of this sub-project.
#      git add $(git ls-files -dmo --exclude-standard --exclude=/doc) $(git diff --name-only) 2>&1 | tee -a $LOG_FILE
#      git commit --message="$patch applied" 2>&1 | tee -a $LOG_FILE
    done
    cd $userdir

    return 0
  fi
}

### ChibiOS dcoumentation setup ################################################
#
# usage:      documentation [<option>]
# arguments:  <option>
#                 Can be either 'g' or 'o' to generate or open HTML documentation respectively.
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: Kernel not nitialized yet.
#             2
#                 Warning: Setup aborted by user.
#             3
#                 Warning: Issues occurred.
#             -1
#                 Error: Unexpected user input.
#
function documentation {
  local userdir=$(pwd)
  local kerneldir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local chibiosdir=${kerneldir}/ChibiOS

  # if the ChibiOS folder is empty
  if [ -z "$(ls -A $chibiosdir)" ]; then
    printWarning "$chibiosdir is empty. Please initialize first.\n"
    return 1
  else
    local option="";
    # if no argument was specified, ask what to do
    if [ $# -eq 0 ]; then
      printInfo "ChibiOS documentation setup\n"
      printf "Please select one of the following actions:\n"
      printf "  [G] - generate HTML documentation\n"
      printf "  [O] - open HTML documentation\n"
      printf "  [A] - abort this setup\n"
      local userinput
      readUserInput "GgOoAa" userinput
      option=${userinput,,}
      if [ $option = "a" ]; then
        printInfo "ChibiOS documentation setup aborted by user\n"
        return 2
      fi
    else
      option="$1"
    fi

    local issues=0
    case "$option" in
      # generate HTML documentation
      g)
        # ChibiOS/HAL: check if required files exis
        if [ -f ${chibiosdir}/doc/hal/makehtml.sh ]; then
          printInfo "generating ChibiOS/HAL documentation...\n"
          cd ${chibiosdir}/doc/hal
          ${chibiosdir}/doc/hal/makehtml.sh 2>&1 | tee -a $LOG_FILE
          cd $userdir
          printInfo "access ChibiOS/HAL documentation via ${chibiosdir}doc/hal/html/index.html\n"
        else
          issues=$((issues + 1))
          printError "could not generate ChibiOS/HAL documentation\n"
        fi
        # ChibiOS/RT: check if required files exis
        if [ -f ${chibiosdir}/doc/rt/makehtml.sh ]; then
          printInfo "generating ChibiOS/RT documentation...\n"
          cd ${chibiosdir}/doc/rt
          ${chibiosdir}/doc/rt/makehtml.sh 2>&1 | tee -a $LOG_FILE
          cd $userdir
          printInfo "access ChibiOS/RT documentation via ${chibiosdir}doc/rt/html/index.html\n"
        else
          issues=$((issues + 1))
          printError "could not generate ChibiOS/RT documentation\n"
        fi
        # ChibiOS/NIL: check if required files exis
        if [ -f ${chibiosdir}/doc/nil/makehtml.sh ]; then
          printInfo "generating ChibiOS/NIL documentation...\n"
          cd ${chibiosdir}/doc/nil
          ${chibiosdir}/doc/nil/makehtml.sh 2>&1 | tee -a $LOG_FILE
          cd $userdir
          printInfo "access ChibiOS/NIL documentation via ${chibiosdir}edoc/nil/html/index.html\n"
        else
          issues=$((issues + 1))
          printError "could not generate ChibiOS/NIL documentation\n"
        fi
        ;;

      # open HTML documentation
      o)
        # ChibiOS/HAL: check if required files exis
        if [ -f ${chibiosdir}/doc/hal/html/index.html ]; then
          printInfo "open ChibiOS/HAL documentation\n"
          xdg-open ${chibiosdir}/doc/hal/html/index.html &> /dev/null &
        else
          issues=$((issues + 1))
          printError "could not open ChibiOS/HAL documentation\n"
        fi
        # ChibiOS/RT: check if required files exis
        if [ -f ${chibiosdir}/doc/rt/html/index.html ]; then
          printInfo "open ChibiOS/RT documentation\n"
          xdg-open ${chibiosdir}/doc/rt/html/index.html &> /dev/null &
        else
          issues=$((issues + 1))
          printError "could not open ChibiOS/RT documentation\n"
        fi
        # ChibiOS/NIL: check if required files exis
        if [ -f ${chibiosdir}/doc/nil/html/index.html ]; then
          printInfo "open ChibiOS/NIL documentation\n"
          xdg-open ${chibiosdir}/doc/nil/html/index.html &> /dev/null &
        else
          issues=$((issues + 1))
          printError "could not open ChibiOS/NIL documentation\n"
        fi
        ;;

      *) # sanity check (return error)
        printError "unexpected input: $userinput\n"; return -1;;
    esac

    if [ $issues -gt 0 ]; then
      return 3
    else
      return 0
    fi
  fi
}

### reset ChibiOS submodule and wipe directory #################################
# Resets the ChibiOS Git submodule and wipes the directory.
#
# usage:      wipeChibiOS
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: Submodule directory is already empty.
#             2
#                 Warning: Wiping aborted by user.
#             -1
#                 Error: Unexpected user input.
#
function wipeChibiOS {
  printInfo "reset and wipe Git submodule $kerneldir\n"
  local userdir=$(pwd)
  local kerneldir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local chibiosdir=${kerneldir}/ChibiOS
  local git_branch_patched="AMiRo-OS"

  # if the ChibiOS folder is empty
  if [ -z "$(ls -A $chibiosdir)" ]; then
    printInfo "$chibiosdir is alread empty\n"
    return 1
  else 
    # get some information from Git
    cd $kerneldir
    local git_basehash=($(git ls-tree -d HEAD $kerneldir)); git_basehash=${git_basehash[2]}
    cd $chibiosdir
    local git_branch_current=$(git rev-parse --abbrev-ref HEAD)
    local git_difftobase="$(git diff ${git_basehash}..HEAD)"
    local git_commits=$(git log --format=oneline ${git_basehash}..HEAD)
    local git_dirtyfiles=($(git ls-files -dmo --exclude-standard --exclude=/doc))
    cd $userdir
    local issues=0
    # if the HEAD is neither detached, nor is the current branch $git_branch_patched
    if [ "$git_branch_current" != "HEAD" ] && [ "$git_branch_current" != "$git_branch_patched" ]; then
      issues=$((issues + 1))
      printWarning "modifications to ChibiOS Git submodule detected\n"
    fi
    # if HEAD is ahead of submodule base commit but with more than just applied patches
    if [ -n "$git_difftobase" ] && [ -n "$(echo $git_commits | grep -Ev '\.patch applied$')" ]; then
      issues=$((issues + 1))
      printWarning "HEAD is ahead of submodule base by unexpected commits\n"
    fi
    # if there are untracked, modified, or deleted files
    if [ ${#git_dirtyfiles[@]} != 0 ]; then
      issues=$((issues + 1))
      printWarning "there are ${#git_dirtyfiles[@]} untracked, modified, or deleted files\n"
    fi
    if [ $issues -gt 0 ]; then
      local userinput=""
      printWarning "$issues issues detected. Do you want to continue? [y/n]\n"
      readUserInput "YyNn" userinput
      case "$userinput" in
        Y|y)
          ;;
        N|n)
          printfWarning "wiping ChibiOS Git submodule aborted by user\n"
          return 2
          ;;
        *) # sanity check (return error)
          printError "unexpected input: $userinput\n"; return -1;;
      esac
    fi

    # checkout base commit and delete all local branches
    cd $kerneldir
    git submodule update --force --checkout $kerneldir | tee -a $LOG_FILE
    cd $chibiosdir
    local git_branches=($(git for-each-ref --format="%(refname)"))
    for branch in $git_branches; do
      if [[ $branch = *"heads/"* ]]; then
        git branch -D ${branch##*/} | tee -a $LOG_FILE
      fi
    done
    cd $userdir

    # deinitialize ChibiOS submodule and delete any remaining files
    cd $kerneldir
    git submodule deinit -f $chibiosdir 2>&1 | tee -a $LOG_FILE
    rm -rf $chibiosdir/*
    cd $userdir

    return 0
  fi
}

### main function of this script ###############################################
# The kernel setup provides comfortable initialization, patching, documentation
# generation and cleanup for ChibiOS.
#
# usage:      see function printHelp
# arguments:  see function printHelp
# return:     0
#                 No error or warning occurred.
#
function main {
  # print welcome/info text if not suppressed
  if [[ $@ != *"--noinfo"* ]]; then
    printWelcomeText
  else
    printf "######################################################################\n"
  fi
  printf "\n"

  # if --help or -h was specified, print the help text and exit
  if [[ $@ == *"--help"* || $@ == *"-h"* ]]; then
    printHelp
    printf "\n"
    quitScript
  fi

  # set log file if specified
  if [[ $@ == *"--log"* ]] || [[ $@ == *"--LOG"* ]]; then
    # get the parameter (file name)
    local cmdidx=1
    while [[ ! "${!cmdidx}" = "--log"* ]] && [[ ! "${!cmdidx}" = "--LOG"* ]]; do
      cmdidx=$[cmdidx + 1]
    done
    local cmd="${!cmdidx}"
    local logfile=""
    if [[ "$cmd" = "--log="* ]] || [[ "$cmd" = "--LOG="* ]]; then
      logfile=${cmd#*=}
    else
      local filenameidx=$((cmdidx + 1))
      logfile="${!filenameidx}"
    fi
    # optionally force silent appending
    if [[ "$cmd" = "--LOG"* ]]; then
      setLogFile --option=c --quiet "$logfile" LOG_FILE
    else
      setLogFile "$logfile" LOG_FILE
      printf "\n"
    fi
  fi
  # log script name
  printLog "this is $(realpath ${BASH_SOURCE[0]})\n"

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -h|--help) # already handled; ignore
          shift 1;;
        -i|--init)
           initChibiOS; printf "\n"; shift 1;;
        -p|--patch)
           patchChibiOS; printf "\n"; shift 1;;
        -d=*|--documentation=*)
           documentation "${1#*=}"; printf "\n"; shift 1;;
        -d|--documentation)
           if ( ! parseIsOption $2 ); then
             documentation "$2"; printf "\n"; shift 2
           else
             documentation; printf "\n"; shift 1
           fi;;
        -w|--wipe)
          wipeChibiOS; printf "\n"; shift 1;;
        -q|--quit)
          quitScript; shift 1;;
        --log=*|--LOG=*) # already handled; ignore
          shift 1;;
        --log|--LOG) # already handled; ignore
          shift 2;;
        --noinfo) # already handled; ignore
          shift 1;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # interactive menu
  while ( true ); do
    # main menu info prompt and selection
    printInfo "ChibiOS kernel setup main menu\n"
    printf "Please select one of the following actions:\n"
    printf "  [I] - initialize ChibiOS submodule\n"
    printf "  [P] - apply patches to ChibiOS\n"
    printf "  [D] - generate or open HTML documentation\n"
    printf "  [W] - wipe ChibiOS submodule\n"
    printf "  [Q] - quit this setup\n"
    local userinput=""
    readUserInput "IiPpDdWwQq" userinput
    printf "\n"

    # evaluate user selection
    case "$userinput" in
      I|i)
        initChibiOS; printf "\n";;
      P|p)
        patchChibiOS; printf "\n";;
      D|d)
        documentation; printf "\n";;
      W|w)
        wipeChibiOS; printf "\n";;
      Q|q)
        quitScript;;
      *) # sanity check (exit with error)
        printError "unexpected argument: $userinput\n";;
    esac
  done

  exit 0
}

################################################################################
# SCRIPT ENTRY POINT                                                           #
################################################################################

main "$@"
