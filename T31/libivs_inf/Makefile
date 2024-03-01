CROSS_COMPILE ?= mipsel-linux-

CC = $(CROSS_COMPILE)gcc
## Disable to avoid libstdc++.so deps
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

INCLUDES = -I../include
CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2
LDFLAG = -Wall -Wl,-gc-sections -lpthread -lm -lrt #-lstdc++

SDK_LIB_DIR = ../lib/uclibc

MXU_LIB_DIR = $(SDK_LIB_DIR)/MXU
IVS_LIB_DIR = $(SDK_LIB_DIR)/IVS

LIBS = $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so \
	$(MXU_LIB_DIR)/libmxu_merge.so $(MXU_LIB_DIR)/libmxu_objdetect.so \
	$(MXU_LIB_DIR)/libmxu_contrib.so $(MXU_LIB_DIR)/libmxu_video.so \
	$(MXU_LIB_DIR)/libmxu_imgproc.so $(MXU_LIB_DIR)/libmxu_core.so \
	$(IVS_LIB_DIR)/libpersonDet_inf.so $(IVS_LIB_DIR)/libjzdl.so

SAMPLES = sample-personDet

all: $(SAMPLES)

sample-personDet: sample-common.o sample-personDet.o ../../common/musl_shim.o
	$(CC) -o $@ $^ $(LIBS) $(LDFLAG)
	$(STRIP) $@

%.o:%.c sample-common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
