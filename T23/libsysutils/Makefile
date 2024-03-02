CROSS_COMPILE ?= mipsel-linux-

CC = $(CROSS_COMPILE)gcc
## Disable to avoid libstdc++.so deps
# CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2

SDK_LIB_DIR = ../lib/uclibc

SDK_INC_DIR = ../include

INCLUDES := -I$(SDK_INC_DIR)

LIBS := $(SDK_LIB_DIR)/libsysutils.so $(SDK_LIB_DIR)/libalog.so

LDFLAG += -Wl,-gc-sections

COMMON_OBJ := ../../common/musl_shim.o

SAMPLES_SRC := $(filter-out sample-common.c, $(wildcard *.c))
SAMPLES_OBJ := $(SAMPLES_SRC:%.c=%.o)

SAMPLES := $(SAMPLES_OBJ:%.o=%)

all: 	$(SAMPLES)

$(SAMPLES):%:%.o $(COMMON_OBJ)
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS)
	$(STRIP) $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
