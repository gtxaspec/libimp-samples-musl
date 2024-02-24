#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define ADC_ENABLE      0
#define ADC_DISABLE     1
#define ADC_SET_VREF    2
#define ADC_PATH_LEN    32

#define ADC_PATH    "/dev/jz_adc_aux_" /* adc channel 0-3 */
#define STD_VAL_VOLTAGE 1800 /* The unit is mv. T10/T20 VREF=3300; T30/T21/T31 VREF=1800 */

int fd;
int vol_n;

void *adc_get_voltage_thread(void *argc)
{
    int *value_ptr = malloc(sizeof(int));
    if (value_ptr == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    // Read value
    if (read(fd, value_ptr, sizeof(int)) != sizeof(int)) {
        perror("Failed to read value");
        free(value_ptr);
        return NULL;
    }

//    printf("%d\n", *value_ptr);
    return value_ptr;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int ch_id = 0; /* change test channel */
    char path[ADC_PATH_LEN];
    int *returned_value;

    sprintf(path, "%s%d", ADC_PATH, ch_id);
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("sample_adc: open error");
        return -1;
    }

    // Set reference voltage
    vol_n = STD_VAL_VOLTAGE;
    ret = ioctl(fd, ADC_SET_VREF, &vol_n);
    if (ret) {
        perror("Failed to set reference voltage");
        close(fd);
        return -1;
    }

    // Enable ADC
    ret = ioctl(fd, ADC_ENABLE);
    if (ret) {
        perror("Failed to enable ADC");
        close(fd);
        return -1;
    }

    // Create thread
    pthread_t adc_thread_id;
    ret = pthread_create(&adc_thread_id, NULL, adc_get_voltage_thread, NULL);
    if (ret != 0) {
        perror("Error: pthread_create error");
        close(fd);
        return -1;
    }

    // Wait for thread to finish and capture the return value
    if (pthread_join(adc_thread_id, (void**)&returned_value) != 0) {
        perror("pthread_join error");
        close(fd);
        return -1;
    }

    if (returned_value != NULL) {
        printf("%d\n", *returned_value);
        free(returned_value);
    }

    // Disable ADC
    ret = ioctl(fd, ADC_DISABLE);
    if (ret) {
        perror("Failed to disable ADC");
    }

    // Close file descriptor
    close(fd);

    return 0;
}
