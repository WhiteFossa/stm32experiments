#include "hal.h"

/**
 * EXTI channel 0 handler.
 */
void EXTI0_IRQHandler(void)
{
    EXTI->PR |= (1 << HAL_BUTTON_PIN); // Clearing interrupt

    // Interrupt handler here.
    UserButtonPressed();
}

void InitializeHardware(void)
{
	SetFullClock();

	// Setting up ports

	// Port A (leds)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// Full speed
	GPIOA->OSPEEDR |= 0b11 << (HAL_LED_RED_BIT << 1);
	GPIOA->OSPEEDR |= 0b11 << (HAL_LED_GREEN_BIT << 1);
	GPIOA->OSPEEDR |= 0b11 << (HAL_LED_BLUE_BIT << 1);

	// Alternative functions
	GPIOA->MODER |= 0b10 << (HAL_LED_RED_BIT << 1);
	GPIOA->MODER |= 0b10 << (HAL_LED_GREEN_BIT << 1);
	GPIOA->MODER |= 0b10 << (HAL_LED_BLUE_BIT << 1);

	// AF1 - TIM2 Channels 2,3,4
	GPIOA->AFR[0] |= 1 << (HAL_LED_RED_BIT << 2);
	GPIOA->AFR[0] |= 1 << (HAL_LED_GREEN_BIT << 2);
	GPIOA->AFR[0] |= 1 << (HAL_LED_BLUE_BIT << 2);

	// Button to input
	GPIOA->MODER &= ~(1 << HAL_BUTTON_PIN);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR0_1 << HAL_BUTTON_PIN); // Pull-down = PUPDR: 10

	// We want interrupt on button press (falling edge). PA0, so EXTI channel 0 -> NVIC channel 6
	EXTI->IMR |= (1 << HAL_BUTTON_PIN);
	EXTI->FTSR |= (1 << HAL_BUTTON_PIN);

	// Enabling 6th channel
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();

	// Highest user IRQ priority (0), 1 sub-pri
	uint32_t priority = NVIC_EncodePriority(prioritygroup, 0, 1 );
	NVIC_SetPriority(EXTI0_IRQn, priority);

	NVIC_EnableIRQ(EXTI0_IRQn);

	// Timer 2 power on
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// SMS = 0b000, no slave mode
	TIM2->SMCR &= ~TIM_SMCR_SMS;

	// Prescaler
	TIM2->PSC = 1;

	// Counting from 0 to this value
	TIM2->ARR = HAL_TIM2_MAX;

	// PWM mode
	TIM2->CCMR1 = (TIM2->CCMR1 & ~TIM_CCMR1_OC2M) | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
	TIM2->CCMR2 = (TIM2->CCMR2 & ~TIM_CCMR2_OC3M) | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
	TIM2->CCMR2 = (TIM2->CCMR2 & ~TIM_CCMR2_OC4M) | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;

	// Enabling output compare
	TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

	TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;
}

void SetFullClock(void)
{
	// Disabling CSS
	RCC->CR &= RCC_CR_CSSON;

	// Starting HSI
	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY)) {}

	// HSI ready, switching to it
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

	// Switched to HSI, now starting HSE
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY)) {}

	// HSE ready, setting-up prescalers

	// RTC clock
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_RTCPRE) | HAL_RTCPRE_VALUE ;

	// APB2 clock
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | HAL_HSE_APB2_PRE;

	// APB1 clock
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | HAL_HSE_APB1_PRE;

	// AHB clock
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | HAL_HSE_AHB_PRE;

	// Switching to HSE.
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSE;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}

	// Switching off HSI
	RCC->CR &= RCC_CR_HSION;

	// Setting-up PLL
	// PLL from HSE
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLSRC) | RCC_PLLCFGR_PLLSRC_HSE;

	// PLLM
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | HAL_PLLM;

	// PLLN
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | HAL_PLLN;

	// PLLP
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLP) | HAL_PLLP;

	// PLLQ
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLQ) | HAL_PLLQ;

	// Switching PLL on and waiting for lock
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) {}

	// PLL ready, setting-up prescalers

	// APB2 clock
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | HAL_PLL_APB2_PRE;

	// APB1 clock
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | HAL_PLL_APB1_PRE;

	// AHB clock
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | HAL_PLL_AHB_PRE;

	// Configurina = RCC->CFGR;a = RCC->CFGR;a = RCC->CFGR;g memory

	// Amount of waitstates
	FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | HAL_FLASH_WAITSTATES;

	// Prefetch and caches enable
	FLASH->ACR = FLASH->ACR | FLASH_ACR_PRFTEN | FLASH_ACR_DCEN | FLASH_ACR_ICEN;

	// Switching to PLL
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

}
