#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include <math.h>

// Bits in port D
#define LED_RED			14
#define LED_ORANGE		13
#define LED_GREEN		12
#define LED_BLUE		15

// PWM levels
#define PWM_RED			0
#define PWM_GREEN		1
#define PWM_BLUE		2

unsigned char pwm_levels[3];

// Brightness states
#define INC_RED			0
#define INC_GREEN		1
#define INC_BLUE		2
#define DEC_RED			3
#define DEC_GREEN		4
#define DEC_BLUE		5

// Correction factors by colors
// PWM = a*X + b, X=[0,1]
#define CORRECTION_A_RED	255.0
#define CORRECTION_A_GREEN	128.0
#define CORRECTION_A_BLUE	255.0

#define CORRECTION_B_RED	0.0
#define CORRECTION_B_GREEN	0.0
#define CORRECTION_B_BLUE	0.0

float correction_a[3];
float correction_b[3];

unsigned char CorrectPWMLevel(float brightness, unsigned char channel);

#define BRIGHTNESS_STEP 0.0001

float brightness[3];
unsigned char brightness_state;

int GetLedPinByChannel(unsigned char channel);

void NextBrightness();

int main(int argc, char* argv[]);
