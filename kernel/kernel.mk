# set kernel path variable
AMIROOS_KERNEL = $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))/ChibiOS

