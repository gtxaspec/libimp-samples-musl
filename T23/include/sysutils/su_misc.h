/*
 * Misc utils header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SU_MISC_H__
#define __SU_MISC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * Sysutils Other function header files
 */

/**
 * @defgroup Sysutils_Misc
 * @ingroup sysutils
 * @brief other functions.
 * @{
 */

/**
 * 按键事件.
 */
typedef enum {
	KEY_RELEASED,	/**< Button lifting */
	KEY_PRESSED,	/**< key down */
} SUKeyEvent;

/**
 * LED behavioral command.
 */
typedef enum {
	LED_OFF,		/**< LED close */
	LED_ON,			/**< LED open */
} SULedCmd;

/**
 * @fn int SU_Key_OpenEvent(void)
 *
 * Get button event handle.
 *
 * @param NULL.
 *
 * @retval >0 Key event handle.
 * @retval <=0 fail.
 *
 * @remarks After successfully obtaining a key event handle, start "recording" key events until the key event is close.
 * @remarks If multiple handles are opened, each key event will record a key event.
 * @remarks For example, if two threads each open a button event and each thread holds a handle, then these two threads will read the same sequence of event.
 * 但But if two threads share the same handle, each key event can only be read once.
 *
 * @attention NULL.
 */
int SU_Key_OpenEvent(void);

/**
 * @fn int SU_Key_CloseEvent(int evfd)
 *
 * close key event.
 *
 * @param[in] evfd Key event handle
 *
 * @retval 0 sucess.
 * @retval !=0 fail.
 *
 * @remarks NULL
 *
 * @attention NULL
 */
int SU_Key_CloseEvent(int evfd);

/**
 * @fn int SU_Key_ReadEvent(int evfd, int *keyCode, SUKeyEvent *event)
 *
 * Read key event.
 *
 * @param[in] evfd Key event handle
 * @param[in] keyCode key code
 * @param[out] event Key event pointer.
 *
 * @retval 0 sucess.
 * @retval !=0 fail.
 *
 * @remarks This function blocks until a key event occurs and returns.
 * @remarks The definition of key codes is in Linux/input.h, and the mapping relationship with GPIO is defined in the kernel board level file.
 * @remarks example：
 * @code
	#define KEY_HOME                102 //HOME key
	#define KEY_POWER               116 //ON/OFF,Generally, it can also be used as a wake-up button
	#define KEY_WAKEUP              143 //Wake up button, a button used to wake up the system in addition to the POWER button
	#define KEY_F13                 183 //When PIR is used as a key, it is defined as F13 key
 * @endcode
 *
 * @remarks The definitions of key codes and GPIO numbers, whether they are used as wake-up sources, and the information on valid power levels are all defined in the kernel board level file, as shown below:
 *
 * @code
	struct gpio_keys_button __attribute__((weak)) board_buttons[] = {
	#ifdef GPIO_HOME
		{
			.gpio           = GPIO_HOME,		//Define GPIO number
			.code           = KEY_HOME,			//Define key code
			.desc           = "home key",
			.active_low     = ACTIVE_LOW_HOME,	//Define effective level
	#ifdef WAKEUP_HOME
			.wakeup         = WAKEUP_HOME,		//Define whether it can be used as a wake-up source, where 1 is a wake-up suspend
	#endif
	#ifdef CAN_DISABLE_HOME
			.can_disable    = CAN_DISABLE_HOME,	//Define whether it can be disabled
	#endif
		},
	#endif
	#ifdef GPIO_POWER
		{
			.gpio           = GPIO_POWER,
			.code           = KEY_POWER,
			.desc           = "power key",
			.active_low     = ACTIVE_LOW_POWER,
	#ifdef WAKEUP_POWER
			.wakeup         = WAKEUP_POWER,
	#endif
	#ifdef CAN_DISABLE_POWER
			.can_disable    = CAN_DISABLE_POWER,
	#endif
		},
	#endif
	}
 * @endcode
 * @remarks For digital PIR, one way to use it is to define it as a key, and PIR triggers a key press event (@ ref KEY-PRESSED)，
 * PIR recovery is equivalent to a button lift event (@ ref KEY-RELEASED). If PIR wake-up function is required, define the corresponding button of PIR as the wake-up source.
 * @remarks Please refer to sample keyevent.c for detailed API usage instructions.
 *
 * @attention NULL。
 */
int SU_Key_ReadEvent(int evfd, int *keyCode, SUKeyEvent *event);

/**
 * @fn int SU_Key_DisableEvent(int keyCode)
 *
 * Disable Key event.
 *
 * @param[in] keyCode key code
 *
 * @retval 0 sucess.
 * @retval !=0 fail.
 *
 * @remarks If the button is configured as a wake-up source, pressing the button while the system is suspended (regardless of whether the button is open) will wake up the system.
 * After the disable button event, the system will turn off the interrupt of the button event, and the button will not be able to wake up the system.
 * @remarks This API can be used to disable PIR buttons to wake up the system.
 *
 * @attention NULL.
 */
int SU_Key_DisableEvent(int keyCode);

/**
 * @fn int SU_Key_EnableEvent(int keyCode)
 *
 * Enable key event.
 *
 * @param[in] keyCode key code
 *
 * @retval 0 sucess.
 * @retval !=0 fail.
 *
 * @remarks As a reverse process of the Disable button event. See details@ref SU_Key_DisableEvent(int keyCode)
 *
 * @attention NULL。
 */
int SU_Key_EnableEvent(int keyCode);

/**
 * @fn int SU_LED_Command(int ledNum, SULedCmd cmd)
 *
 * Send LED command.
 *
 * @param[in] ledNum LED num.
 * @param[in] cmd LED command.
 *
 * @retval 0 sucess.
 * @retval !=0 fail.
 *
 * @remarks The LED number for sending LED commands varies depending on the development board. The LED number is defined in the kernel board level file and 
 * registered as a Linux standard Fixed Regulator device. In the board level file, it is necessary to define the GPIO number, effective level, power recursive 
 * relationship, and other information of the LED. The following are examples of two LED fixed regulators defined：
 * @code
    FIXED_REGULATOR_DEF(  //Define fixed regulator
            led0,
            "LED0",         3300000,        GPIO_PA(14),
            HIGH_ENABLE,    UN_AT_BOOT,     0,
            "ldo7",         "vled0",        NULL);

    FIXED_REGULATOR_DEF(
            led1,
            "LED1",         3300000,        GPIO_PA(15),
            HIGH_ENABLE,    UN_AT_BOOT,     0,
            "ldo7",         "vled1",        NULL);

    static struct platform_device *fixed_regulator_devices[] __initdata = {
            &gsensor_regulator_device,
            &led0_regulator_device,
            &led1_regulator_device,
    };

    static int __init fix_regulator_init(void)  //subsys_initcall_sync register regulator
    {
            int i;

            for (i = 0; i < ARRAY_SIZE(fixed_regulator_devices); i++)
                    fixed_regulator_devices[i]->id = i;

            return platform_add_devices(fixed_regulator_devices,
                                        ARRAY_SIZE(fixed_regulator_devices));
    }
    subsys_initcall_sync(fix_regulator_init);
 * @endcode
 * @remarks example：
 * @code
   if (SU_LED_Command(0, LED_ON) < 0)  // Enable LED0
       printf("LED0 turn on error\n");
   if (SU_LED_Command(1, LED_ON) < 0)  //Enable LED1
       printf("LED0 turn on error\n");
   if (SU_LED_Command(0, LED_OFF) < 0)  //Close LED0
       printf("LED1 turn off error\n");
   if (SU_LED_Command(1, LED_OFF) < 0)  //close LED1
       printf("LED1 turn off error\n");
 * @endcode
 * @attention NULL
 */
int SU_LED_Command(int ledNum, SULedCmd cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SU_MISC_H__ */
