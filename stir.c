/*
 * stir.c
 *
 *  Created on: Aug 24, 2013
 *      Author: brad
 */

//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "lcd.h"
#include "task.h"
#include "adc.h"
#include "leds.h"
#include "semphr.h"
#include "I2C-IO.h"
#include "stir.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
#include "stirApplet.h"

volatile StirState xStirState;

void vStirInit(void){
  // The stirrer motor is on the i2c output board.
  vPCF_ResetBits(STIR_PIN, STIR_PORT); //pull low
  xStirState = STIR_STOPPED;
  vSemaphoreCreateBinary(xStirAppletRunningSemaphore);
}
<<<<<<< HEAD

void vStir( StirState state )
{
  if (state == STIR_DRIVING)
    {
      vPCF_SetBits(STIR_PIN, STIR_PORT);
    }
  else
    {
      vPCF_ResetBits(STIR_PIN, STIR_PORT);
    }
  xStirState = state;

}

StirState xGetStirState(void)
{
  return xStirState;

}


=======
=======
#include "stirApplet.h"
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

volatile StirState xStirState;

void vStirInit(void){
  // The stirrer motor is on the i2c output board.
  vPCF_ResetBits(STIR_PIN, STIR_PORT); //pull low
  xStirState = STIR_STOPPED;
  vSemaphoreCreateBinary(xStirAppletRunningSemaphore);
}

=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
void vStir( StirState state )
{
  if (state == STIR_DRIVING)
    {
      vPCF_SetBits(STIR_PIN, STIR_PORT);
    }
  else
    {
      vPCF_ResetBits(STIR_PIN, STIR_PORT);
    }
  xStirState = state;

}

StirState xGetStirState(void)
{
  return xStirState;

}
>>>>>>> master







