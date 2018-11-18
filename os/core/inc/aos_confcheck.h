/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2018  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _AMIROOS_CONFCHECK_H_
#define _AMIROOS_CONFCHECK_H_

#include <aosconf.h>

/*
 * Kernel parameters and options
 */

#ifndef AMIROOS_CFG_DBG
  #error "AMIROOS_CFG_DBG not defined in aosconf.h"
#endif

#ifndef AMIROOS_CFG_TESTS_ENABLE
  #error "AMIROOS_CFG_TESTS_ENABLE not defined in aosconf.h"
#endif

#ifndef AMIROOS_CFG_PROFILE
  #error "AMIROOS_CFG_PROFILE not defined in aosconf.h"
#endif

#ifndef AMIROOS_CFG_MAIN_LOOP_TIMEOUT
  #error "AMIROOS_CFG_MAIN_LOOP_TIMEOUT not defined in aosconf.h"
#endif

/*
 * SSSP parameters and options
 */

#ifndef AMIROOS_CFG_SSSP_MASTER
  #error "AMIROOS_CFG_SSSP_MASTER not defined in aosconf.h"
#endif

#ifndef AMIROOS_CFG_SSSP_STACK_START
  #error "AMIROOS_CFG_SSSP_STACK_START not defined in aosconf.h"
#endif

#ifndef AMIROOS_CFG_SSSP_STACK_END
  #error "AMIROOS_CFG_SSSP_STACK_END not defined in aosconf.h"
#endif

#if (AMIROOS_CFG_SSSP_STACK_START == true) && (AMIROOS_CFG_SSSP_STACK_END == true)
  #warning "AMIROOS_CFG_SSSP_STACK_START and AMIROOS_CFG_SSSP_STACK_END both enabled in aosconf.h"
  #if (AMIROOS_CFG_SSSP_MASTER != true)
    #error "AMIROOS_CFG_SSSP_MASTER must be enabled in this case"
  #endif
#endif

#ifndef AMIROOS_CFG_SSSP_SIGNALDELAY
  #error "AMIROOS_CFG_SSSP_SIGNALDELAY not defined in aosconf.h"
#endif

#ifndef AMIROOS_CFG_SSSP_SYSSYNCPERIOD
  #error "AMIROOS_CFG_SSSP_SYSSYNCPERIOD not defined in aosconf.h"
#endif

/*
 * System shell options
 */

#ifndef AMIROOS_CFG_SHELL_ENABLE
  #error "AMIROOS_CFG_SHELL_ENABLE not defined in aosconf.h"
#endif

#if (AMIROOS_CFG_SHELL_ENABLE == true)

  #ifndef AMIROOS_CFG_SHELL_STACKSIZE
    #error "AMIROOS_CFG_SHELL_STACKSIZE not defined in aosconf.h"
  #endif

  #ifndef AMIROOS_CFG_SHELL_THREADPRIO
    #error "AMIROOS_CFG_SHELL_THREADPRIO not defined in aosconf.h"
  #endif

  #ifndef AMIROOS_CFG_SHELL_LINEWIDTH
    #error "AMIROOS_CFG_SHELL_LINEWIDTH not defined in aosconf.h"
  #endif

  #ifndef AMIROOS_CFG_SHELL_MAXARGS
    #error "AMIROOS_CFG_SHELL_MAXARGS not defined in aosconf.h"
  #endif

#endif /* AMIROOS_CFG_SHELL_ENABLE == true */

#endif /* _AMIROOS_CONFCHECK_H_ */
