//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 18 Feb 2012
// Email Address: W0085400@umail.usq.edu.au
// 
// Purpose:
// Pre:
// Post:
// RCS $Date$
// RCS $Revision$
// RCS $Source$
// RCS $Log$
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "lcd.h"
#include "task.h"
#include "adc.h"
#include "leds.h"
#include "semphr.h"
#include "queue.h"
#include "I2C-IO.h"
#include "console.h"
#include "crane.h"
#include "brew.h"
#include "io_util.h"
#include "message.h"
#include "stir.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#include "main.h"
=======
>>>>>>> master
=======
#include "main.h"
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
#include "main.h"
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

volatile int8_t cs = STOPPED;
int8_t iCraneState = STOPPED;

//static const int STEP_COMPLETE = BREW_STEP_COMPLETE;
//static const int STEP_FAILED = BREW_STEP_FAILED;
//static const int STEP_TIMEOUT = BREW_STEP_TIMEOUT;

xQueueHandle xCraneQueue = NULL;
xSemaphoreHandle xAppletRunningSemaphore;
xTaskHandle xCraneTaskHandle = NULL, xCraneAppletDisplayHandle = NULL;

void vTaskCrane(void * pvParameters);


void vCraneInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Set up the input pin configuration for PE4
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Pin =  CRANE_UPPER_LIMIT_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//  GPIO_Init( CRANE_UPPER_LIMIT_PORT, &GPIO_InitStructure );
<<<<<<< HEAD
<<<<<<< HEAD

//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Pin =  CRANE_LOWER_LIMIT_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//  GPIO_Init( CRANE_LIMIT_PORT, &GPIO_InitStructure );
=======
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin =  CRANE_UPPER_LIMIT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init( CRANE_UPPER_LIMIT_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin =  CRANE_LOWER_LIMIT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init( CRANE_LIMIT_PORT, &GPIO_InitStructure );
>>>>>>> master
=======

=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Pin =  CRANE_LOWER_LIMIT_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//  GPIO_Init( CRANE_LIMIT_PORT, &GPIO_InitStructure );
<<<<<<< HEAD
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

  vPCF_ResetBits(CRANE_PIN1, CRANE_PORT); //pull low
  vPCF_ResetBits(CRANE_PIN2, CRANE_PORT); //pull low

  vSemaphoreCreateBinary(xAppletRunningSemaphore);

  xCraneQueue = xQueueCreate( 5, sizeof( struct GenericMessage * ) );

  if (xCraneQueue == NULL)
    {
      vConsolePrint("Crane Queue couldn't be created\r\n");
    }


  vConsolePrint("Crane Initialised\r\n");

}


void vCraneFunc(int Command)
{
  //portENTER_CRITICAL(); // so that we dont get switched out whilst accessing these pins.

  switch (Command)
  {
  case UP:
    {
      // need to turn relay 2 off which sets up for REV direction
      vPCF_ResetBits(CRANE_PIN2, CRANE_PORT); //pull low
      vPCF_SetBits(CRANE_PIN1, CRANE_PORT); //pull low
      break;
    }
  case DN:
    {
      // need to turn relay 1 off which sets up for REV direction
      vPCF_ResetBits(CRANE_PIN1, CRANE_PORT); //pull low
      vPCF_SetBits(CRANE_PIN2, CRANE_PORT); //pull low
      break;
    }

  default:
    {
      vPCF_ResetBits(CRANE_PIN1, CRANE_PORT); //pull low
      vPCF_ResetBits(CRANE_PIN2, CRANE_PORT); //pull low
      break;
    }
  }
  //portEXIT_CRITICAL();

}
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
struct GenericMessage  xMessageA, xLastMessageA, xToSendA;
=======
>>>>>>> master
=======
struct GenericMessage  xMessageA, xLastMessageA, xToSendA;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
struct GenericMessage  xMessageA, xLastMessageA, xToSendA;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

void vTaskCrane(void * pvParameters)
{

  static struct GenericMessage * xMessage, *xLastMessage, * xToSend;


<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//  xLastMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
//  xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
//  xToSend = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
  xLastMessage = &xLastMessageA;
    xMessage = &xMessageA;
    xToSend = &xToSendA;


<<<<<<< HEAD
<<<<<<< HEAD
=======
  xLastMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
  xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
  xToSend = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  static int iComplete = 0;
  static int iStep = 0;
  xToSend->ucFromTask = CRANE_TASK;
  xToSend->ucToTask = BREW_TASK;
  xToSend->pvMessageContent = (void *)&iComplete;
  const int iTest = 40;
  static unsigned char ucDownIncrements = 0;
  uint8_t limit = 0xFF, limit1 = 0xFF; //neither on or off.
  static int iC = STOP;
  static int iCommandState = 0;
  char buf[40];
  xLastMessage->pvMessageContent = &iC;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  unsigned char failedI2cReads = 0;
  // testing vars
  char cCount = 0;
  char cLimitVal = 255;
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

  for (;;)
    {

      if(xQueueReceive(xCraneQueue, &xMessage, 50) != pdPASS)
        {
          xMessage->pvMessageContent = xLastMessage->pvMessageContent;
        }
      else // received successfully
        {
          iStep = xMessage->uiStepNumber;
          vConsolePrint("Crane Task has received a message\r\n");
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
          sprintf(buf, "xMessage = %d, Step:%d\r\n", *(int*)xMessage->pvMessageContent, xMessage->uiStepNumber);
          vConsolePrint(buf);
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
          xLastMessage->pvMessageContent = xMessage->pvMessageContent;
          xLastMessage->ucFromTask = xMessage->ucFromTask;
          iC = *((int*)xMessage->pvMessageContent); //casts to int * and then dereferences.
          iComplete = 0;
          xToSend->uiStepNumber = xMessage->uiStepNumber;
          iCommandState = 0;
          ucDownIncrements = 0;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c



//#ifdef TESTING
//      iCommandState = 1;
//      iComplete = STEP_COMPLETE;
//      vTaskDelay(1000);
//      {
//               vConsolePrint("Crane Command Complete, Sending Message\r\n");
//               vTaskDelay(50);
//               xToSend->pvMessageContent = (void*)&iTest;
//               xQueueSendToBack(xBrewTaskReceiveQueue, &xToSend, 1000);
//               //xQueueSendToBack(xBrewTaskReceiveQueue, (void *)40, 100);
//               iCommandState  = 0;
//               iCraneState = STOPPED;
//               iC = STOP;
//             }
//#endif



<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
        }

      switch(iCraneState)
      {
      case TOP:
        {
          if (iC == DN)
            {
              vCraneFunc(DN);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
              iCommandState = 0; // not done yet
=======
>>>>>>> master
=======
              iCommandState = 0; // not done yet
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
              iCommandState = 0; // not done yet
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              iCraneState = DRIVING_DOWN;
            }
          else if (iC == DN_INC)
            {
              iCraneState = DRIVING_DOWN_INC;
            }
          else if (iC == UP)
            {
              iCommandState = 1;
              iComplete = STEP_COMPLETE;
            }
          //iCommandState = 0;
          break;

        }
      case BOTTOM:
        {
          if (iC == UP)
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
             // vConsolePrint("Command = UP whilst at bottom\r\n");
              vCraneFunc(UP);
              iCraneState = DRIVING_UP;
              iCommandState = 0; // not done yet
            }
          else if (iC == DN)
            {
            //  vConsolePrint("Command = DN whilst at bottom\r\n");
<<<<<<< HEAD
=======
=======
             // vConsolePrint("Command = UP whilst at bottom\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              vCraneFunc(UP);
              iCraneState = DRIVING_UP;
              iCommandState = 0; // not done yet
            }
          else if (iC == DN)
            {
<<<<<<< HEAD
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
            //  vConsolePrint("Command = DN whilst at bottom\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              iCommandState = 1;
              iComplete = STEP_COMPLETE;
            }
          break;
        }
      case DRIVING_UP:
        {
          if (iC == STOP)
            {
              vCraneFunc(STOP);
              vConsolePrint("STOP while DU \r\n");
              iCraneState = STOPPED;
              iCommandState = 1;
              iComplete = STEP_COMPLETE;
            }
          else if (iC == DN)
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              vConsolePrint("Command = DN whilst driving up\r\n");
              limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);

              if (limit == 255) // read failed
                {
                   vTaskDelay(200);
                  limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
                }
<<<<<<< HEAD
<<<<<<< HEAD
=======
              limit = debounce(CRANE_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              if (limit != 1)
                {
                  vCraneFunc(DN);
                  iCraneState = DRIVING_DOWN;
                }
              else
                {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
                  iCraneState = BOTTOM;
                  iCommandState = 1;
                  iComplete = STEP_COMPLETE;
                }

            }
          else
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
              vPCF_SetBits(CRANE_UPPER_LIMIT_PIN, CRANE_UPPER_LIMIT_PORT); // yes the port and pin are reverse from read
              vTaskDelay(100);
=======
				vPCF_SetBits(CRANE_UPPER_LIMIT_PIN, CRANE_UPPER_LIMIT_PORT); // yes the port and pin are reverse from read
				vTaskDelay(100);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
				vPCF_SetBits(CRANE_UPPER_LIMIT_PIN, CRANE_UPPER_LIMIT_PORT); // yes the port and pin are reverse from read
				vTaskDelay(100);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              limit = cI2cGetInput(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
              //limit = debounce(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
              while (limit == 255) // read failed
                {
                  vTaskDelay(200);
                  limit = cI2cGetInput(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
                }

<<<<<<< HEAD
<<<<<<< HEAD
=======
              limit = debounce(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              if (limit == 1)
                {
                  vCraneFunc(STOP);
                  vConsolePrint("STOP limit DU \r\n");
                  iCraneState = TOP;
                  iComplete = STEP_COMPLETE;
                  iCommandState = 1;

                  //if (xMessage->ucFromTask == BREW_TASK)
                  //   xQueueSendToBack(xBrewTaskReceiveQueue, &xToSend, 0);
                }
            }
          break;
        }
      case DRIVING_DOWN:
        {
          if (iC == STOP)
            {
              vCraneFunc(STOP);
              vConsolePrint("STOP while DDN \r\n");
              iCraneState = STOPPED;
              iComplete = STEP_COMPLETE;
              iCommandState = 1;
            }
          else if (iC == UP)
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              vConsolePrint("Command = UP whilst driving down\r\n");
              //limit = debounce(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
              limit = cI2cGetInput(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
              if (limit == 255) // read failed
                {
                  vTaskDelay(200);
                  limit = cI2cGetInput(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
                }
<<<<<<< HEAD
<<<<<<< HEAD
=======
              limit = debounce(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              if (limit == 1)
                {
                  iCraneState = TOP;
                  iComplete = STEP_COMPLETE;
                  iCommandState = 1;
                }
              else
                {
                  vCraneFunc(UP);
                  iCraneState = DRIVING_UP;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
                  iCommandState = 0;
=======
>>>>>>> master
=======
                  iCommandState = 0;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
                  iCommandState = 0;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
                }

            }
          else// if (xMessage->cDirection == DN)
            {
              vConsolePrint("waiting on lower limit\r\n");
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
              if (limit == 255) // read failed
                {
                  vTaskDelay(200);
                  limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
                }
<<<<<<< HEAD
<<<<<<< HEAD
=======
              limit = debounce(CRANE_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              if (limit == 1)
                {
                  vTaskDelay(200);
                  vCraneFunc(STOP);
                  vConsolePrint("STOP limit DDN \r\n");
                  iCraneState = BOTTOM;
                  iCommandState = 1;
                  iComplete = STEP_COMPLETE;
                  //   if (xMessage->ucFromTask == BREW_TASK)
                  //    xQueueSendToBack(xBrewTaskReceiveQueue, &xToSend, 0);
                }
            }

          break;
        }
      case DRIVING_DOWN_INC:
        {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

          limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
//          cLimitVal = limit;
//                  while( cLimitVal  == 1 && cCount < 5)
//                    {
//                      limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
//                      cLimitVal = limit;
//                    }
          if (limit == ERROR) // read failed
            {
              vTaskDelay(200);
              limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
            }
          if(limit == FALSE)
<<<<<<< HEAD
<<<<<<< HEAD
=======
          limit = debounce(CRANE_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
          if(limit != 1)
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
            {
              vCraneFunc(DN);
              vConsolePrint("Crane DN \r\n");
              vTaskDelay(200);
              vCraneFunc(STOP);
              vConsolePrint("Crane STOP \r\n");
              vTaskDelay(1500);
              ucDownIncrements++;
            }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

          else if (limit == TRUE)
=======
          else
>>>>>>> master
=======

          else if (limit == TRUE)
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

          else if (limit == TRUE)
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
            {
              // Make sure we are down
              vCraneFunc(DN);
              vTaskDelay(100);

              vConsolePrint("STOP limit DDN \r\n");
              vCraneFunc(STOP);
              iCraneState = BOTTOM;
              iComplete = STEP_COMPLETE;
              iCommandState = 1;
              ucDownIncrements = 0;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
              vStir(STIR_STOPPED);
=======
              vStir(STOP);
>>>>>>> master
=======
              vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
              vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

              // if (xMessage->ucFromTask == BREW_TASK)
              //   xQueueSendToBack(xBrewTaskReceiveQueue, &xToSend, 0);

            }
          if (ucDownIncrements == 11)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
            vStir(STIR_DRIVING);
=======
            vStir(DRIVING);
>>>>>>> master
=======
            vStir(STIR_DRIVING);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
            vStir(STIR_DRIVING);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
          break;
        }
      case STOPPED:
        {
          if (iC == UP)
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              vConsolePrint("Crane: State = Stopped, Cmd = UP\r\n");
              limit = cI2cGetInput(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
              if (limit == 255) // read failed
                {
                  vConsolePrint("Crane I2cFail1\r\n");
                  vTaskDelay(200);
                  limit = cI2cGetInput(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
                }

              //limit = debounce(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
<<<<<<< HEAD
<<<<<<< HEAD
              if (limit != 1)
                {
                  iCommandState = 0;
=======
              limit = debounce(CRANE_UPPER_LIMIT_PORT, CRANE_UPPER_LIMIT_PIN);
              if (limit != 1)
                {
>>>>>>> master
=======
              if (limit != 1)
                {
                  iCommandState = 0;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
              if (limit != 1)
                {
                  iCommandState = 0;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
                  vCraneFunc(UP);
                  iCraneState = DRIVING_UP;
                }

              else
                {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
                  vConsolePrint("Crane: State = Stopped, Cmd = UP, Sensed top\r\n");
=======
>>>>>>> master
=======
                  vConsolePrint("Crane: State = Stopped, Cmd = UP, Sensed top\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
                  vConsolePrint("Crane: State = Stopped, Cmd = UP, Sensed top\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
                  iCommandState = 1;
                  iComplete = STEP_COMPLETE;
                  iCraneState = TOP;
                }
            }
          else if (iC == DN)
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              vConsolePrint("Crane: State = Stopped, Cmd = DN\r\n");
			  vPCF_SetBits(CRANE_LOWER_LIMIT_PIN, CRANE_LOWER_LIMIT_PORT); // yes the port and pin are reverse from read
			  vTaskDelay(100);
              limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
              if (limit == 255) // read failed
                {
                  vTaskDelay(200);
                  limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
                }
<<<<<<< HEAD
<<<<<<< HEAD
              if (limit != 1)
                {
                  iCommandState = 0;
=======
              limit = debounce(CRANE_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
              if (limit != 1)
                {
>>>>>>> master
=======
              if (limit != 1)
                {
                  iCommandState = 0;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
              if (limit != 1)
                {
                  iCommandState = 0;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
                  vCraneFunc(DN);
                  iCraneState = DRIVING_DOWN;
                }
              else
                {
                  iCommandState =1;
                  iComplete = STEP_COMPLETE;
                  iCraneState = BOTTOM;
                }
            }
          else if (iC == DN_INC)
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
				vPCF_SetBits(CRANE_LOWER_LIMIT_PIN, CRANE_LOWER_LIMIT_PORT); // yes the port and pin are reverse from read
				vTaskDelay(100);
              limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
              if (limit == 255) // read failed
                {
                  vTaskDelay(200);
                  limit = cI2cGetInput(CRANE_LOWER_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
                }
<<<<<<< HEAD
<<<<<<< HEAD
              if (limit != 1)
                {
                  iCommandState = 0;
=======
              limit = debounce(CRANE_LIMIT_PORT, CRANE_LOWER_LIMIT_PIN);
              if (limit != 1)
                {

>>>>>>> master
=======
              if (limit != 1)
                {
                  iCommandState = 0;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
              if (limit != 1)
                {
                  iCommandState = 0;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
                  iCraneState = DRIVING_DOWN_INC;
                }
              else
                {
                  iCommandState  = 1;
                  iComplete = STEP_COMPLETE;
                  iCraneState = BOTTOM;
                }
            }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
          iC = 14; // goes into undefined command state. shouldnt be needed.
=======
          iC = 14;
>>>>>>> master
=======
//          iC = 14; // goes into undefined command state. shouldnt be needed. 
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
//          iC = 14; // goes into undefined command state. shouldnt be needed. 
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

          break;
        }
      default:
        {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
			vConsolePrint("Default Case in Crane Function\r\n");
=======
>>>>>>> master
=======
			vConsolePrint("Default Case in Crane Function\r\n")
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
			vConsolePrint("Default Case in Crane Function\r\n")
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
          vCraneFunc(STOP); //stop the crane on an instant.
          iCraneState = STOPPED;
          break;

        }
      }// Switch
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
      if (iCommandState == 1 && xMessage->ucFromTask == BREW_TASK)
        {
          vConsolePrint("Crane Command Complete, Sending Message\r\n");
          vTaskDelay(50);
          xToSend->pvMessageContent = (void*)&iTest;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
          xQueueSendToBack(xBrewTaskReceiveQueue, &xToSend, 10000);
          //xQueueSendToBack(xBrewTaskReceiveQueue, (void *)40, 100);
          iCommandState  = 0;
         iCraneState = STOPPED;
=======
          xQueueSendToBack(xBrewTaskReceiveQueue, &xToSend, 1000);
          //xQueueSendToBack(xBrewTaskReceiveQueue, (void *)40, 100);
          iCommandState  = 0;
          iCraneState = STOPPED;
>>>>>>> master
=======
          xQueueSendToBack(xBrewTaskReceiveQueue, &xToSend, 10000);
          //xQueueSendToBack(xBrewTaskReceiveQueue, (void *)40, 100);
          iCommandState  = 0;
         iCraneState = STOPPED;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
          xQueueSendToBack(xBrewTaskReceiveQueue, &xToSend, 10000);
          //xQueueSendToBack(xBrewTaskReceiveQueue, (void *)40, 100);
          iCommandState  = 0;
         iCraneState = STOPPED;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
        }

      //vTaskDelay(500);
    }// Task Loop
}//func



void vCraneAppletDisplay( void *pvParameters){
  static char tog = 0; //toggles each loop
  for(;;)
    {

      xSemaphoreTake(xAppletRunningSemaphore, portMAX_DELAY); //take the semaphore so that the key handler wont
      //return to the menu system until its returned

      //display the state and user info (the state will flash on the screen)
      switch (iCraneState)
      {
      case DRIVING_UP:
        {
          if(tog)
            {
              lcd_fill(1,220, 180,29, Black);
              lcd_printf(1,13,15,"DRIVING UP");
            }
          else{
              lcd_fill(1,210, 180,17, Black);
          }
          break;
        }
      case DRIVING_DOWN:
        {
          if(tog)
            {
              lcd_fill(1,220, 180,29, Black);
              lcd_printf(1,13,15,"DRIVING DOWN");
            }
          else{
              lcd_fill(1,210, 180,17, Black);
          }
          break;
        }
      case TOP:
        {
          if(tog)
            {
              lcd_fill(1,210, 180,29, Black);
              lcd_printf(1,13,11,"TOP");
            }
          else
            {
              lcd_fill(1,210, 180,17, Black);
            }

          break;
        }
      case BOTTOM:
        {
          if(tog)
            {
              lcd_fill(1,210, 180,29, Black);
              lcd_printf(1,13,11,"BOTTOM");
            }
          else
            {
              lcd_fill(1,210, 180,17, Black);
            }

          break;
        }
      case STOPPED:
        {
          if(tog)
            {
              lcd_fill(1,210, 180,29, Black);
              lcd_printf(1,13,11,"STOPPED");
            }
          else
            {
              lcd_fill(1,210, 180,17, Black);
            }

          break;
        }
      default:
        {
          break;
        }
      }

      tog = tog ^ 1;

      xSemaphoreGive(xAppletRunningSemaphore); //give back the semaphore as its safe to return now.
      vTaskDelay(500);


    }
}

#define UP_X1 0
#define UP_Y1 30
#define UP_X2 150
#define UP_Y2 100
#define UP_W (UP_X2-UP_X1)
#define UP_H (UP_Y2-UP_Y1)

#define DN_X1 0
#define DN_Y1 105
#define DN_X2 150
#define DN_Y2 175
#define DN_W (DN_X2-DN_X1)
#define DN_H (DN_Y2-DN_Y1)

#define ST_X1 155
#define ST_Y1 30
#define ST_X2 300
#define ST_Y2 175
#define ST_W (ST_X2-ST_X1)
#define ST_H (ST_Y2-ST_Y1)

#define BAK_X1 200
#define BAK_Y1 190
#define BAK_X2 315
#define BAK_Y2 235
#define BAK_W (BAK_X2-BAK_X1)
#define BAK_H (BAK_Y2-BAK_Y1)

void vCraneApplet(int init){
  if (init)
    {
      lcd_DrawRect(UP_X1, UP_Y1, UP_X2, UP_Y2, Red);
      lcd_fill(UP_X1+1, UP_Y1+1, UP_W, UP_H, Blue);
      lcd_DrawRect(DN_X1, DN_Y1, DN_X2, DN_Y2, Red);
      lcd_fill(DN_X1+1, DN_Y1+1, DN_W, DN_H, Blue);
      lcd_DrawRect(ST_X1, ST_Y1, ST_X2, ST_Y2, Cyan);
      lcd_fill(ST_X1+1, ST_Y1+1, ST_W, ST_H, Red);
      lcd_DrawRect(BAK_X1, BAK_Y1, BAK_X2, BAK_Y2, Cyan);
      lcd_fill(BAK_X1+1, BAK_Y1+1, BAK_W, BAK_H, Magenta);
      lcd_printf(10,1,12,  "MANUAL CRANE");
      lcd_printf(8,4,2, "UP");
      lcd_printf(8,8,2, "DN");
      lcd_printf(26,6,4, "STOP");
      lcd_printf(30, 13, 4, "Back");

      xTaskCreate( vCraneAppletDisplay,
          ( signed portCHAR * ) "Crane_display",
          configMINIMAL_STACK_SIZE +500,
          NULL,
          tskIDLE_PRIORITY + 1,
          &xCraneAppletDisplayHandle );
    }

}


<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
struct GenericMessage  xMessage;
struct GenericMessage * pxMessage;
volatile int iUp= UP, iDn = DN, iStop = STOP;
=======

>>>>>>> master
=======
struct GenericMessage  xMessage;
struct GenericMessage * pxMessage;
volatile int iUp= UP, iDn = DN, iStop = STOP;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
struct GenericMessage  xMessage;
struct GenericMessage * pxMessage;
volatile int iUp= UP, iDn = DN, iStop = STOP;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
int iCraneKey(int xx, int yy){

  uint16_t window = 0;
  static uint8_t w = 5,h = 5;
  static uint16_t last_window = 0;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

  pxMessage = &xMessage;

=======
  static struct GenericMessage * pxMessage;
  pxMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
  int iUp= UP, iDn = DN, iStop = STOP;
>>>>>>> master
=======

  pxMessage = &xMessage;

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

  pxMessage = &xMessage;

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  pxMessage->pvMessageContent = &iStop;

  if (xx > UP_X1+1 && xx < UP_X2-1 && yy > UP_Y1+1 && yy < UP_Y2-1)
    {
      pxMessage->pvMessageContent = &iUp;
      xQueueSendToBack( xCraneQueue, &pxMessage, 0 );
    }
  else if (xx > DN_X1+1 && xx < DN_X2-1 && yy > DN_Y1+1 && yy < DN_Y2-1)
    {
      pxMessage->pvMessageContent = &iDn;
      xQueueSendToBack( xCraneQueue, &pxMessage, 0 );

    }
  else if (xx > ST_X1+1 && xx < ST_X2-1 && yy > ST_Y1+1 && yy < ST_Y2-1)
    {
      pxMessage->pvMessageContent = &iStop;
      xQueueSendToBack( xCraneQueue, &pxMessage, portMAX_DELAY );


    }
  else if (xx > BAK_X1 && yy > BAK_Y1 && xx < BAK_X2 && yy < BAK_Y2)
    {

      //try to take the semaphore from the display applet. wait here if we cant take it.
      xSemaphoreTake(xAppletRunningSemaphore, portMAX_DELAY);
      //delete the display applet task if its been created.
      if (xCraneAppletDisplayHandle != NULL)
        {
          vTaskDelete(xCraneAppletDisplayHandle);
          vTaskDelay(100);
          xCraneAppletDisplayHandle = NULL;
        }
      //return the semaphore for taking by another task.
      xSemaphoreGive(xAppletRunningSemaphore);
      return 1;

    }

  vTaskDelay(10);
  return 0;

}


