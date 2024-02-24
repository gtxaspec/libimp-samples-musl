/*
 * Ingenic SU ADC solution.
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 * Author: Tiger <bohu.liang@ingenic.com>
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define TAG		"ADC"
#define ADC_NUM		8
#define ADC_PATH	"/dev/jz_adc_aux_"
#define ADC_PATH_LEN	32

unsigned int test_vol = 0;
int std_val = 1800;

int adc_fd  = 0;

int SU_ADC_Init(int ch_id)
{
	int fd;
	char path[ADC_PATH_LEN];

	sprintf(path, "%s%d", ADC_PATH, ch_id);
	fd = open(path, O_RDONLY);
	if(fd < 0) {
		printf("test_adc:open error !\n");
		return -1;
	}

	adc_fd = fd;

	return 0;
}

int SU_ADC_Exit(void)
{
	close(adc_fd);

	return 0;
}

int SU_ADC_EnableChn()
{
	int ret;

	ret = ioctl(adc_fd, 0);

	return ret;
}

int SU_ADC_DisableChn()
{
	int ret;

	ret = ioctl(adc_fd, 1);

	return ret;
}

int SU_ADC_GetChnValue()
{
	read(adc_fd,&test_vol, 4);

	return 0;
}

int read_adc(unsigned int ch_id)
{
	int ret;

	ret = SU_ADC_Init(ch_id);
	if(ret < 0) {
		printf("test_adc:SU_ADC_INIT() error !\n");
		return -1;
	}

	ret = SU_ADC_EnableChn();
	if(ret < 0) {
		printf("test_adc:SU_ADC_EnableChn(%d) error !\n",ch_id);
		return -1;
	}

	ret = SU_ADC_GetChnValue();
	if(ret < 0) {
		printf("test_adc:SU_ADC_GetValue() error !\n");
		return -1;
	}

	ret = SU_ADC_DisableChn();
	if(ret < 0) {
		printf("test_adc:SU_ADC_DisableChn() error !\n");
		return -1;
	}

	printf("----[channel%d]: %d mv----\n", ch_id,test_vol);

	SU_ADC_Exit();

	return 0;
}

void help(void)
{
	printf("Input error\n");
	printf("Please input:./test_adc a\n");
	printf("a-->channel id  0:AUX0 1:AUX1 MAX = 8\n");
}

int main(int argc ,char *argv[])
{
	int ret;
	unsigned int ch_id;

	if (argc != 2) {
		help();
		return -1;
	}

	ch_id = atoi(argv[1]);

	if((ch_id >= ADC_NUM) || (ch_id < 0)) {
		printf("test_adc:chn_num error !\n");
		help();
		return -1;
	}

	ret = read_adc(ch_id);
	if (ret < 0)
	{
		printf("test_adc:read_adc() value fail !\n");
		return -1;
	}

	return ret;
}
