CONFIG_MUSL_BUILD=y

CROSS_COMPILE ?= mipsel-linux-

CC = $(CROSS_COMPILE)gcc
## Disable to avoid libstdc++.so deps
# CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2

ifeq ($(CONFIG_MUSL_BUILD), y)
SDK_LIB_DIR = ../lib/uclibc
else
SDK_LIB_DIR = ../lib/glibc
endif

SDK_INC_DIR = ../include

INCLUDES = -I$(SDK_INC_DIR)

LIBS =	$(SDK_LIB_DIR)/libsysutils.a $(SDK_LIB_DIR)/libalog.a

LDFLAG += -Wl,-gc-sections -static

SAMPLES = sample-keyevent \
	sample-pir \
	sample-chipid \
	sample-adc \
	sample-wdt

all: 	$(SAMPLES)

sample-keyevent: ../../common/musl_shim.o sample-keyevent.o
	$(CC)  $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-alarm: ../../common/musl_shim.o sample-alarm.o
	$(CC)  $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt

sample-led: ../../common/musl_shim.o sample-led.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt

sample-pir: ../../common/musl_shim.o sample-pir.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt

sample-cipher: ../../common/musl_shim.o sample-cipher.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt

sample-chipid: ../../common/musl_shim.o sample-chipid.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt

sample-adc: ../../common/musl_shim.o sample-adc.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt

sample-wdt: ../../common/musl_shim.o sample-wdt.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
