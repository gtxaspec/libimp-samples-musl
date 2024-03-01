CONFIG_MUSL_BUILD=y

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

INCLUDES = -I$(SDK_INC_DIR)

LIBS =	$(SDK_LIB_DIR)/libsysutils.so $(SDK_LIB_DIR)/libalog.so

LDFLAG += -Wl,-gc-sections

SAMPLES = sample-keyevent \
	sample-alarm \
	sample-led \
	sample-pir \
	sample-cipher \
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

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
