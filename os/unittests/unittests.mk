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



# absolute path to this directory
UNITTESTS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# include path
UNITTESTSINC = $(UNITTESTS_DIR)lld/inc \
               $(UNITTESTS_DIR)periphery-lld/inc

# C sources
UNITTESTSCSRC = $(UNITTESTS_DIR)lld/src/ut_lld_adc.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_a3906.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_at24c01bn-sh-b.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_bq24103a.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_bq27500.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_bq27500_bq24103a.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_hmc5883l.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_ina219.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_l3g4200d.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_led.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_lis331dlh.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_ltc4412.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_mpr121.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_pca9544a.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_pklcs1212e4001.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_tlc5947.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_tps2051bdbv.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_tps62113.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_tps62113_ina219.c \
                $(UNITTESTS_DIR)periphery-lld/src/ut_alld_vcnl4020.c

