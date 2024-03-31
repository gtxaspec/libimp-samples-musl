CROSS_COMPILE ?= mipsel-linux-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2

SDK_LIB_DIR = ../lib/uclibc

SDK_INC_DIR = ../include

INCLUDES := -I$(SDK_INC_DIR)

LIBS := $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so

LDFLAGS += -Wl,-gc-sections

COMMON_OBJ := sample-common.o ../../ingenic_musl/musl_shim.o

SAMPLES_SRC := $(filter-out sample-common.c sample-Change-Resolution.c sample-Change-Resolution-Common.c, $(wildcard *.c))
SAMPLES_OBJ := $(SAMPLES_SRC:%.c=%.o)
SAMPLES := $(SAMPLES_OBJ:%.o=%)

all: $(SAMPLES) sample-Change-Resolution

$(SAMPLES): % : %.o $(COMMON_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

sample-Change-Resolution-Common.o: sample-Change-Resolution-Common.c
	$(CC) -c $(CFLAGS) $< -o $@

sample-Change-Resolution.o: sample-Change-Resolution.c sample-Change-Resolution-Common.o
	$(CC) -c $(CFLAGS) $< -o $@

sample-Change-Resolution: sample-Change-Resolution.o $(COMMON_OBJ) sample-Change-Resolution-Common.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@

clean:
	rm -f *.o *~ $(SAMPLES) sample-Change-Resolution

distclean: clean
	rm -f $(SAMPLES)
