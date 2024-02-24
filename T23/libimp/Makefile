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

LIBS = $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so

LDFLAG += -Wl,-gc-sections

SAMPLES = sample-Encoder-video \
	sample-OSD \
	sample-IspOsd \
	sample-Audio \
	sample-Ai \
	sample-Ao \
	sample-Ai-AEC \
	sample-Ai-Ref \
	sample-Setfps \
	sample-ISP-flip \
	sample-Encoder-jpeg \
	sample-Encoder-h264-jpeg \
	sample-Encoder-Double-Ivdc \
	sample-Decoder-jpeg \
	sample-IVS-move \
	sample-IVS-unbind-move \
	sample-IVS-base-move\
	sample-Change-Resolution \
	sample-Snap-YUV \
	sample-Framesource \
	sample-EXTFS-Encoder-h264 \
	sample-EXTFS-Encoder-h264-rgba \
	sample-Soft-photosensitive \

all: 	$(SAMPLES)

sample-Ai: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Ai.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ao: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Ao.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ai-AEC: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Ai-AEC.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ai-Ref: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Ai-Ref.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Encoder-video: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-video.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-jpeg: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-jpeg.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-h264-jpeg: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-h264-jpeg.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-Double-Ivdc: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-Double-Ivdc.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-OSD: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-OSD.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IspOsd: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-IspOsd.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Audio: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Audio.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Setfps: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Setfps.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-ISP-flip: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-ISP-flip.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Decoder-jpeg: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Decoder-jpeg.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IVS-move: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-IVS-move.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IVS-unbind-move: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-IVS-unbind-move.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IVS-base-move: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-IVS-base-move.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Change-Resolution: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Change-Resolution.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Snap-YUV: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Snap-YUV.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Framesource: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Framesource.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-EXTFS-Encoder-h264: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-EXTFS-Encoder-h264.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-EXTFS-Encoder-h264-rgba: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-EXTFS-Encoder-h264-rgba.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Soft-photosensitive: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Soft-photosensitive.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

%.o:%.c sample-common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
