//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 11/11/2013
// Email Address: W0085400@umail.usq.edu.au
//
// Purpose:
// Pre:
// Post:
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
<<<<<<< HEAD
<<<<<<< HEAD
#include <string.h>
=======
>>>>>>> master
=======
#include <string.h>
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "lcd.h"
#include "task.h"
#include "adc.h"
#include "leds.h"
#include "semphr.h"
#include "queue.h"
<<<<<<< HEAD
<<<<<<< HEAD
#include "console.h"
#include "main.h"
=======
>>>>>>> master
=======
#include "console.h"
#include "main.h"
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

xQueueHandle xPrintQueue;


void vConsolePrintTask(void * pvParameters)
{
  char * pcMessageToPrint;
  static char * pcLastMessage;
  for (;;)
    {
      xQueueReceive(xPrintQueue, &pcMessageToPrint, portMAX_DELAY);
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
      //sprintf(cBuffer,"%s", &pcMessageToPrint);

      //if (pcMessageToPrint != pcLastMessage)
        {
          portENTER_CRITICAL();
          printf(pcMessageToPrint);
<<<<<<< HEAD
<<<<<<< HEAD
          fflush(stdout);
          portEXIT_CRITICAL();
      //    pcLastMessage = pcMessageToPrint;
          vTaskDelay(10); //wait for the usart to print before filling it's buffer.
          //maybe can do something with the interrupt flags here?
=======
          portEXIT_CRITICAL();
      //    pcLastMessage = pcMessageToPrint;

>>>>>>> master
=======
          fflush(stdout);
          portEXIT_CRITICAL();
      //    pcLastMessage = pcMessageToPrint;
          vTaskDelay(10); //wait for the usart to print before filling it's buffer.
          //maybe can do something with the interrupt flags here?
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
        }
    }


}

<<<<<<< HEAD
<<<<<<< HEAD

const char * pcX = "ConsolePrint failed\r\n";
void vConsolePrint(const char * format)
{

   if ( xQueueSendToBack(xPrintQueue, &format, 30) != pdPASS)
     xQueueSendToBack(xPrintQueue, &pcX, 30);

=======
void vConsolePrint(const char * cX)
{
  const char * pcX = "ConsolePrint failed\r\n";
   if ( xQueueSendToBack(xPrintQueue, &cX, 10) != pdPASS)
     xQueueSendToBack(xPrintQueue, &pcX, 10);
>>>>>>> master
=======

const char * pcX = "ConsolePrint failed\r\n";
void vConsolePrint(const char * format)
{

   if ( xQueueSendToBack(xPrintQueue, &format, 30) != pdPASS)
     xQueueSendToBack(xPrintQueue, &pcX, 30);

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
}
