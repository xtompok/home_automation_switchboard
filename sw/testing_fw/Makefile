PROJECT = testing-fw
BUILD_DIR = bin

SHARED_DIR = ../shared
CFILES = main.c
#CFILES += api.c
#AFILES += api-asm.S

# TODO - you will need to edit these two lines!
DEVICE=stm32g070cbt6
OOCD_INTERFACE = stlink-v2
OOCD_TARGET = stm32g0x


V = 1

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
