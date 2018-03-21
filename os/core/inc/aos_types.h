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

#ifndef _AMIROOS_TYPES_H_
#define _AMIROOS_TYPES_H_

/**
 * @brief   Error codes used in AMiRo-OS.
 */
typedef enum {
  AOS_OK = 0,                 /**< No error, success. */
  AOS_SUCCESS = 0,            /**< No error, success */
  AOS_WARNING = 1,            /**< No error, but a warning occurred */
  AOS_INVALID_ARGUMENTS = 2,  /**< No error, but given arguments are invalid */
  AOS_ERROR = 0x80,           /**< An error occurred */
  AOS_FAILURE = 0x80,         /**< An error occurred */
} aos_status_t;

#endif /* _AMIROOS_TYPES_H_ */
