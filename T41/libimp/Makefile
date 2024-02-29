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
	sample-IspOsd \
	sample-OSD \
	sample-Audio \
	sample-Ai \
	sample-Ao \
	sample-Ai-AEC \
	sample-Ai-Ref \
	sample-dmic \
	sample-dmic-aec \
	sample-dmic-ref \
	sample-Setfps \
	sample-ISP-flip \
	sample-Encoder-jpeg \
	sample-Encoder-yuv \
	sample-Encoder-jpeg-ivpu \
	sample-Encoder-h265-ivpu-jpeg \
	sample-IVS-move \
	sample-IVS-base-move\
	sample-Change-Resolution \
	sample-Snap-Raw \
	sample-Snap-YUV \
	sample-Framesource \
	sample-LCD \
	sample-AutoZoom \
	sample-Fcrop \
	sample-IVS-unbind-move \
	sample-Soft-photosensitive \
	sample-Encoder-video-direct \
	sample-Encoder-OSD \
	sample-OSD-SnapOsdFrame \
	sample-Extern-inject-yuv \
	sample-I2d \

all: 	$(SAMPLES)

sample-Ai: ../../common/musl_shim.o sample-Ai.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ao: ../../common/musl_shim.o sample-Ao.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ai-AEC: ../../common/musl_shim.o sample-Ai-AEC.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ai-Ref: ../../common/musl_shim.o sample-Ai-Ref.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-dmic: ../../common/musl_shim.o sample-dmic.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-dmic-aec: ../../common/musl_shim.o sample-dmic-aec.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-dmic-ref: ../../common/musl_shim.o sample-dmic-ref.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Encoder-video: ../../common/musl_shim.o sample-Encoder-video.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-jpeg: ../../common/musl_shim.o sample-Encoder-jpeg.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-yuv: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-Encoder-yuv.o ../../common/musl_shim.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-jpeg-ivpu: ../../common/musl_shim.o sample-Encoder-jpeg-ivpu.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-h265-ivpu-jpeg: ../../common/musl_shim.o sample-Encoder-h265-ivpu-jpeg.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IspOsd: ../../common/musl_shim.o sample-IspOsd.o
		$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
			$(STRIP) $@

sample-OSD: ../../common/musl_shim.o sample-OSD.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Audio: ../../common/musl_shim.o sample-Audio.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Setfps: ../../common/musl_shim.o sample-Setfps.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-ISP-flip: ../../common/musl_shim.o sample-ISP-flip.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IVS-move: ../../common/musl_shim.o sample-IVS-move.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IVS-base-move: ../../common/musl_shim.o sample-IVS-base-move.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IVS-unbind-move: ../../common/musl_shim.o sample-IVS-unbind-move.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Change-Resolution: ../../common/musl_shim.o sample-Change-Resolution.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Snap-Raw: ../../common/musl_shim.o sample-Snap-Raw.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Snap-YUV: ../../common/musl_shim.o sample-Snap-YUV.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Framesource: ../../common/musl_shim.o sample-Framesource.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-LCD: ../../common/musl_shim.o sample-LCD.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-ISP-Bypass: ../../common/musl_shim.o sample-ISP-Bypass.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-EXTFS-Encoder-h264: ../../common/musl_shim.o sample-EXTFS-Encoder-h264.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-EXTFS-Encoder-h264-bgra: ../../common/musl_shim.o sample-EXTFS-Encoder-h264-bgra.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Framesource-algo: ../../common/musl_shim.o sample-Framesource-algo.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-AutoZoom: ../../common/musl_shim.o sample-AutoZoom.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Fcrop: ../../common/musl_shim.o sample-Fcrop.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Soft-photosensitive: ../../common/musl_shim.o sample-Soft-photosensitive.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Encoder-video-direct: ../../common/musl_shim.o sample-Encoder-video-direct.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Encoder-OSD: ../../common/musl_shim.o sample-Encoder-OSD.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-OSD-SnapOsdFrame: ../../common/musl_shim.o sample-OSD-SnapOsdFrame.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Extern-inject-yuv: ../../common/musl_shim.o sample-Extern-inject-yuv.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-I2d: ../../common/musl_shim.o sample-I2d.o
	$(CC) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

%.o:%.c sample-common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
