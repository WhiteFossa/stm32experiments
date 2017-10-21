/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------


#include "main.h"
#include "hal.c"


// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"



int GetLedPinByChannel(unsigned char channel)
{
	switch (channel)
	{
		case PWM_RED:
			return LED_RED;
		case PWM_GREEN:
			return LED_GREEN;
		case PWM_BLUE:
			return LED_BLUE;
		default:
			return -1;
	}
}

void NextBrightness()
{
	switch (brightness_state)
	{
		case INC_RED:
			brightness[PWM_RED] += BRIGHTNESS_STEP;

			if (brightness[PWM_RED] >= 1)
			{
				brightness_state = INC_GREEN;
			}
		break;

		case INC_GREEN:
			brightness[PWM_GREEN] += BRIGHTNESS_STEP;

			if (brightness[PWM_GREEN] >= 1)
			{
				brightness_state = INC_BLUE;
			}
		break;

		case INC_BLUE:
			brightness[PWM_BLUE] += BRIGHTNESS_STEP;

			if (brightness[PWM_BLUE] >= 1)
			{
				brightness_state = DEC_RED;
			}
		break;

		case DEC_RED:
			brightness[PWM_RED] -= BRIGHTNESS_STEP;

			if (brightness[PWM_RED] <= 0)
			{
				brightness_state = DEC_GREEN;
			}
		break;

		case DEC_GREEN:
			brightness[PWM_GREEN] -= BRIGHTNESS_STEP;

			if (brightness[PWM_GREEN] <= 0)
			{
				brightness_state = DEC_BLUE;
			}
		break;

		case DEC_BLUE:
			brightness[PWM_BLUE] -= BRIGHTNESS_STEP;

			if (brightness[PWM_BLUE] <= 0)
			{
				brightness_state = INC_RED;
			}
		break;
	}
}

unsigned char CorrectPWMLevel(float brightness, unsigned char channel)
{
	int result = floor(brightness * correction_a[channel] + correction_b[channel] + 0.5);
	if (result > 255)
	{
		result = 255;
	}
	else if (result < 0)
	{
		result = 0;
	}

	return (unsigned char)result;
}

int main(int argc, char* argv[])
{
	// Hardware initialization.
	InitializeHardware();

	// Correction table
	correction_a[PWM_RED] = CORRECTION_A_RED;
	correction_a[PWM_GREEN] = CORRECTION_A_GREEN;
	correction_a[PWM_BLUE] = CORRECTION_A_BLUE;

	correction_b[PWM_RED] = CORRECTION_B_RED;
	correction_b[PWM_GREEN] = CORRECTION_B_GREEN;
	correction_b[PWM_BLUE] = CORRECTION_B_BLUE;

	// Enabling port D
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	// Open drain output
	GPIOD->MODER |= 01 << (LED_RED << 1);
	GPIOD->MODER |= 01 << (LED_ORANGE << 1);
	GPIOD->MODER |= 01 << (LED_GREEN << 1);
	GPIOD->MODER |= 01 << (LED_BLUE << 1);

	// Software PWM
	pwm_levels[PWM_RED] = 0;
	pwm_levels[PWM_GREEN] = 0;
	pwm_levels[PWM_BLUE] = 0;


	brightness[PWM_RED] = 0;
	brightness[PWM_GREEN] = 0;
	brightness[PWM_BLUE] = 0;

	// Endless loop
	unsigned char pwm = 0;

	unsigned int counter = 0;

	while(1)
	{
		for (unsigned char channel = 0; channel < 3; channel ++)
		{
			volatile int ledPin = GetLedPinByChannel(channel);
			if (pwm_levels[channel] > pwm)
			{
				GPIOD->ODR |= BV(ledPin);
			}
			else
			{
				GPIOD->ODR &= ~BV(ledPin);
			}
		}

		pwm ++;

		if (counter < 1000)
		{
			counter ++;
		}
		else
		{
			NextBrightness();

			for (unsigned char channel = 0; channel < 3; channel ++)
			{
				pwm_levels[channel] = CorrectPWMLevel(brightness[channel], channel);
			}

			counter = 0;
		}
	}

  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
