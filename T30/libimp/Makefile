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
	sample-Audio \
	sample-Ai \
	sample-Ao \
	sample-Ai-AEC \
	sample-Ai-Ref \
	sample-Setfps \
	sample-ISP-flip \
	sample-Decoder-jpeg \
	sample-Encoder-video-IVS-move \
	sample-Change-Resolution \
	sample-Snap-Raw \
	sample-Encoder-video-IVS-exalgo \
	sample-Framesource \
	sample-Capture-nv12 \
	sample-Encoder-jpeg \
	sample-Encoder-video \
	sample-EXTFS-Encoder-h264 \
	sample-EXTFS-Encoder-h264-bgra \
	sample-dmic\
	sample-dmic-aec\
	sample-dmic-ref\
	sample-Framesource-algo \


all: 	$(SAMPLES)

sample-Encoder-video: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-video.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-OSD: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-OSD.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Ai: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-Ai.o ../../common/musl_shim.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ao: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-Ao.o ../../common/musl_shim.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ai-AEC: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-Ai-AEC.o ../../common/musl_shim.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ai-Ref: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-Ai-Ref.o ../../common/musl_shim.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Audio: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Audio.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-dmic: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-dmic.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@
sample-dmic-aec: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-dmic-aec.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@
sample-dmic-ref: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-dmic-ref.o
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

sample-Encoder-video-IVS-move: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-video-IVS-move.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Change-Resolution: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-Change-Resolution-Common.o sample-Change-Resolution.o ../../common/musl_shim.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Snap-Raw: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Snap-Raw.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-video-IVS-exalgo: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-video-IVS-exalgo.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Framesource: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Framesource.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-jpeg: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-jpeg.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Capture-nv12: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Capture-nv12.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-EXTFS-Encoder-h264-bgra: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-EXTFS-Encoder-h264-bgra.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-EXTFS-Encoder-h264: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-EXTFS-Encoder-h264.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-video: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Encoder-video.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Framesource-algo: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o ../../common/musl_shim.o sample-Framesource-algo.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

%.o:%.c sample-common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
