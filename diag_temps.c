/*
 * diag_temps.c
 *
 *  Created on: Dec 14, 2012
 *      Author: brad
 */
#include <stdint.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "lcd.h"
#include "task.h"
#include "adc.h"
#include "leds.h"
#include "semphr.h"
#include "diag_temps.h"
#include "ds1820.h"


// semaphore that stops the returning from the applet to the menu system until the applet goes into the blocked state.
xSemaphoreHandle xAppletRunningSemaphore;

void  vTaskDS1820DisplayTemps( void *pvParameters);

xTaskHandle xTaskDS1820DisplayTempsHandle;
////////////////////////////////////////////////////////////////////////////

void vDiagTempsInit(void)
{
  vSemaphoreCreateBinary(xAppletRunningSemaphore);

}

void vDS1820DiagApplet(int init){

        if (init){
                //lcd_clear(Black);
        xTaskCreate( vTaskDS1820DisplayTemps,
                         ( signed portCHAR * ) "ds1820_dsp",
                         configMINIMAL_STACK_SIZE + 500,
                         NULL,
                         tskIDLE_PRIORITY+1,
                         &xTaskDS1820DisplayTempsHandle );
        }
}

int DS1820DiagKey(int xx, int yy){

        if (xx > 200 && yy > 200)
        {
            //try to take the semaphore from the display applet. wait here if we cant take it.
                xSemaphoreTake(xAppletRunningSemaphore, portMAX_DELAY);
                vTaskDelete(xTaskDS1820DisplayTempsHandle);
                //return the semaphore for taking by another task.
                      xSemaphoreGive(xAppletRunningSemaphore);
                vTaskDelay(100);

                return 1;
        }
        vTaskDelay(10);
        return 0; // xx > 200 && yy > 200;
}


void  vTaskDS1820DisplayTemps( void *pvParameters){

        //char lcd_string[20];
        static int count = 0;
        lcd_printf(1,1, 12, "TEMPERATURES");
        for (;;)
        {
            xSemaphoreTake(xAppletRunningSemaphore, portMAX_DELAY); //take the semaphore so that the key handler wont
                                                                            //return to the menu system until its returned
                count++;
                //portENTER_CRITICAL();
                lcd_fill(1,50, 200, 190, Black);


                lcd_printf(1, 5, 10, "HLT = %.2f", ds1820_get_temp(HLT));
                lcd_printf(1, 6, 10, "Mash = %.2f", ds1820_get_temp(MASH));
                lcd_printf(1, 7, 10, "Cabinet = %.2f", ds1820_get_temp(CABINET));
                lcd_printf(1, 8, 10, "Ambient = %.2f", ds1820_get_temp(AMBIENT));
                lcd_printf(1, 9, 10, "HLT_SSR = %.2f", ds1820_get_temp(HLT_SSR));
                lcd_printf(1, 10, 10, "BOIL_SSR = %.2f", ds1820_get_temp(BOIL_SSR));

                //printf("Display High water = %u\r\n",uxTaskGetStackHighWaterMark(NULL));
                //portEXIT_CRITICAL();

                xSemaphoreGive(xAppletRunningSemaphore); //give back the semaphore as its safe to return now.
                vTaskDelay(500);

        }
}
////////////////////////////////////////////////////////////////////////////