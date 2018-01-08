
#include "stm32f4xx.h"
#include "../fossa/libfossastm32/common/macroses.h"

/**
 * BEGIN CLOCK-DEPENDENT DEFINES
 * For 8MHz HSE crystall.
 * 168 MHz SYSCLK.
 */

/*
 * We need RTC clock equal to 1MHz. RTC clock = HSE_CLOCK / RTCPRE.
 * RTCPRE = 8.
 */
#define HAL_RTCPRE_VALUE RCC_CFGR_RTCPRE_3

/**
 * Prescaler for APB2.
 * Max. clock = 84 MHz
 */
#define HAL_HSE_APB2_PRE RCC_CFGR_PPRE2_DIV1
#define HAL_PLL_APB2_PRE RCC_CFGR_PPRE2_DIV2

/**
 * Prescaler for APB1.
 * Max. clock = 42 MHz
 */
#define HAL_HSE_APB1_PRE RCC_CFGR_PPRE1_DIV1
#define HAL_PLL_APB1_PRE RCC_CFGR_PPRE1_DIV4

/**
 * Prescaler for AHB when clocked from HSE/PLL.
 */
#define HAL_HSE_AHB_PRE RCC_CFGR_HPRE_DIV1
#define HAL_PLL_AHB_PRE RCC_CFGR_HPRE_DIV1

/**
 * PLL-related stuff.
 * HSE = 8 MHz
 * Fin = 1 MHz
 * Fpll = 168 MHz
 * Fusb = 48 MHz
 *
 * Fin = HSE / PLLM (must be 1-2 MHz, PLLM = 2 - 63)
 * Fvco = Fin * PLLN (Fvco must be 100-432 MHz, PLLN = 5-432).
 * Fpll = Fvco / PLLP (PLLP = 2, 4, 6, 8)
 * Fusb = Fvco / PLLQ (PLLQ = 2 - 15).
 *
 * So, we need Fvco = 336 MHz.
 *
 * PLLM = 8 (0b0000 1000)
 * PLLN = 336 (0b1 0101 0000)
 * PLLP = 2
 * PLLQ = 7
 */

#define HAL_PLLM (RCC_PLLCFGR_PLLM_3)
#define HAL_PLLN (RCC_PLLCFGR_PLLN_8 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_4)
#define HAL_PLLP (0) // 0 -> PLLP = 2
#define HAL_PLLQ (RCC_PLLCFGR_PLLQ_2 | RCC_PLLCFGR_PLLQ_1 | RCC_PLLCFGR_PLLQ_0)

/**
 * Amount of wait states for FLASH read (depends on system clock).
 */
#define HAL_FLASH_WAITSTATES (FLASH_ACR_LATENCY_5WS)


/**
 * END CLOCK-DEPENDENT DEFINES
 */

/**
 * GPIO-related stuff.
 */

/**
 * Leds at port A.
 */
#define HAL_LED_RED_BIT		1
#define HAL_LED_GREEN_BIT	2
#define HAL_LED_BLUE_BIT	3

/**
 * Button pin on port A.
 */
#define HAL_BUTTON_PIN		0

/**
 * Timer 2 maximal value.
 * 42 MHz / 500 000 = 84 Hz PWM.
 */
#define HAL_TIM2_MAX		500000

/**
 * Basic hardware initialization.
 */
void InitializeHardware(void);

/**
 * Sets maximal possible clock.
 */
void SetFullClock(void);
