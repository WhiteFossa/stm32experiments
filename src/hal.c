#include "hal.h"

void InitializeHardware(void)
{
	SetFullClock();
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

	// Swithching off HSI
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

	// Configuring memory

	// Amount of waitstates
	FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | HAL_FLASH_WAITSTATES;

	// Prefetch and caches enable
	FLASH->ACR = FLASH->ACR | FLASH_ACR_PRFTEN | FLASH_ACR_DCEN | FLASH_ACR_ICEN;

	// Switching to PLL
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

}
