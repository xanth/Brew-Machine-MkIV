//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 14 Nov 2013
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
#include <stdlib.h>
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
#include <math.h>
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "lcd.h"
#include "task.h"
#include "adc.h"
#include "leds.h"
#include "semphr.h"
#include "queue.h"
#include "console.h"
#include "ds1820.h"
#include "brew.h"
#include "chiller_pump.h"
#include "mash_pump.h"
#include "valves.h"
#include "hlt.h"
#include "Flow1.h"
#include "crane.h"
#include "stir.h"
#include "mill.h"
#include "parameters.h"
#include "message.h"
#include "io_util.h"
#include "boil_valve.h"
#include "boil.h"
#include "hop_dropper.h"
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

#define MAX_BREW_STEPS iMaxBrewSteps()

// Applet States
#define MAIN 0
#define GRAPH 1
#define STATS 2
#define RES  3
#define QUIT 4
#define OTHER 5

// Brew Step Codes
#define BREW_STEP_BASE 40
#define BREW_STEP_COMPLETE 40
#define BREW_STEP_FAILED 41
#define BREW_STEP_WAIT 42

// Running States
#define IDLE 0
#define RUNNING 1

struct State BrewState = {0,0,0,0,0,0,0,0,0,0,0,0,0};

const int STEP_COMPLETE = 40;
const int STEP_FAILED = 41;
const int STEP_WAIT = 42;

const char  * pcRunningState[2] = {"IDLE", "RUNNING"};
const char  * pcHLTLevels[3] = {"LOW", "MID", "HIGH"};
const char  * pcHLTStates[4] = {"IDLE", "FILL+HEAT", "DRAIN", "AT TEMP"};
const char  * pcCraneStates[6] = {"Top", "Bottom", "Driving up", "Driving Down", "Down Inc", "Stopped"};
const char  * pcTASKS[6] = {"Brew", "Boil", "HLT", "Crane", "BoilValve", "HopDropper"};
const char  * pcBrewMessages[3] = {"Step Complete", "Step Failed", "Wait for previous steps"};



#define CLEAR_APPLET_CANVAS lcd_fill(CANVAS_X1+1,CANVAS_Y1+1, CANVAS_W, CANVAS_H, Black);
#define LCD_FLOAT( x, y, dp , var ) lcd_printf(x, y, 4, "%0d.%0d", (unsigned int)floor(var), (unsigned int)((var-floor(var))*pow(10, dp)));

#define DRAW_RESUME_BUTTONS lcd_DrawRect(BUTTON_4_X1, BUTTON_4_Y1, BUTTON_4_X2, BUTTON_4_Y2, Red); \
    lcd_fill(BUTTON_4_X1+1, BUTTON_4_Y1+1, BUTTON_4_W, BUTTON_4_H, Blue); \
    lcd_DrawRect(BUTTON_5_X1, BUTTON_5_Y1, BUTTON_5_X2, BUTTON_5_Y2, Red); \
    lcd_fill(BUTTON_5_X1+1, BUTTON_5_Y1+1, BUTTON_5_W, BUTTON_5_H, Blue); \
    lcd_DrawRect(BUTTON_6_X1, BUTTON_6_Y1, BUTTON_6_X2, BUTTON_6_Y2, Red); \
    lcd_fill(BUTTON_6_X1+1, BUTTON_6_Y1+1, BUTTON_6_W, BUTTON_6_H, Blue);

#define DRAW_RESUME_BUTTON_TEXT  lcd_printf(1,13,5, "UP"); \
    lcd_printf(9,13,5, "DOWN"); \
    lcd_printf(17,13,5, "RESUME"); //Button6

#define DRAW_MAIN_BUTTON_TEXT  lcd_printf(1,13,5, "START"); \
    lcd_printf(9,13,5, "PAUSE"); \
    lcd_printf(17,13,5, "SKIP"); //Button6




void vBrewAppletDisplay( void *pvParameters);
void vBrewGraphAppletDisplay(void * pvParameters);
void vBrewStatsAppletDisplay(void * pvParameters);
void vBrewResAppletDisplay(void * pvParameters);
void vBrewReset(void);
void vBrewApplet(int init);
void vBrewRunStep(void);
void vBrewNextStep(void);

xTaskHandle xBrewTaskHandle = NULL,
    xBrewAppletDisplayHandle = NULL,
    xBrewGraphAppletDisplayHandle = NULL,
    xBrewStatsAppletDisplayHandle = NULL,
    xBrewResAppletDisplayHandle = NULL;


// semaphore that stops the returning from the applet to the menu system until the applet goes into the blocked state.
xSemaphoreHandle xBrewAppletRunningSemaphore;
xQueueHandle xBrewTaskStateQueue = NULL, xBrewTaskReceiveQueue = NULL;
xQueueHandle xBrewAppletTextQueue = NULL;

static int iAppletState = MAIN;
static unsigned char ucResStep = 0;

// Define Structure for brew steps
static struct BrewStep
{
  const char * pcStepName;
  int (*func)(void * pvParameters);
  void (*poll)(void * pvParameters);
  int iFuncParams[5];
  uint16_t uTimeout;
  uint32_t uStartTime;
  uint16_t uElapsedTime;
  //uint16_t uTimeRemaining;
  unsigned char ucComplete; // step complete flag
  unsigned char ucWait; // Wait for previous steps to be complete?
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
} Brew[]; //Brew is an array of brew steps
=======
} Brew[], BrewClean[]; //Brew is an array of brew steps
>>>>>>> master
=======
} Brew[]; //Brew is an array of brew steps
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
} Brew[]; //Brew is an array of brew steps
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c


static int iBrewStepMonitor(int iWaiting)
{
  static char buf[50];
  int ii=0, jj=0, kk=0;
  int iPreviousStepsComplete;

  //vConsolePrint("STEP MONITOR\r\n");
  for (jj = 0;jj <= BrewState.ucStep;jj++)
    {
      //sprintf(buf,"checking step %d : state = %d\r\n",jj,Brew[jj].ucComplete);
      //vConsolePrint(buf);
      if (Brew[jj].ucComplete == 0)
        {

          kk++;
          Brew[jj].uElapsedTime = (BrewState.uSecondCounter - Brew[jj].uStartTime);

          if (jj != BrewState.ucStep)
            {
              sprintf(buf, "Waiting on Step %d:'%s' \r\n", jj, Brew[jj].pcStepName);
              vConsolePrint(buf);
            }

          if (Brew[jj].uElapsedTime > Brew[jj].uTimeout)
            {
              sprintf(buf, "Step %d: %s - TIMEOUT\r\n", jj, Brew[jj].pcStepName);
              vConsolePrint(buf);
            }
        }

    }
  iPreviousStepsComplete = kk;

  if (iWaiting)
    {
      //  sprintf(buf, "Returning %d\r\n=====================\r\n", iPreviousStepsComplete);
      //   vConsolePrint(buf);
      return iPreviousStepsComplete;
    }
  else
    // vConsolePrint("Returning 0\r\n");
    return 0;

}

static int iMaxBrewSteps(void)
{
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
  static char buf[50];
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  int i = 0;
  while (Brew[i].pcStepName != NULL)
    {
      i++;
    }
  //sprintf(buf, "Sizeof Brew[] = %d\r\n", i);
  //vConsolePrint(buf);
  return i;
}



void vBrewTotalReset(void){

  int iMax = iMaxBrewSteps();
  vBrewReset();
  vTaskDelete(xBrewTaskHandle);
  vQueueDelete(xBrewTaskReceiveQueue);
  vQueueDelete(xBrewTaskStateQueue);
  xBrewTaskHandle = NULL;
  xBrewTaskReceiveQueue = NULL;
  xBrewTaskStateQueue = NULL;
  for (int i = 0; i < iMax; i++)
    {
      Brew[i].ucComplete = 0;
      Brew[i].uElapsedTime = 0;
      Brew[i].uStartTime = 0;
    }

}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c


unsigned char ucGetBrewHoursElapsed()
{
  return BrewState.uHoursElapsed;
}

unsigned char ucGetBrewMinutesElapsed()
{
  return BrewState.uMinutesElapsed;
}

unsigned char ucGetBrewSecondsElapsed()
{
  return BrewState.uSecondsElapsed;
}

unsigned char ucGetBrewStepMinutesElapsed()
{
  return Brew[BrewState.ucStep].uElapsedTime/60;
}

unsigned char ucGetBrewStepSecondsElapsed()
{
  return Brew[BrewState.ucStep].uElapsedTime%60;
}

unsigned char ucGetBrewStep()
{
  return BrewState.ucStep;

}



<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//----------------------------------------------------------------------------------------------------------------------------
// Brew Task
//----------------------------------------------------------------------------------------------------------------------------
void vTaskBrew(void * pvParameters)
{
  portTickType xBrewStart;
  static uint16_t uBrewSecondsElapsed = 0, uBrewMinutesElapsed = 0, uBrewHoursElapsed=0;
  xBrewStart = xTaskGetTickCount();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//  char pcBrewElapsedTime[50], pcStepElapsedTime[50];
//  char pcBrewElapsedHours[45], pcBrewElapsedMinutes[45], pcBrewElapsedSeconds[45], pcBrewStep[45];
//  char pcBrewStepElapsedHours[45], pcBrewStepElapsedMinutes[45], pcBrewStepElapsedSeconds[45], pcMashTemp[45], pcHLTTemp[45];
////char pcStepRemainingTime[16];
<<<<<<< HEAD
<<<<<<< HEAD
  //uint32_t uStartingStep = (uint32_t)pvParameters;
  //char buf1[40];

=======
  char pcBrewElapsedTime[9], pcStepElapsedTime[16];
//char pcStepRemainingTime[16];
  //uint32_t uStartingStep = (uint32_t)pvParameters;
  //char buf1[40];
>>>>>>> master
=======
  //uint32_t uStartingStep = (uint32_t)pvParameters;
  //char buf1[40];

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  //uint32_t uStartingStep = (uint32_t)pvParameters;
  //char buf1[40];

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  BrewState.ucStep = 0;
  BrewState.uHoursElapsed = 0;
  BrewState.uMinutesElapsed = 0;
  BrewState.uSecondCounter = 0;
  BrewState.ucHLTState = HLT_STATE_IDLE;
  BrewState.ucHopAddition = 0;
  BrewState.ucRunningState = IDLE;
  BrewState.ucTarget = 0;
  BrewState.ucTargetHLTTemp = 0;
  BrewState.ucTargetHLTVolume = 0;
  BrewState.ucTargetWaterVolume = 0;
  BrewState.xLastWakeTime = xTaskGetTickCount();
  static uint8_t uMsg = IDLE;
  struct GenericMessage * xMessage;
  static int iContent = 0;
  static int iWaiting = 0, iStepsToComplete = 0;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
=======
  xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
  xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

  static char buf[50], buf1[50];

  for(;;)
    {
      if(xQueueReceive(xBrewTaskStateQueue, &uMsg, 0) == pdPASS)
        {
          BrewState.ucRunningState = uMsg;
          if (BrewState.ucRunningState == IDLE)
            sprintf(buf, "BrewState changing to IDLE\r\n");
          else
            sprintf(buf, "BrewState changing to RUNNING\r\n");
          vConsolePrint(buf);
          // if (uMsg == RUNNING && BrewState.ucStep == 0)
          //   vBrewRunStep();
        }
      switch(BrewState.ucRunningState)
      {
      case IDLE:
        {
          //vConsolePrint("Brew Idle\r\n");

          break;
        }
      case RUNNING:
        {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

          // Every thirty seconds, print the brew time and step time to the console.
          if ((BrewState.uSecondsElapsed % 15) == 0)
            {
            // this is where the print statements were for the console
            }
          if ((BrewState.uSecondsElapsed % 16) == 0)
            {

            }
<<<<<<< HEAD
=======
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
          // Every thirty seconds, print the brew time and step time to the console.
          if ((BrewState.uSecondsElapsed % 15) == 0)
            {
            // this is where the print statements were for the console
            }
          if ((BrewState.uSecondsElapsed % 16) == 0)
            {

<<<<<<< HEAD
>>>>>>> master
=======
            }
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

          // receive message from queue and if there is one, act on it.. (ie if the message is STEP COMPLETED.. then change to the next step.
          if(xQueueReceive(xBrewTaskReceiveQueue, &xMessage, 0) == pdPASS)
            {

              iContent = *(int*)xMessage->pvMessageContent; //casts to int * and then dereferences.
              sprintf(buf, "Brew Task: Msg from %s : %d : %u\r\n", pcTASKS[xMessage->ucFromTask- TASK_BASE], *(int *)xMessage->pvMessageContent, xMessage->uiStepNumber);
              vConsolePrint(buf);

              sprintf(buf1, "Brew Task: iContent = %s\r\n", pcBrewMessages[iContent-BREW_STEP_BASE]);
              vConsolePrint(buf1);

              switch(iContent)
              {
              case BREW_STEP_COMPLETE:
                {
                  Brew[xMessage->uiStepNumber].ucComplete = 1;
                  sprintf(buf, "Brew Task: Setting Step %d to Complete\r\n", xMessage->uiStepNumber);
                  vConsolePrint(buf);
                  break;
                }
              case BREW_STEP_FAILED:
                {

                  break;
                }
              case BREW_STEP_WAIT:
                {
                  vConsolePrint("WAITING COMMAND RECEIVED\r\n");
                  iWaiting = 1;

                }
              }
            }

          iStepsToComplete = iBrewStepMonitor(iWaiting); // updates elapsed times and monitors timeouts of steps.

          if (iWaiting) // are we waiting on all previous steps to be completed?
            {
              if (iStepsToComplete)// are there any steps to be completed?
                {
                  iWaiting = 1;
                }
              else // if not, we move on...
                {
                  iWaiting = 0;
                  BrewState.ucStep++;
                  vBrewRunStep();
                }
            }

          // If there is a polling function, run it.
          if (!iWaiting)
            {
              if (Brew[BrewState.ucStep].poll)
                Brew[BrewState.ucStep].poll((void *)Brew[BrewState.ucStep].iFuncParams);
            }
          //size_t heap;
          //heap = xPortGetFreeHeapSize();
          //vConsolePrint("HEAP REMAINING: %d\r\n", heap);
          // Update the counters
          BrewState.uSecondCounter++;
          BrewState.uSecondsElapsed++;
          if (BrewState.uSecondsElapsed == 60)
            {
              BrewState.uSecondsElapsed = 0;
              BrewState.uMinutesElapsed++;
            }

          if (BrewState.uMinutesElapsed == 60)
            {
              BrewState.uMinutesElapsed = 0;
              BrewState.uHoursElapsed++;
            }

          break;
        }
      }

      vTaskDelayUntil(&BrewState.xLastWakeTime, 1000 / portTICK_RATE_MS );

    }

}

//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
// BREW PAUSE
//----------------------------------------------------------------------------------------------------------------------------
void vBrewReset(void){
  static struct GenericMessage * xMessage = NULL;
  const int iCommand = 0; // changed to see if its more stable
  if (xMessage == NULL)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
=======
    xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  xMessage->ucFromTask = BREW_TASK_RESET;
  xMessage->ucToTask = BOIL_TASK;
  xMessage->pvMessageContent = (void *)&iCommand;
  if (xBoilQueue != NULL)
    xQueueSendToBack(xBoilQueue, &xMessage, 0);
  vChillerPump(STOP);
  vMashPump(STOP);
  vMill(MILL_STOPPED); // need to change state name.
  vValveActuate(HLT_VALVE, CLOSE);
  vValveActuate(INLET_VALVE, CLOSE);
  vValveActuate(MASH_VALVE, CLOSE);
  vValveActuate(CHILLER_VALVE, CLOSE);
}

void vBrewNextStep(void){
  static struct GenericMessage * xMessage = NULL;
  static int iCommand;
  static char buf[50], buf1[50];
  static int iCount = 0;
  iCount++;
  if (xMessage ==  NULL)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
=======
    xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  xMessage->ucFromTask = BREW_TASK;
  xMessage->ucToTask = BREW_TASK;
  xMessage->pvMessageContent = (void *)&iCommand;


  sprintf(buf, "Requesting next step: %d, %s\r\n", BrewState.ucStep+1, Brew[BrewState.ucStep+1].pcStepName);
  vConsolePrint(buf);
  if (BrewState.ucStep < MAX_BREW_STEPS && Brew[BrewState.ucStep+1].ucWait == 1)
    {
      iCommand = STEP_WAIT;
      sprintf(buf1, "NextStep:'%s' requires waiting\r\n", Brew[BrewState.ucStep].pcStepName);
      vConsolePrint(buf1);
      xQueueSendToBack(xBrewTaskReceiveQueue, &xMessage, 0);

    }
  else if (BrewState.ucStep < MAX_BREW_STEPS-1){
      BrewState.ucStep++;
  //    sprintf(buf1, "NextStep: %d, %s\r\n", BrewState.ucStep, Brew[BrewState.ucStep].pcStepName);
  //    vConsolePrint(buf1);
      vBrewRunStep();
  }
  else
    {
      vConsolePrint("Brew Finished!\r\n");
      BrewState.ucRunningState = IDLE;
    }
  sprintf(buf1, "NextStep: Count = %d\r\n", iCount);
  vConsolePrint(buf1);


}

void vBrewRunStep(void){
  int iFuncReturnValue = 0;
  static char buf[50];

  //vConsolePrint("vBrewRunStep Called\r\n");

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  //sprintf(buf, "RunStep: %d -> %s\r\n", BrewState.ucStep, Brew[BrewState.ucStep].pcStepName);
  //vConsolePrint(buf);
=======
  sprintf(buf, "RunStep: %d -> %s\r\n", BrewState.ucStep, Brew[BrewState.ucStep].pcStepName);
  vConsolePrint(buf);
>>>>>>> master
=======
  //sprintf(buf, "RunStep: %d -> %s\r\n", BrewState.ucStep, Brew[BrewState.ucStep].pcStepName);
  //vConsolePrint(buf);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  //sprintf(buf, "RunStep: %d -> %s\r\n", BrewState.ucStep, Brew[BrewState.ucStep].pcStepName);
  //vConsolePrint(buf);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

  vBrewReset();

  Brew[BrewState.ucStep].uElapsedTime = 0;
  Brew[BrewState.ucStep].uStartTime = BrewState.uSecondCounter;
  CLEAR_APPLET_CANVAS;

  if (Brew[BrewState.ucStep].func)
    Brew[BrewState.ucStep].func((void *)Brew[BrewState.ucStep].iFuncParams);

}


#define STRIKE 0
#define MASH_OUT 1
#define SPARGE 2
#define NO_MASH_OUT 3
#define CLEAN 4

//===================================================================================================================================================
void vBrewHLTSetupFunction(int piParameters[5]){
  static struct HLTMsg  * Content =  NULL;
  struct GenericMessage * Message = NULL;

  if (Content == NULL)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    Content = (struct HLTMsg *)pvPortMalloc(sizeof(struct HLTMsg));
  if (Message == NULL)
    Message = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
=======
    Content = (struct HLTMsg *)malloc(sizeof(struct HLTMsg));
  if (Message == NULL)
    Message = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
    Content = (struct HLTMsg *)pvPortMalloc(sizeof(struct HLTMsg));
  if (Message == NULL)
    Message = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
    Content = (struct HLTMsg *)pvPortMalloc(sizeof(struct HLTMsg));
  if (Message == NULL)
    Message = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

  switch( piParameters[0] )
  {
  case HLT_STATE_FILL_HEAT:
    {
      Content->uState = HLT_STATE_FILL_HEAT;
      switch( piParameters[1] )
      {
      case STRIKE:
        {
          Content->pcMsgText = "Strike";
          Content->uData1 = BrewParameters.fStrikeTemp;
          //xQueueSendToBack(xBrewTaskHLTQueue, &Content, portMAX_DELAY);
          break;
        }
      case MASH_OUT:
        {
          Content->pcMsgText = "Mash Out";
          Content->uData1 = BrewParameters.fMashOutTemp;
          //xQueueSendToBack(xBrewTaskHLTQueue, &Content, portMAX_DELAY);
          break;
        }
      case SPARGE:
        {
          Content->pcMsgText = "Sparge";
          Content->uData1 = BrewParameters.fSpargeTemp;
          //xQueueSendToBack(xBrewTaskHLTQueue, &Content, portMAX_DELAY);

          break;
        }
      case NO_MASH_OUT:
        {

          break;
        }
      case CLEAN:
        {
          Content->pcMsgText = "Clean";
          Content->uData1 = BrewParameters.fCleanTemp;


          break;
        }

      }
      break;
    }
  case HLT_STATE_DRAIN:
    {
      Content->uState = HLT_STATE_DRAIN;
      vResetFlow1();
      switch( piParameters[1] )
      {
      case STRIKE:
        {
          Content->pcMsgText = "Drain For Strike";
          Content->uData1 = BrewParameters.fStrikeLitres;
          //xQueueSendToBack(xBrewTaskHLTQueue, &Content, portMAX_DELAY);
          break;
        }
      case MASH_OUT:
        {
          Content->pcMsgText = "Drain For Mash Out";
          Content->uData1 = BrewParameters.fMashOutLitres;
          //xQueueSendToBack(xBrewTaskHLTQueue, &Content, portMAX_DELAY);
          break;
        }
      case SPARGE:
        {
          Content->pcMsgText = "Drain For Sparge";
          Content->uData1 = BrewParameters.fSpargeLitres;
          //xQueueSendToBack(xBrewTaskHLTQueue, &Content, portMAX_DELAY);
          break;
        }
      case NO_MASH_OUT:
        {

          break;
        }
      case CLEAN:
        {

          break;
        }

      }
      break;

    }
  }
  Message->ucFromTask = BREW_TASK;
  Message->ucToTask = HLT_TASK;
  Message->uiStepNumber = BrewState.ucStep;
  Message->pvMessageContent = Content;
  xQueueSendToBack(xBrewTaskHLTQueue, &Message, portMAX_DELAY);

  //xQueueSendToBack(xBrewTaskReceiveQueue, &STEP_COMPLETE, 0);
  vBrewNextStep();
}


void vBrewMillSetupFunction (int piParameters[5])
{

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  vMill(MILL_DRIVING);
=======
  vMill(DRIVING);
>>>>>>> master
=======
  vMill(MILL_DRIVING);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  vMill(MILL_DRIVING);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  vConsolePrint("Grain Mill Started\r\n");
  //xQueueSendToBack(xBrewTaskReceiveQueue, &STEP_COMPLETE, 0);
}

void vBrewValvesSetupFunction (int piParameters[5])
{
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  vValveActuate(HLT_VALVE, CLOSE);
  vValveActuate(MASH_VALVE, CLOSE);
=======

  vValveActuate(HLT_VALVE, CLOSE);
  vValveActuate(MASH_VALVE, CLOSE);

>>>>>>> master
=======
  vValveActuate(HLT_VALVE, CLOSE);
  vValveActuate(MASH_VALVE, CLOSE);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  vValveActuate(HLT_VALVE, CLOSE);
  vValveActuate(MASH_VALVE, CLOSE);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  vValveActuate(INLET_VALVE, CLOSE);
  vConsolePrint("Valves Setup Func Called\r\n");
  vTaskDelay(50);
  //xQueueSendToBack(xBrewTaskReceiveQueue, &STEP_COMPLETE, 0);
  Brew[BrewState.ucStep].ucComplete = 1;
  vBrewNextStep();
}

void vBrewBoilValveSetupFunction (int piParameters[5])
{
  static struct GenericMessage * xMessage = NULL;
  static int * iCommand;
  iCommand = &piParameters[0];
  if (xMessage == NULL)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
=======
    xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  xMessage->ucFromTask = BREW_TASK;
  xMessage->ucToTask = BOIL_VALVE_TASK;
  xMessage->uiStepNumber = BrewState.ucStep;
  xMessage->pvMessageContent = (void *)iCommand;
  vConsolePrint("Boil Valve Setup Function called\r\n");
  xQueueSendToBack(xBoilValveQueue, &xMessage, 0);
  vBrewNextStep();
}

void vBrewCraneSetupFunction(int piParameters[5])
{
  static struct GenericMessage * xMessage = NULL;
  static int * iCommand;
  iCommand = &piParameters[0];
  if (xMessage == NULL)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
=======
    xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  xMessage->ucFromTask = BREW_TASK;
  xMessage->ucToTask = CRANE_TASK;
  xMessage->uiStepNumber = BrewState.ucStep;
  xMessage->pvMessageContent = (void *)iCommand;
  // piParameters[0] sets the state to send to the crane task.
  // check to see if we are driving down.. if so , stir
  //if (piParameters[0] == DN_INC)
  //  vStir(DRIVING);

  // start the crane.
  int i = xQueueSendToBack(xCraneQueue, &xMessage, 0);

  vBrewNextStep();
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
struct GenericMessage xMessage2;
struct GenericMessage * xMessage1;
=======

>>>>>>> master
=======
struct GenericMessage xMessage2;
struct GenericMessage * xMessage1;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
struct GenericMessage xMessage2;
struct GenericMessage * xMessage1;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
// PRE-CHILL setup function.
//--------------------------
// Makes sure the boil valve is closed and turns on the pump.
void vBrewPreChillSetupFunction(int piParameters[5])
{
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
 // vConsolePrint("Pre-Chill setup Function called\r\n");
  //new code to stop the boil
//
  int ii;
   const int iCommand1 = 0; // changed to see if its more stable
   xMessage1 = &xMessage2;
   //if (xMessage1 == NULL)
     //xMessage1 = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
   xMessage1->ucFromTask = BREW_TASK_RESET;
   xMessage1->ucToTask = BOIL_TASK;
   xMessage1->pvMessageContent = (void *)&iCommand1;
   if (xBoilQueue != NULL){
       xQueueSendToBack(xBoilQueue, &xMessage1, 5000);
       vConsolePrint("Stopping Boil with duty cycle 0\r\n");
   }
vTaskDelay(2000);
  //-------------------------------------------------
   static struct GenericMessage * xMessage = NULL;
    const int iCommand = CLOSE;
    xMessage = &xMessage2;
//    if (xMessage ==  NULL)
//      xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
<<<<<<< HEAD
=======

=======
 // vConsolePrint("Pre-Chill setup Function called\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  //new code to stop the boil
//
  int ii;
   const int iCommand1 = 0; // changed to see if its more stable
   xMessage1 = &xMessage2;
   //if (xMessage1 == NULL)
     //xMessage1 = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
   xMessage1->ucFromTask = BREW_TASK_RESET;
   xMessage1->ucToTask = BOIL_TASK;
   xMessage1->pvMessageContent = (void *)&iCommand1;
   if (xBoilQueue != NULL){
       xQueueSendToBack(xBoilQueue, &xMessage1, 5000);
       vConsolePrint("Stopping Boil with duty cycle 0\r\n");
   }
vTaskDelay(2000);
  //-------------------------------------------------
   static struct GenericMessage * xMessage = NULL;
    const int iCommand = CLOSE;
<<<<<<< HEAD
    if (xMessage ==  NULL)
      xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
    xMessage = &xMessage2;
//    if (xMessage ==  NULL)
//      xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    xMessage->ucFromTask = CHILL_TASK;
    xMessage->ucToTask = BOIL_VALVE_TASK;
    xMessage->uiStepNumber = BrewState.ucStep;
    xMessage->pvMessageContent = (void *)&iCommand;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
    vConsolePrint("Boil Valve Setup Function called\r\n");
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    xQueueSendToBack(xBoilValveQueue, &xMessage, 1000); // Make sure the boil valve is closed.

    for (ii = 0; ii < BrewParameters.uiChillerPumpPrimingCycles; ii++)
       // try to prime, if not already primed
       {
         vChillerPump(PUMPING);
         vTaskDelay(BrewParameters.uiChillerPumpPrimingTime*1000);
         vChillerPump(STOPPED);
         vTaskDelay(BrewParameters.uiChillerPumpPrimingTime*1000);
       }

    vChillerPump(PUMPING); //Pump.


}

// CHILL setup function.
//--------------------------
// Opens the boil valve, Opens the chiller valve and turns on the pump.
void vBrewChillSetupFunction(int piParameters[5])
{
  static struct GenericMessage * xMessage = NULL;
  const int iCommand = OPEN;
  if (xMessage ==  NULL)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
=======
    xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
    xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  xMessage->ucFromTask = CHILL_TASK;
  xMessage->ucToTask = BOIL_VALVE_TASK;
  xMessage->uiStepNumber = BrewState.ucStep;
  xMessage->pvMessageContent = (void *)&iCommand;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  vConsolePrint("Chill Setup Function called\r\n");
=======
  vConsolePrint("Boil Valve Setup Function called\r\n");
>>>>>>> master
=======
  vConsolePrint("Chill Setup Function called\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  vConsolePrint("Chill Setup Function called\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  xQueueSendToBack(xBoilValveQueue, &xMessage, 1000); // open boil valve to direct wort through chiller
  vTaskDelay(50);
  vChillerPump(PUMPING); //Pump.
  vValveActuate(CHILLER_VALVE, OPEN); // Runs water through the other side of the chiller.


}

// PRE-CHILL polling function
//----------------------------
// waits for elapsed time, stops the pump, moves on
void vBrewPreChillPollFunction(int piParameters[5])
{
//  char mes[32];
//   sprintf(mes, "settling recirc time = %u", BrewParameters.uiSettlingRecircTime*60);
//   vConsolePrint(mes);
  //Brew[BrewState.ucStep].uTimeRemaining = (BrewParameters.uiSettlingTime*60) - Brew[BrewState.ucStep].uElapsedTime;
  if (Brew[BrewState.ucStep].uElapsedTime >= BrewParameters.uiSettlingRecircTime*60)
    {
      vChillerPump(STOPPED);
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  if (Brew[BrewState.ucStep].uElapsedTime >= BrewParameters.uiSettlingTime*60)
    {
      vChillerPump(STOPPED); // just in case
      Brew[BrewState.ucStep].ucComplete = 1;
      vBrewNextStep();

    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//#ifdef TESTING
//
//  if (Brew[BrewState.ucStep].uElapsedTime >= 3)
//    {
//      vChillerPump(STOPPED); // just in case
//      Brew[BrewState.ucStep].ucComplete = 1;
//      vBrewNextStep();
//
//    }
//
//  #endif


<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

}

// CHILL Polling function
void vBrewChillPollFunction(int piParameters[5])
{
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  static struct GenericMessage * xMessage = NULL;
      const int iCommand = CLOSE;


<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    //Brew[BrewState.ucStep].uTimeRemaining = (BrewParameters.uiChillTime*60) - Brew[BrewState.ucStep].uElapsedTime;
  if (Brew[BrewState.ucStep].uElapsedTime >= BrewParameters.uiChillTime*60)
    {
      vChillerPump(STOPPED);
      vValveActuate(CHILLER_VALVE, CLOSE);
      Brew[BrewState.ucStep].ucComplete = 1;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
      if (xMessage ==  NULL)
          xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
        xMessage->ucFromTask = CHILL_TASK;
        xMessage->ucToTask = BOIL_VALVE_TASK;
        xMessage->uiStepNumber = BrewState.ucStep;
        xMessage->pvMessageContent = (void *)&iCommand;
        xQueueSendToBack(xBoilValveQueue, &xMessage, 1000); // Make sure the boil valve is closed.
<<<<<<< HEAD
<<<<<<< HEAD
      vBrewNextStep();

    }

//#ifdef TESTING
//  if (Brew[BrewState.ucStep].uElapsedTime >= 2)
//      {
//        vChillerPump(STOPPED);
//        vValveActuate(CHILLER_VALVE, CLOSE);
//        Brew[BrewState.ucStep].ucComplete = 1;
//        if (xMessage ==  NULL)
//            xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
//          xMessage->ucFromTask = CHILL_TASK;
//          xMessage->ucToTask = BOIL_VALVE_TASK;
//          xMessage->uiStepNumber = BrewState.ucStep;
//          xMessage->pvMessageContent = (void *)&iCommand;
//          xQueueSendToBack(xBoilValveQueue, &xMessage, 1000); // Make sure the boil valve is closed.
//        vBrewNextStep();
//
//      }
//#endif


=======
      vBrewNextStep();

    }
>>>>>>> master
=======
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
      vBrewNextStep();

    }

//#ifdef TESTING
//  if (Brew[BrewState.ucStep].uElapsedTime >= 2)
//      {
//        vChillerPump(STOPPED);
//        vValveActuate(CHILLER_VALVE, CLOSE);
//        Brew[BrewState.ucStep].ucComplete = 1;
//        if (xMessage ==  NULL)
//            xMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
//          xMessage->ucFromTask = CHILL_TASK;
//          xMessage->ucToTask = BOIL_VALVE_TASK;
//          xMessage->uiStepNumber = BrewState.ucStep;
//          xMessage->pvMessageContent = (void *)&iCommand;
//          xQueueSendToBack(xBoilValveQueue, &xMessage, 1000); // Make sure the boil valve is closed.
//        vBrewNextStep();
//
//      }
//#endif


<<<<<<< HEAD
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
}



//uint8_t uNextStep = MSG_STEP_COMPLETE;

void vBrewMillPollFunction (int piParameters[5])
{
  int iGrindTime = BrewParameters.iGrindTime*60;
  //Brew[BrewState.ucStep].uTimeRemaining = (BrewParameters.iGrindTime*60) - Brew[BrewState.ucStep].uElapsedTime;
  if (Brew[BrewState.ucStep].uElapsedTime >= iGrindTime)
    {
      vMill(STOPPED);
      Brew[BrewState.ucStep].ucComplete = 1;
      vBrewNextStep();
      //xQueueSendToBack(xBrewTaskReceiveQueue, &STEP_COMPLETE, 0);
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//#ifdef TESTING
//  if (Brew[BrewState.ucStep].uElapsedTime >= 2)
//      {
//        vMill(STOPPED);
//        Brew[BrewState.ucStep].ucComplete = 1;
//        vBrewNextStep();
//        //xQueueSendToBack(xBrewTaskReceiveQueue, &STEP_COMPLETE, 0);
//      }
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
void vBrewWaitingPollFunction(int piParameters[5])
{
  if (Brew[BrewState.ucStep].uElapsedTime >= piParameters[0])
    {
      Brew[BrewState.ucStep].ucComplete = 1;
      vBrewNextStep();
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

#ifdef TESTING

  if (Brew[BrewState.ucStep].uElapsedTime >= 2)
    {
      Brew[BrewState.ucStep].ucComplete = 1;
      vBrewNextStep();
    }

#endif
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  // else
  //   vConsolePrint("Waiting: Poll Function\r\n");

}
#define MASH_STAGE_1 1
#define MASH_STAGE_2 2
#define MASH_STAGE_3 3
#define MASH_STAGE_4 4

static int iStirState = MASH_STAGE_1;
static int iPumpState = MASH_STAGE_1;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
volatile  int iMashTime;
volatile  int iStirTime1;
volatile  int iStirTime2;
volatile  int iPumpTime1;
volatile  int iPumpTime2;



void vBrewMashOutSetupFunction(int piParameters[5])
{
  iMashTime = BrewParameters.iMashOutTime * 60;
    iStirTime1 = BrewParameters.iMashOutStirTime1 * 60;
    iStirTime2 = BrewParameters.iMashOutStirTime2 * 60;
    iPumpTime1 = BrewParameters.iMashOutPumpTime1 * 60;
    iPumpTime2 = BrewParameters.iMashOutPumpTime2 * 60;
//#ifdef TESTING
//  iMashTime = 20; //seconds
//    iStirTime1 = 5;
//    iStirTime2 = 5;
//    iPumpTime1 = 5;
//    iPumpTime2 = 5;
//#endif




}


void vBrewSpargeSetupFunction(int piParameters[5])
{

  iMashTime = BrewParameters.iSpargeTime * 60;
  iStirTime1 = BrewParameters.iSpargeStirTime1 * 60;
  iStirTime2 = BrewParameters.iSpargeStirTime2 * 60;
  iPumpTime1 = BrewParameters.iSpargePumpTime1 * 60;
  iPumpTime2 = BrewParameters.iSpargePumpTime2 * 60;
//#ifdef TESTING
//  iMashTime = 20; //seconds
//    iStirTime1 = 5;
//    iStirTime2 = 5;
//    iPumpTime1 = 5;
//    iPumpTime2 = 5;
//#endif


}


<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  void vBrewMashSetupFunction(int piParameters[5])
  {
    iStirState = MASH_STAGE_1;
    iPumpState = MASH_STAGE_1;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    iMashTime = BrewParameters.iMashTime * 60;
    iStirTime1 = BrewParameters.iStirTime1 * 60;
    iStirTime2 = BrewParameters.iStirTime2 * 60;
    iPumpTime1 = BrewParameters.iPumpTime1 * 60;
    iPumpTime2 = BrewParameters.iPumpTime2 * 60;
//#ifdef TESTING
//  iMashTime = 20; //seconds
//    iStirTime1 = 5;
//    iStirTime2 = 5;
//    iPumpTime1 = 5;
//    iPumpTime2 = 5;
//#endif

    //open close to get air out of pump.
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    int iCycles = 2;
    int ii;
    for (ii = 0; ii < iCycles; ii++)
      {
        vValveActuate(MASH_VALVE, OPEN);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
        vTaskDelay(4000);
        vValveActuate(MASH_VALVE, CLOSED);
        vTaskDelay(4000);
      }
  }


void vBrewMashPollFunction(int piParameters[5])
{
<<<<<<< HEAD

  int iTimeRemaining = iMashTime;
=======
        vTaskDelay(2000);
=======
        vTaskDelay(4000);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
        vValveActuate(MASH_VALVE, CLOSED);
        vTaskDelay(4000);
      }
  }


void vBrewMashPollFunction(int piParameters[5])
{

<<<<<<< HEAD
   iMashTime = BrewParameters.iMashTime * 60;
   iStirTime1 = BrewParameters.iStirTime1 * 60;
   iStirTime2 = BrewParameters.iStirTime2 * 60;
   iPumpTime1 = BrewParameters.iPumpTime1 * 60;
   iPumpTime2 = BrewParameters.iPumpTime2 * 60;
>>>>>>> master
=======
  int iTimeRemaining = iMashTime;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

  int iTimeRemaining = iMashTime;
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
   iTimeRemaining = iMashTime;

  static char buf[50];
  static int flag = 0;
  iTimeRemaining = iMashTime - Brew[BrewState.ucStep].uElapsedTime;
 // Brew[BrewState.ucStep].uTimeRemaining = iTimeRemaining;
  if (flag == 0){
      sprintf(buf, "Remaining: %d, Elapsed: %d, p1: %d, p2: %d, s1 %d, s2 %d\r\n", iTimeRemaining/60, Brew[BrewState.ucStep].uElapsedTime/60, iPumpTime1/60, iPumpTime2/60, iStirTime1/60, iStirTime2/60);
      vConsolePrint(buf);
      flag = 1;
  }
  switch (iPumpState)
  {
  case MASH_STAGE_1:
    {
      if ((Brew[BrewState.ucStep].uElapsedTime < iPumpTime1))
          {
            vMashPump(PUMPING);
            iPumpState = MASH_STAGE_2;
            flag =0;
            vConsolePrint("MashPoll1: Pumping\r\n");
          }
      if (iPumpTime1 == 0)
          {
            vMashPump(STOPPED);
            iPumpState = MASH_STAGE_2;
            flag = 0;
            vConsolePrint("MashPoll1: Pump Time 1 = 0, not pumping\r\n");
          }
      break;
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  case MASH_STAGE_2: //blah

=======
  case MASH_STAGE_2:
>>>>>>> master
=======
  case MASH_STAGE_2: //blah

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  case MASH_STAGE_2: //blah

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    {
      if ((Brew[BrewState.ucStep].uElapsedTime > iPumpTime1))
        {
          vMashPump(STOPPED);
          iPumpState = MASH_STAGE_3;
          flag = 0;
          vConsolePrint("MashPoll2: Stopped\r\n");
        }
      break;
    }
  case MASH_STAGE_3:
      {
        if (iTimeRemaining < iPumpTime2)
          {
            vMashPump(PUMPING);
            iPumpState = MASH_STAGE_4;
            flag = 0;
            vConsolePrint("MashPoll3: Pumping\r\n");
          }
        if (iPumpTime2 == 0)
          {
            vMashPump(STOPPED);
            iPumpState = MASH_STAGE_4;
            flag =0;
            vConsolePrint("MashPoll3: Pump Time 2 = 0, not pumping\r\n");
          }
        break;

      }
  case MASH_STAGE_4:
    {
      flag =0;
      break;
    }
  }

    {
      switch (iStirState)
      {
      case MASH_STAGE_1:
        {
          if ((Brew[BrewState.ucStep].uElapsedTime > 30) && (Brew[BrewState.ucStep].uElapsedTime < iStirTime1))
            {
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
              iStirState = MASH_STAGE_2;
              flag =0;
              vConsolePrint("MashPoll1: Stirring\r\n");
            }
          if (iStirTime1 == 0)
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
              vStir(STIR_STOPPED);
=======
              vStir(STOPPED);
>>>>>>> master
=======
              vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
              vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              iStirState = MASH_STAGE_2;
              flag =0;
              vConsolePrint("MashPoll1: Stir Time 1 = 0, not Stirring\r\n");
            }
          break;
        }
      case MASH_STAGE_2:
        {
          if ((Brew[BrewState.ucStep].uElapsedTime > iStirTime1))
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
              vStir(STIR_STOPPED);
=======
              vStir(STOPPED);
>>>>>>> master
=======
              vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
              vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              iStirState = MASH_STAGE_3;
              flag =0;
              vConsolePrint("MashPoll2: Stopped Stirring\r\n");
            }
          break;
        }
      case MASH_STAGE_3:
        {
          if (iTimeRemaining < iStirTime2)
            {
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
              iStirState = MASH_STAGE_4;
              flag =0;
              vConsolePrint("MashPoll3: Stirring\r\n");
            }
          if (iStirTime2 == 0)
            {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
              vStir(STIR_STOPPED);
=======
              vStir(STOPPED);
>>>>>>> master
=======
              vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
              vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
              iStirState = MASH_STAGE_4;
              flag =0;
              vConsolePrint("MashPoll3: Stir Time 2 = 0, not Stirring\r\n");
            }
          break;

        }
      case MASH_STAGE_4:
        {
          flag =0;
          break;
        }
      }
    }

  if (Brew[BrewState.ucStep].uElapsedTime >= iMashTime)
    {
      vMashPump(STOPPED);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
      vStir(STIR_STOPPED);
=======
      vStir(STOPPED);
>>>>>>> master
=======
      vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
      vStir(STIR_STOPPED);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
      iStirState = MASH_STAGE_1;
      iPumpState = MASH_STAGE_1;
      Brew[BrewState.ucStep].ucComplete = 1;
      vBrewNextStep();
    }


}


void vBrewPumpToBoilSetupFunction(int piParameters[5])
{
  //int iIterations = piParameters[0], ii;
  int ii = 0;
  vValveActuate(MASH_VALVE, OPEN);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  for (ii = 0; ii < BrewParameters.iPumpPrimingCycles; ii++)
    // try to prime, if not already primed
    {
      vMashPump(PUMPING);
      vTaskDelay(BrewParameters.iPumpPrimingTime*1000);
      vMashPump(STOPPED);
      vTaskDelay(BrewParameters.iPumpPrimingTime*1000);
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
}

void vBrewPumpToBoilPollFunction(int  piParameters[5])
{
  int iPumpToBoilTime = piParameters[1];
  vMashPump(PUMPING);
  vValveActuate(MASH_VALVE, OPEN);
  //Brew[BrewState.ucStep].uTimeRemaining = (piParameters[1]*60) - Brew[BrewState.ucStep].uElapsedTime;
  if (Brew[BrewState.ucStep].uElapsedTime >= iPumpToBoilTime)
    {
      Brew[BrewState.ucStep].ucComplete = 1;
      vMashPump(STOPPED);
      vValveActuate(MASH_VALVE, CLOSE);
      vBrewNextStep();
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
#ifdef TESTING
  if (Brew[BrewState.ucStep].uElapsedTime >= 3)
      {
        Brew[BrewState.ucStep].ucComplete = 1;
        vMashPump(STOPPED);
        vValveActuate(MASH_VALVE, CLOSE);
        vBrewNextStep();
      }
#endif

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

}
#define BOIL_0 0
#define BOIL_1 1
#define BOIL_2 2
#define BOIL_3 3
#define BOIL_4 4
#define BOIL_5 5
#define BOIL_6 6
#define BOIL_7 7
static int iBoilState = BOIL_0;
static struct GenericMessage * xBoilMessage = NULL;
static struct TextMsg * xBoilTextMessage = NULL;
void vBrewBoilSetupFunction(int piParameters[5])
{
  const int i100 = 100;
  if (xBoilMessage == NULL)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    xBoilMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
  if (xBoilTextMessage == NULL)
    xBoilTextMessage = (struct TextMsg *)pvPortMalloc(sizeof(struct TextMsg));
=======
    xBoilMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
  if (xBoilTextMessage == NULL)
    xBoilTextMessage = (struct TextMsg *)malloc(sizeof(struct TextMsg));
>>>>>>> master
=======
    xBoilMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
  if (xBoilTextMessage == NULL)
    xBoilTextMessage = (struct TextMsg *)pvPortMalloc(sizeof(struct TextMsg));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
    xBoilMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
  if (xBoilTextMessage == NULL)
    xBoilTextMessage = (struct TextMsg *)pvPortMalloc(sizeof(struct TextMsg));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c


  if (piParameters[2] == 1)
    {
      iBoilState = BOIL_0;
      xBoilMessage->pvMessageContent = (void *)&piParameters[1];
      xBoilMessage->ucFromTask = BREW_TASK;
      xBoilTextMessage->pcMsgText = "BOIL";
      xBoilTextMessage->ucLine = 5;
      xQueueSendToBack(xBrewAppletTextQueue, &xBoilTextMessage, 0);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
      vValveActuate(MASH_VALVE, CLOSE); // Runs water through the other side of the chiller.
=======
>>>>>>> master
=======
      vValveActuate(MASH_VALVE, CLOSE); // Runs water through the other side of the chiller.
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
      vValveActuate(MASH_VALVE, CLOSE); // Runs water through the other side of the chiller.
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    }
  else
    {
      xBoilMessage->pvMessageContent = (void *)100;
      xBoilTextMessage->pcMsgText = "Bring To Boil";
      xBoilMessage->ucFromTask = BREW_TASK_BRING_TO_BOIL;
           xBoilTextMessage->ucLine = 5;
           xQueueSendToBack(xBrewAppletTextQueue, &xBoilTextMessage, 0);
      iBoilState = BOIL_7;
    }



  xBoilMessage->ucToTask = BOIL_TASK;
  xBoilMessage->uiStepNumber = BrewState.ucStep;
  vConsolePrint("Boil Setup Function called\r\n");
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  xQueueSendToBack(xBoilQueue, &xBoilMessage, 5000);
=======
  xQueueSendToBack(xBoilQueue, &xBoilMessage, 0);
>>>>>>> master
=======
  xQueueSendToBack(xBoilQueue, &xBoilMessage, 5000);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  xQueueSendToBack(xBoilQueue, &xBoilMessage, 5000);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

}

void vBrewBoilPollFunction(int piParameters[5])
{

  int iBoilTime = BrewParameters.uiBoilTime*60;
  int iBringToBoilTime = BrewParameters.uiBringToBoilTime*60;
  int iBoilDuty = piParameters[1];
  int iTimeRemaining = ((iBoilTime - Brew[BrewState.ucStep].uElapsedTime)/60);
  int iBringTimeRemaining= ((iBringToBoilTime - Brew[BrewState.ucStep].uElapsedTime)/60);
  static int iLastTime;
  static char buf[50];

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//#ifdef TESTING
//  iBringToBoilTime = 1;
//  iBoilTime = 10;
//  vTaskDelay(1000);
//  iBoilDuty = 0;
//         xBoilMessage->pvMessageContent = (void*)iBoilDuty;
//         Brew[BrewState.ucStep].ucComplete = 1;
//         xQueueSendToBack(xBoilQueue, &xBoilMessage, 0);
//         vConsolePrint("Boil Completed, sent duty 0 to boil\r\n");
//         vBrewNextStep();
//#endif
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  //===================================================================================
  //added 14/11/2014 ... sends a message every second so the brew task can check the ADC
  //not ideal, but will do for now.
  if(xBoilMessage == NULL)
    {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
      xBoilMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
=======
      xBoilMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));
>>>>>>> master
=======
      xBoilMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
      xBoilMessage = (struct GenericMessage *)pvPortMalloc(sizeof(struct GenericMessage));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
      vConsolePrint("Had to allocate memory for xBoilMessage!!!\r\n");
    }
  if (iBoilState != BOIL_7)
    {
      xBoilMessage->pvMessageContent = (void *)&piParameters[1];
      xBoilMessage->ucFromTask = BREW_TASK;
      xBoilMessage->ucToTask = BOIL_TASK;
      xBoilMessage->uiStepNumber = BrewState.ucStep;
      xQueueSendToBack(xBoilQueue, &xBoilMessage, 0);
    }
  //===================================================================================


  //  xBoilMessage->pvMessageContent = (void *)&iBoilDuty;
  //static char buf[50];
  //Brew[BrewState.ucStep].uTimeRemaining = (BrewParameters.uiBoilTime*60) - Brew[BrewState.ucStep].uElapsedTime;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  //if (iTimeRemaining != iLastTime)
=======
  if (iTimeRemaining != iLastTime)
>>>>>>> master
=======
  //if (iTimeRemaining != iLastTime)
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  //if (iTimeRemaining != iLastTime)
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    {
      sprintf(buf, "Time Remaining: %d\r\n State = %d\r\n", iTimeRemaining, iBoilState);
      vConsolePrint(buf);
      iLastTime = iTimeRemaining;
    }
  switch (iBoilState)
  {
  case BOIL_0:
    {
      if (iTimeRemaining <   BrewParameters.uiHopTimes[0])
        {
          xQueueSendToBack(xHopsQueue, (void *)1,0);
          vConsolePrint("Hops Delivered 0\r\n");
          xBoilTextMessage->pcMsgText = "HOPS 0 DELIVERED";
          xBoilTextMessage->ucLine = 6;
          xQueueSendToBack(xBrewAppletTextQueue, &xBoilTextMessage, 0);
          iBoilState = BOIL_1;
        }
      break;
    }
  case BOIL_1:
    {
      if (iTimeRemaining <   BrewParameters.uiHopTimes[1])
        {
          xQueueSendToBack(xHopsQueue, (void *)1,0);
          vConsolePrint("Hops Delivered 1\r\n");
          xBoilTextMessage->pcMsgText = "HOPS 1 DELIVERED";
          xBoilTextMessage->ucLine = 6;
          xQueueSendToBack(xBrewAppletTextQueue, &xBoilTextMessage, 0);
          iBoilState = BOIL_2;
        }
      break;
    }
  case BOIL_2:
    {
      if (iTimeRemaining <   BrewParameters.uiHopTimes[2])
        {
          xQueueSendToBack(xHopsQueue, (void *)1,0);
          vConsolePrint("Hops Delivered 2\r\n");
          xBoilTextMessage->pcMsgText = "HOPS 2 DELIVERED";
                   xBoilTextMessage->ucLine = 6;
                   xQueueSendToBack(xBrewAppletTextQueue, &xBoilTextMessage, 0);

          iBoilState = BOIL_3;
        }
      break;
    }
  case BOIL_3:
    {
      if (iTimeRemaining <   BrewParameters.uiHopTimes[3])
        {
          xQueueSendToBack(xHopsQueue, (void *)1,0);
          vConsolePrint("Hops Delivered 3\r\n");
          xBoilTextMessage->pcMsgText = "HOPS 3 DELIVERED";
                   xBoilTextMessage->ucLine = 6;
                   xQueueSendToBack(xBrewAppletTextQueue, &xBoilTextMessage, 0);

          iBoilState = BOIL_4;
        }
      break;
    }
  case BOIL_4:
    {
      if (iTimeRemaining <   BrewParameters.uiHopTimes[4])
        {
          xQueueSendToBack(xHopsQueue, (void *)1,0);
          vConsolePrint("Hops Delivered 4\r\n");
          xBoilTextMessage->pcMsgText = "HOPS 4 DELIVERED";
                   xBoilTextMessage->ucLine = 6;
                   xQueueSendToBack(xBrewAppletTextQueue, &xBoilTextMessage, 0);

          iBoilState = BOIL_5;
        }
      break;
    }
  case BOIL_5:
    {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
      if (iTimeRemaining <=   BrewParameters.uiHopTimes[5])
=======
      if (iTimeRemaining <   BrewParameters.uiHopTimes[5])
>>>>>>> master
=======
      if (iTimeRemaining <=   BrewParameters.uiHopTimes[5])
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
      if (iTimeRemaining <=   BrewParameters.uiHopTimes[5])
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
        {
          xQueueSendToBack(xHopsQueue, (void *)1,0);
          vConsolePrint("Hops Delivered 5\r\n");
          xBoilTextMessage->pcMsgText = "HOPS 5 DELIVERED";
                   xBoilTextMessage->ucLine = 6;
                   xQueueSendToBack(xBrewAppletTextQueue, &xBoilTextMessage, 0);

          iBoilState = BOIL_6;
        }
      break;
    }
  case BOIL_6:
    {
      if (Brew[BrewState.ucStep].uElapsedTime >= iBoilTime)
        {
          iBoilDuty = 0;
          xBoilMessage->pvMessageContent = (void*)iBoilDuty;
          Brew[BrewState.ucStep].ucComplete = 1;
          xQueueSendToBack(xBoilQueue, &xBoilMessage, 0);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
          vConsolePrint("Boil Completed, sent duty 0 to boil\r\n");
=======
>>>>>>> master
=======
          vConsolePrint("Boil Completed, sent duty 0 to boil\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
          vConsolePrint("Boil Completed, sent duty 0 to boil\r\n");
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
          vBrewNextStep();
        }
      break;
    }
  case BOIL_7: //bringtoboil
    {
      if (Brew[BrewState.ucStep].uElapsedTime >= iBringToBoilTime)
        {
          iBoilDuty = 0;
          xBoilMessage->pvMessageContent = (void*)iBoilDuty;
          Brew[BrewState.ucStep].ucComplete = 1;
          xQueueSendToBack(xBoilQueue, &xBoilMessage, 0);
          vBrewNextStep();
        }
      break;
    }

  default:
    {
      iBoilState = 100;
      break;
      // hang here;
    }
  }// switch

}

//----------------------------------------------------------------------------------------------------------------------------
// BREW APPLET - Called from menu
//----------------------------------------------------------------------------------------------------------------------------
void vBrewApplet(int init){
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

  if (init)
    {

=======
  size_t heap;
  if (init)
    {
      heap = xPortGetFreeHeapSize();
      vConsolePrint("HEAP REMAINING: %d\r\n", heap);
>>>>>>> master
=======

  if (init)
    {

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

  if (init)
    {

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
      lcd_DrawRect(TOP_BANNER_X1, TOP_BANNER_Y1, TOP_BANNER_X2, TOP_BANNER_Y2, Cyan);
      lcd_fill(TOP_BANNER_X1+1, TOP_BANNER_Y1+1, TOP_BANNER_W, TOP_BANNER_H, Blue);
      //lcd_printf(12,0,13,"BREW");
      lcd_printf(15,1,13,"BREW");

      lcd_DrawRect(BUTTON_1_X1, BUTTON_1_Y1, BUTTON_1_X2, BUTTON_1_Y2, Red);
      lcd_fill(BUTTON_1_X1+1, BUTTON_1_Y1+1, BUTTON_1_W, BUTTON_1_H, Green);
      lcd_DrawRect(BUTTON_2_X1, BUTTON_2_Y1, BUTTON_2_X2, BUTTON_2_Y2, Red);
      lcd_fill(BUTTON_2_X1+1, BUTTON_2_Y1+1, BUTTON_2_W, BUTTON_2_H, Green);
      lcd_DrawRect(BUTTON_3_X1, BUTTON_3_Y1, BUTTON_3_X2, BUTTON_3_Y2, Red);
      lcd_fill(BUTTON_3_X1+1, BUTTON_3_Y1+1, BUTTON_3_W, BUTTON_3_H, Green);

      lcd_DrawRect(BUTTON_4_X1, BUTTON_4_Y1, BUTTON_4_X2, BUTTON_4_Y2, Red);
      lcd_fill(BUTTON_4_X1+1, BUTTON_4_Y1+1, BUTTON_4_W, BUTTON_4_H, Blue);
      lcd_DrawRect(BUTTON_5_X1, BUTTON_5_Y1, BUTTON_5_X2, BUTTON_5_Y2, Red);
      lcd_fill(BUTTON_5_X1+1, BUTTON_5_Y1+1, BUTTON_5_W, BUTTON_5_H, Blue);
      lcd_DrawRect(BUTTON_6_X1, BUTTON_6_Y1, BUTTON_6_X2, BUTTON_6_Y2, Red);
      lcd_fill(BUTTON_6_X1+1, BUTTON_6_Y1+1, BUTTON_6_W, BUTTON_6_H, Blue);

      lcd_DrawRect(BK_X1, BK_Y1, BK_X2, BK_Y2, White);
      lcd_fill(BK_X1+1, BK_Y1+1, BK_W, BK_H, Red);

      lcd_printf(33,1,13,"GRAPH"); //Button1
      lcd_printf(33,5,13,"STATS"); //Button2
      lcd_printf(33,8,13,"RESUME"); //Button3
      lcd_printf(30,13,10, "QUIT");
      lcd_printf(1,13,5, "START"); //Button4
      lcd_printf(9,13,5, "PAUSE"); //Button5
      lcd_printf(17,13,5, "SKIP"); //Button6


      vConsolePrint("Brew Applet Opened\r\n");
      vSemaphoreCreateBinary(xBrewAppletRunningSemaphore);
      if (xBrewTaskStateQueue == NULL && xBrewTaskReceiveQueue == NULL )
        {
          xBrewTaskStateQueue = xQueueCreate(1, sizeof(uint8_t));
          xBrewTaskReceiveQueue = xQueueCreate(5, sizeof(struct GenericMessage *));
          if (xBrewTaskStateQueue == NULL || xBrewTaskReceiveQueue == NULL )
            vConsolePrint("FATAL Error creating Brew Task Queues\r\n");
          else vConsolePrint("Created Brew Task Queues\r\n");
        }
      //      if (xBrewTaskHLTQueue == NULL)
      //            {
      //              xBrewTaskHLTQueue = xQueueCreate(5,sizeof(struct GenericMessage *));
      //              if (xBrewTaskHLTQueue == NULL)
      //                vConsolePrint("FATAL Error creating  Brew HLT Task Queue\r\n");
      //              else vConsolePrint("Created Brew HLT Task Queues\r\n");
      //            }

      if (xBrewAppletTextQueue == NULL)
        {
          xBrewAppletTextQueue = xQueueCreate(5,sizeof(struct TextMsg *));
          if (xBrewAppletTextQueue == NULL)
            vConsolePrint("FATAL Error creating Applet Text Queue\r\n");
          else vConsolePrint("Created Applet Text Queue\r\n");
        }

      if (xBrewTaskHandle == NULL)
        xTaskCreate( vTaskBrew,
            ( signed portCHAR * ) "Brew Task",
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
            configMINIMAL_STACK_SIZE + 900,
=======
            configMINIMAL_STACK_SIZE + 800,
>>>>>>> master
=======
            configMINIMAL_STACK_SIZE + 900,
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
            configMINIMAL_STACK_SIZE + 900,
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
            NULL,
            tskIDLE_PRIORITY+2,
            &xBrewTaskHandle );
      vConsolePrint("Brew Task Created\r\n");

      if (xBrewHLTTaskHandle == NULL)
        xTaskCreate( vTaskBrewHLT,
            ( signed portCHAR * ) "Brew HLT",
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
            configMINIMAL_STACK_SIZE + 300,
=======
            configMINIMAL_STACK_SIZE + 200,
>>>>>>> master
=======
            configMINIMAL_STACK_SIZE + 300,
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
            configMINIMAL_STACK_SIZE + 300,
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
            NULL,
            tskIDLE_PRIORITY,
            &xBrewHLTTaskHandle );
      vConsolePrint("Brew HLT Task Created\r\n");

      xTaskCreate( vBrewAppletDisplay,
          ( signed portCHAR * ) "Brew Display",
          configMINIMAL_STACK_SIZE + 200,
          NULL,
          tskIDLE_PRIORITY,
          &xBrewAppletDisplayHandle );

      if (xBrewGraphAppletDisplayHandle == NULL)
        {
          xTaskCreate( vBrewGraphAppletDisplay,
              ( signed portCHAR * ) "Brew Graph",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY,
              &xBrewGraphAppletDisplayHandle );
          vConsolePrint("Graph Applet Task Created\r\n");
        }
      vTaskSuspend(xBrewGraphAppletDisplayHandle);

      if (xBrewStatsAppletDisplayHandle == NULL)
        {
          xTaskCreate( vBrewStatsAppletDisplay,
              ( signed portCHAR * ) "Brew Stats",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY,
              &xBrewStatsAppletDisplayHandle );
          vConsolePrint("Stats Applet Task Created\r\n");
        }

      vTaskSuspend(xBrewStatsAppletDisplayHandle);
      if (xBrewResAppletDisplayHandle == NULL)
        {
          xTaskCreate( vBrewResAppletDisplay,
              ( signed portCHAR * ) "Brew Resume",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY,
              &xBrewResAppletDisplayHandle );
          vConsolePrint("Resume Applet Task Created\r\n");
        }

      vTaskSuspend(xBrewResAppletDisplayHandle);

      vTaskDelay(200);
    }
  else vConsolePrint("Leaving Brew Applet\r\n");

}

//----------------------------------------------------------------------------------------------------------------------------
// BREW GRAPH SUB-APPLET
//----------------------------------------------------------------------------------------------------------------------------
void vBrewGraphAppletDisplay(void * pvParameters){
  int i = 0;
  CLEAR_APPLET_CANVAS;
  for (;;)
    {
      vConsolePrint("Graph Display Applet Running\r\n");

      vTaskDelay(500);
    }

}


//----------------------------------------------------------------------------------------------------------------------------
// BREW STATISTICS SUB-APPLET
//----------------------------------------------------------------------------------------------------------------------------
void vBrewStatsAppletDisplay(void * pvParameters){

  CLEAR_APPLET_CANVAS;
  for (;;)
    {
      vConsolePrint("Stats Display Applet Running\r\n");
      vTaskDelay(500);
    }

}


void vBrewResAppletDisplay(void * pvParameters){
  static unsigned char ucLastStep;
  ucLastStep = 255;
  CLEAR_APPLET_CANVAS;
  DRAW_RESUME_BUTTONS;
  DRAW_RESUME_BUTTON_TEXT;

  for (;;)
    {
      if (ucLastStep != ucResStep)
        {
          CLEAR_APPLET_CANVAS;
          if (ucResStep > 0)
            lcd_printf(1,4,10, "  STEP %d: %s", ucResStep-1, Brew[ucResStep-1].pcStepName);
          lcd_printf(1,5,10, "->STEP %d: %s", ucResStep, Brew[ucResStep].pcStepName);
          if (ucResStep < MAX_BREW_STEPS)
            lcd_printf(1,6,10, "  STEP %d: %s", ucResStep+1, Brew[ucResStep+1].pcStepName);
          vConsolePrint("Resume Display Applet Running\r\n");

          ucLastStep = ucResStep;
        }
      vTaskDelay(200);
    }


}

//{
//      if (ucLastStep != BrewState.ucStep)
//        {
//          CLEAR_APPLET_CANVAS;
//          if (BrewState.ucStep > 0)
//            lcd_printf(1,4,10, "  STEP %d: %s", BrewState.ucStep-1, Brew[BrewState.ucStep-1].pcStepName);
//          lcd_printf(1,5,10, "->STEP %d: %s", BrewState.ucStep, Brew[BrewState.ucStep].pcStepName);
//          if (BrewState.ucStep < MAX_BREW_STEPS)
//            lcd_printf(1,6,10, "  STEP %d: %s", BrewState.ucStep+1, Brew[BrewState.ucStep+1].pcStepName);
//          vConsolePrint("Resume Display Applet Running\r\n");
//
//          ucLastStep = BrewState.ucStep;
//        }
//      vTaskDelay(200);
//    }


<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
unsigned int uiGetBrewAppletDisplayHWM()
{
  if (xBrewAppletDisplayHandle)
   {return uxTaskGetStackHighWaterMark(xBrewAppletDisplayHandle);}
  else return 0;
}

unsigned int uiGetBrewTaskHWM()
{
  if (xBrewTaskHandle)
  {return uxTaskGetStackHighWaterMark(xBrewTaskHandle);}
  else return 0;
}

unsigned int uiGetBrewGraphAppletHWM()
{
  if(xBrewGraphAppletDisplayHandle)
  {return uxTaskGetStackHighWaterMark(xBrewGraphAppletDisplayHandle);}
  else return 0;
}
unsigned int uiGetBrewStatsAppletHWM()
{
  if(xBrewStatsAppletDisplayHandle)
  {return uxTaskGetStackHighWaterMark(xBrewStatsAppletDisplayHandle);}
  else return 0;
}

unsigned int uiGetBrewResAppletHWM()
{
  if (xBrewResAppletDisplayHandle)
  {return uxTaskGetStackHighWaterMark(xBrewResAppletDisplayHandle);}
  else return 0;
}

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

void vBrewAppletDisplay( void *pvParameters){

  static char tog = 0; //toggles each loop
  //unsigned int uiDecimalPlaces = 3;
  float fHLTTemp = 0, fMashTemp = 0, fAmbientTemp = 0, fCabinetTemp;
  float fHLTTempLast = 1, fMashTempLast = 1, fAmbientTempLast = 1, fCabinetTempLast;
  static char buf[8][40];
  static char b[40], a[40];
  struct TextMsg * RcvdTextMsg;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  RcvdTextMsg = (struct TextMsg *)pvPortMalloc(sizeof(struct TextMsg));
=======
  RcvdTextMsg = (struct TextMsg *)malloc(sizeof(struct TextMsg));
>>>>>>> master
=======
  RcvdTextMsg = (struct TextMsg *)pvPortMalloc(sizeof(struct TextMsg));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
  RcvdTextMsg = (struct TextMsg *)pvPortMalloc(sizeof(struct TextMsg));
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  unsigned char ucHLTLevel;
  static unsigned char ucLastStep = 255;
  static unsigned char ucLastState = 255;
  static unsigned char ucLastHLTLevel = 255;
  lcd_printf(0,10,40, "|HLT   |MASH  |CAB   |AMB   |");
  lcd_printf(0,11,40, "|      |      |      |      |");
  ucLastStep = BrewState.ucStep+1;
  ucLastState = BrewState.ucRunningState-1;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
  for(;;)
    {

      xSemaphoreTake(xBrewAppletRunningSemaphore, portMAX_DELAY); //take the semaphore so that the key handler wont
      //return to the menu system until its returned
      if (xQueueReceive(xBrewAppletTextQueue, &RcvdTextMsg, 0) == pdPASS)
        {
          sprintf(buf[RcvdTextMsg->ucLine], RcvdTextMsg->pcMsgText);
        }
      fHLTTemp = ds1820_get_temp(HLT);
      fMashTemp = ds1820_get_temp(MASH);
      fAmbientTemp = ds1820_get_temp(AMBIENT);
      fCabinetTemp = ds1820_get_temp(CABINET);

      if (!GPIO_ReadInputDataBit(HLT_LEVEL_CHECK_PORT, HLT_LEVEL_CHECK_PIN))
        {
          ucHLTLevel = HLT_LEVEL_MID;
          if (!GPIO_ReadInputDataBit(HLT_HIGH_LEVEL_PORT, HLT_HIGH_LEVEL_PIN))
            {
              ucHLTLevel = HLT_LEVEL_HIGH;
            }

        }
      else {
          ucHLTLevel = HLT_LEVEL_LOW;
      }



      if (ucLastStep != BrewState.ucStep && Brew[BrewState.ucStep].pcStepName != NULL)
        {
          sprintf(buf[0], "STEP %d: %s", BrewState.ucStep, Brew[BrewState.ucStep].pcStepName);
          ucLastStep = BrewState.ucStep;
        }
      if (ucLastState != BrewState.ucRunningState)
        {
          sprintf(buf[1], "STATE = %s", pcRunningState[BrewState.ucRunningState]);
          ucLastState = BrewState.ucRunningState;
        }
      if (ucLastHLTLevel != ucHLTLevel)
        {
          sprintf(buf[3], "HLT Level = %s", pcHLTLevels[ucHLTLevel]);
          ucLastHLTLevel = ucHLTLevel;
        }
      sprintf(buf[2], "Elapsed Time: %02d:%02d", Brew[BrewState.ucStep].uElapsedTime/60, Brew[BrewState.ucStep].uElapsedTime%60);


      //fHLTTemp = fHLTTemp + 0.1;


      if(tog)
        {
          CLEAR_APPLET_CANVAS;

          //lcd_printf(1, 8, 25, "HLT = %d.%ddegC", (unsigned int)floor(fHLTTemp), (unsigned int)((fHLTTemp-floor(fHLTTemp))*pow(10, 1)));
          //lcd_printf(1, 9, 25, "MASH = %d.%ddegC", (unsigned int)floor(fMashTemp), (unsigned int)((fMashTemp-floor(fMashTemp))*pow(10, 1)));
          //lcd_printf(1, 10, 25, "Cabinet = %d.%ddegC", (unsigned int)floor(fCabinetTemp), (unsigned int)((fCabinetTemp-floor(fCabinetTemp))*pow(10, 1)));
          //lcd_printf(1, 11, 25, "Ambient = %d.%ddegC", (unsigned int)floor(fAmbientTemp), (unsigned int)((fAmbientTemp-floor(fAmbientTemp))*pow(10, 1)));
          //lcd_printf(1,10,40, "|HLT   |MASH  |CAB   |AMB   |");
          //lcd_printf(1,11,40, "|      |      |      |      |");
          if((fHLTTempLast != fHLTTemp) ||(fMashTempLast != fMashTemp) || (fCabinetTempLast != fCabinetTemp) || (fAmbientTempLast != fAmbientTemp) )
            {
              lcd_fill(0,159, 254, 32, Black);
              lcd_printf(0,10,40, "|HLT   |MASH  |CAB   |AMB   |");
              lcd_printf(0,11,40, "|      |      |      |      |");
              LCD_FLOAT(1,11, 2,fHLTTemp);
              LCD_FLOAT(8,11, 2,fMashTemp);
              LCD_FLOAT(15,11, 2,fCabinetTemp);
              LCD_FLOAT(22,11, 2,fAmbientTemp);
              fHLTTempLast = fHLTTemp;
              fMashTempLast = fMashTemp;
              fAmbientTempLast = fAmbientTemp;
              fCabinetTempLast = fCabinetTemp;
            }
          lcd_text_xy(1 * 8, 2 * 16, buf[0], Yellow, Blue2);
          lcd_text_xy(1 * 8, 3 * 16, buf[1], Green, Blue2);
          lcd_text_xy(1 * 8, 4 * 16, buf[2], Blue2, Blue2);
          lcd_text_xy(1 * 8, 5 * 16, buf[3], Blue, Blue2);
          if (buf[4] != NULL)
            lcd_text_xy(1 * 8, 6 * 16, buf[4], Grey, Blue2);
          if (buf[5] != NULL)
            lcd_text_xy(1 * 8, 7 * 16, buf[5], Grey, Blue2);
          if (buf[6] != NULL)
            lcd_text_xy(1 * 8, 8 * 16, buf[6], Grey, Blue2);
          if (buf[7] != NULL)
            lcd_text_xy(1 * 8, 9 * 16, buf[7], Grey, Blue2);

          xSemaphoreGive(xBrewAppletRunningSemaphore); //give back the semaphore as its safe to return now.
          vTaskDelay(750);

        }
      else
        {
          CLEAR_APPLET_CANVAS;
          xSemaphoreGive(xBrewAppletRunningSemaphore); //give back the semaphore as its safe to return now.
          vTaskDelay(250);

        }
      static int j = 0;
      int i = uxTaskGetStackHighWaterMark(xBrewTaskHandle);
      sprintf(a, "BrewTask HWM: %d\r\n", i);
      if (j != i){
          vConsolePrint(a);
          j = i;
      }


      tog = tog ^ 1;


    }

}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
void vBrewRemoteStart(){
  const uint8_t uRun = RUNNING;
  vBrewRunStep();
  xQueueSendToBack(xBrewTaskStateQueue, &uRun, 0);
}
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

static uint8_t uEvaluateTouch(int xx, int yy)
{

  if (xx > BUTTON_1_X1+5 && xx < BUTTON_1_X2-5 && yy > BUTTON_1_Y1+5 && yy < BUTTON_1_Y2-5)
    return BUTTON_1;
  if (xx > BUTTON_2_X1+5 && xx < BUTTON_2_X2-5 && yy > BUTTON_2_Y1+5 && yy < BUTTON_2_Y2-5)
    return BUTTON_2;
  if (xx > BUTTON_3_X1+5 && xx < BUTTON_3_X2-5 && yy > BUTTON_3_Y1+5 && yy < BUTTON_3_Y2-5)
    return BUTTON_3;
  if (xx > BUTTON_4_X1+5 && xx < BUTTON_4_X2-5 && yy > BUTTON_4_Y1+5 && yy < BUTTON_4_Y2-5)
    return BUTTON_4;
  if (xx > BUTTON_5_X1+5 && xx < BUTTON_5_X2-5 && yy > BUTTON_5_Y1+5 && yy < BUTTON_5_Y2-5)
    return BUTTON_5;
  if (xx > BUTTON_6_X1+5 && xx < BUTTON_6_X2-5 && yy > BUTTON_6_Y1+5 && yy < BUTTON_6_Y2-5)
    return BUTTON_6;
  if (xx > BK_X1+5 && xx < BK_X2-5 && yy > BK_Y1+5 && yy < BK_Y2-5)
    return QUIT_BUTTON;

  return NO_BUTTON;

}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> master
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======

>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
int iBrewKey(int xx, int yy)
{
  uint16_t window = 0;
  static uint8_t w = 5,h = 5;
  static uint16_t last_window = 0;
  int iKeyFromApplet = 0;
  uint8_t uButton;
  const uint8_t uRun = RUNNING, uIdle = IDLE, uStepComplete = STEP_COMPLETE;
  uButton = uEvaluateTouch(xx,yy);
  static uint8_t iOneShot = 0;
  static unsigned char ucPause = 0;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
  static struct GenericMessage * xMessage;
  xMessage = (struct GenericMessage *)malloc(sizeof(struct GenericMessage));


>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

  if (xx == -1 || yy == -1)
    return 0;

  switch (iAppletState)
  {
  case GRAPH:
    {
      vConsolePrint("AppletState = GRAPH\r\n");
      switch(uButton)
      {
      case BUTTON_1:
        {
          iAppletState = GRAPH;
          break;
        }
      case BUTTON_2:
        {
          iAppletState = STATS;
          break;
        }
      case BUTTON_3:
        {
          iAppletState = RES;
          ucResStep = BrewState.ucStep;
          break;
        }
      case BUTTON_4:
        {
          break;
        }
      case BUTTON_5:
        {
          break;
        }
      case BUTTON_6:
        {
          break;
        }

      case QUIT_BUTTON:
        {
          xSemaphoreTake(xBrewAppletRunningSemaphore, portMAX_DELAY);
          vTaskSuspend(xBrewGraphAppletDisplayHandle);
          vTaskResume(xBrewAppletDisplayHandle);
          xSemaphoreGive(xBrewAppletRunningSemaphore);
          iAppletState = MAIN;

          break;
        }
      case NO_BUTTON:
        {
          break;
        }
      }// switch uButton
      break;
    }// case GRAPH
  case STATS:
    {
      vConsolePrint("AppletState = STATS\r\n");
      switch(uButton)
      {
      case BUTTON_1:
        {
          iAppletState = GRAPH;
          break;
        }
      case BUTTON_2:
        {
          iAppletState = STATS;
          break;
        }
      case BUTTON_3:
        {
         iAppletState = RES;
         ucResStep = BrewState.ucStep;
          break;
        }
      case BUTTON_4:
        {
          break;
        }
      case BUTTON_5:
        {
          break;
        }
      case BUTTON_6:
        {
          break;
        }

      case QUIT_BUTTON:
        {
          xSemaphoreTake(xBrewAppletRunningSemaphore, portMAX_DELAY);
          vTaskSuspend(xBrewStatsAppletDisplayHandle);
          vTaskResume(xBrewAppletDisplayHandle);
          xSemaphoreGive(xBrewAppletRunningSemaphore);
          iAppletState = MAIN;
          break;
        }
      case NO_BUTTON:
        {
          break;
        }
      }// switch uButton
      break;
    }// case STATS

  case RES:
    {
      vConsolePrint("AppletState = RES\r\n");
      switch(uButton)
      {
      case BUTTON_1:
        {
          iAppletState = GRAPH;
          break;
        }
      case BUTTON_2:
        {
          iAppletState = STATS;
          break;
        }
      case BUTTON_3:
        {
          iAppletState = RES;
          ucResStep = BrewState.ucStep;
          break;
        }
      case BUTTON_4:
        {
          if (ucResStep < MAX_BREW_STEPS)
            {
              //BrewState.ucStep++;
              ucResStep++;
              vConsolePrint("Increasing Brew Step\r\n");
            }
          break;
        }
      case BUTTON_5:
        {
          if (ucResStep > 0)
            {
              //BrewState.ucStep--;
              ucResStep--;
              vConsolePrint("Decreasing Brew Step\r\n");
            }
          break;
        }
      case BUTTON_6: //RESUME Button at bottom
        {
          //for (int i = 0; i < BrewState.ucStep; i++)
            for (int i = 0; i < ucResStep; i++)
            {
              Brew[i].ucComplete = 1;
            }
            BrewState.ucStep = ucResStep;
          vBrewRunStep();
          xQueueSendToBack(xBrewTaskStateQueue, &uRun, 0);
          break;
        }

      case QUIT_BUTTON:
        {
          vConsolePrint("Leaving Resume Applet\r\n");
          xSemaphoreTake(xBrewAppletRunningSemaphore, portMAX_DELAY);
          vTaskSuspend(xBrewResAppletDisplayHandle);
          CLEAR_APPLET_CANVAS;
          DRAW_RESUME_BUTTONS;
          DRAW_MAIN_BUTTON_TEXT;

          vTaskResume(xBrewAppletDisplayHandle);
          xSemaphoreGive(xBrewAppletRunningSemaphore);
          iAppletState = MAIN;
          break;
        }
      case NO_BUTTON:
        {
          break;
        }
      }// switch uButton
      break;
    }// case RES
  case MAIN:
    {
      switch(uButton)
      {
      case BUTTON_1:
        {
          xSemaphoreTake(xBrewAppletRunningSemaphore, portMAX_DELAY);
          vTaskSuspend(xBrewAppletDisplayHandle);
          vTaskSuspend(xBrewStatsAppletDisplayHandle);
          vTaskSuspend(xBrewResAppletDisplayHandle);
          CLEAR_APPLET_CANVAS;
          vTaskResume(xBrewGraphAppletDisplayHandle);
          xSemaphoreGive(xBrewAppletRunningSemaphore);
          vConsolePrint("Button1 Pressed\r\n");
          iAppletState = GRAPH;

          break;
        }
      case BUTTON_2:
        {
          xSemaphoreTake(xBrewAppletRunningSemaphore, portMAX_DELAY);
          vTaskSuspend(xBrewAppletDisplayHandle);
          vTaskSuspend(xBrewGraphAppletDisplayHandle);
          vTaskSuspend(xBrewResAppletDisplayHandle);
          CLEAR_APPLET_CANVAS;
          vTaskResume(xBrewStatsAppletDisplayHandle);
          xSemaphoreGive(xBrewAppletRunningSemaphore);
          vConsolePrint("Button2 Pressed\r\n");
          iAppletState = STATS;

          break;
        }
      case BUTTON_3:
        {
          xSemaphoreTake(xBrewAppletRunningSemaphore, portMAX_DELAY);
          vTaskSuspend(xBrewAppletDisplayHandle);
          vTaskSuspend(xBrewGraphAppletDisplayHandle);
          vTaskSuspend(xBrewStatsAppletDisplayHandle);

          CLEAR_APPLET_CANVAS;
          DRAW_RESUME_BUTTONS;
          DRAW_RESUME_BUTTON_TEXT;

          vTaskResume(xBrewResAppletDisplayHandle);

          xSemaphoreGive(xBrewAppletRunningSemaphore);


          iAppletState = RES;
          ucResStep = BrewState.ucStep;
          vConsolePrint("Button3 Pressed\r\n");

          break;
        }
      case BUTTON_4:
        {
          vBrewRunStep();
          xQueueSendToBack(xBrewTaskStateQueue, &uRun, 0);
          //xQueueSendToBack(xBrewTaskReceiveQueue, &uStepComplete, 0);
          break;
        }
      case BUTTON_5:
        {
          if (ucPause == 0)
            {
              xQueueSendToBack(xBrewTaskStateQueue, &uIdle, 0);
              ucPause = 1;
            }
          else if (ucPause == 1)
            {
              xQueueSendToBack(xBrewTaskStateQueue, &uRun, 0);
              ucPause = 0;
            }

          break;
        }
      case BUTTON_6:
        {
          Brew[BrewState.ucStep].ucComplete = 1;
          if (BrewState.ucRunningState == RUNNING)
            {
              vBrewNextStep();
            }
          else
            BrewState.ucStep++;
          //xQueueSendToBack(xBrewTaskStateQueue, &uRun, 0);
          break;
        }

      case QUIT_BUTTON:
        {
          iAppletState = QUIT;
          vConsolePrint("QUIT Pressed\r\n");
          xSemaphoreTake(xBrewAppletRunningSemaphore, portMAX_DELAY); //given back in QUIT state
          vTaskSuspend(xBrewAppletDisplayHandle);
          CLEAR_APPLET_CANVAS;
          //if (iOneShot == 0)
          {
            lcd_DrawRect(Q_X1, Q_Y1, Q_X2, Q_Y2, White);
            lcd_fill(Q_X1+1, Q_Y1+1, Q_W, Q_H, Blue);
            lcd_printf(13,4,13,"QUIT???");
            lcd_printf(13,5,13,"IF YES PRESS");
            lcd_printf(13,6,13,"QUIT AGAIN");
            lcd_printf(13,7,13,"IF NO, PRESS");
            lcd_printf(13,8,13,"HERE");
            vConsolePrint("Press back again to exit BREW\r\n");
          }
          break;
        }
      case NO_BUTTON:
        {
          break;
        }

      }// switch uButton
      break;
    }// case STATS
  case QUIT:
    {

      if (xx > BK_X1 && yy > BK_Y1 && xx < BK_X2 && yy < BK_Y2)
        {
          vConsolePrint("Selected to leave BREW Applet\r\n");

          vTaskResume(xBrewAppletDisplayHandle);
          if (xBrewAppletDisplayHandle != NULL)
            {
              vTaskDelete(xBrewAppletDisplayHandle);
              vTaskDelay(100);
              xBrewAppletDisplayHandle = NULL;
            }
          vBrewTotalReset();
          //return the semaphore for taking by another task.
          xSemaphoreGive(xBrewAppletRunningSemaphore);
          iAppletState = MAIN;

          return 1;
        }
      else if (xx > Q_X1 && yy > Q_Y1 && xx < Q_X2 && yy < Q_Y2)
        {
          vTaskResume(xBrewAppletDisplayHandle);
          iAppletState = MAIN;

          vConsolePrint("Staying BREW Applet\r\n");
        }
      xSemaphoreGive(xBrewAppletRunningSemaphore);
      break;
    }// case QUIT
  }//Switch iAppletState

  vTaskDelay(10);
  return 0;
}

 //HLT - no control in manual.
// When Quit is pressed.. need to send message to HLT to go to IDLE state



static struct BrewStep Brew[] = {
    // TEXT                       SETUP                           POLL                                   PARMS                          TIMEOUT   START ELAPSED COMPLETE WAIT
    {"Waiting",              NULL,                                (void *)vBrewWaitingPollFunction , {3,0,0,0,0},                          20,     0,      0, 0, 0},
<<<<<<< HEAD
    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 1},
    {"Close D-Valves",       (void *)vBrewValvesSetupFunction,    NULL,                              {0,0,0,0,0},                      1,      0,      0, 0, 0},
    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
    {"Fill+Heat:Strike",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT,STRIKE,0,0,0},   40*60,  0,      0, 0, 0},
    {"Grind Grains",         (void *)vBrewMillSetupFunction,      (void *)vBrewMillPollFunction,     {0,0,0,0,0},                          10*60,   0,      0, 0, 0},
    {"Drain HLT for Mash",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,STRIKE,0,0,0},       5*60,   0,      0, 0, 1},
    {"Lower Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {DN_INC,0,0,0,0},                     60,     0,      0, 0, 1},
    {"Fill+Heat:Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, MASH_OUT,0,0,0},40*60,  0,      0, 0, 1},
    {"Mash",                 (void *)vBrewMashSetupFunction,      (void *)vBrewMashPollFunction,     {0,0,0,0,0},                       60*60,  0,      0, 0, 0},
    {"Drain for Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,MASH_OUT,0,0,0},     5*60,   0,      0, 0, 1},
    {"Fill+Heat:Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT,SPARGE,0,0,0},   40*60,  0,      0, 0, 1},
    {"Mash out",             (void *)vBrewMashOutSetupFunction,   (void *)vBrewMashPollFunction,     {0,0,0,0,0},                         10*60,  0,      0, 0, 0},
    {"MO pump to boil",      (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,7*60,0,0,0},                      11*60,  0,      0, 0, 0},
    {"Drain for Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,SPARGE,0,0,0},       10*60,  0,      0, 0, 1},
    {"Sparge",               (void *)vBrewSpargeSetupFunction,    (void *)vBrewMashPollFunction,     {0,0,0,0,0},                          5*60,   0,      0, 0, 1},
    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         10,     0,      0, 0, 1},
    {"Pump to boil",         (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,4*60,0,0,0},                      11*60,  0,      0, 0, 1},
=======
    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 0},
    {"Close D-Valves",       (void *)vBrewValvesSetupFunction,    NULL,                              {0,0,0,0,0},                      1,      0,      0, 0, 0},
    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
    {"Fill+Heat:Strike",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT,STRIKE,0,0,0},   40*60,  0,      0, 0, 0},
    {"Grind Grains",         (void *)vBrewMillSetupFunction,      (void *)vBrewMillPollFunction,     {0,0,0,0,0},                          10*60,   0,      0, 0, 0},
    {"Drain HLT for Mash",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,STRIKE,0,0,0},       5*60,   0,      0, 0, 1},
    {"Lower Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {DN_INC,0,0,0,0},                     60,     0,      0, 0, 1},
    {"Fill+Heat:Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, MASH_OUT,0,0,0},40*60,  0,      0, 0, 1},
    {"Mash",                 (void *)vBrewMashSetupFunction,      (void *)vBrewMashPollFunction,     {0,0,0,0,0},                       60*60,  0,      0, 0, 0},
    {"Drain for Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,MASH_OUT,0,0,0},     5*60,   0,      0, 0, 1},
    {"Fill+Heat:Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT,SPARGE,0,0,0},   40*60,  0,      0, 0, 1},
    {"Mash out",             (void *)vBrewMashOutSetupFunction,   (void *)vBrewMashPollFunction,     {0,0,0,0,0},                         10*60,  0,      0, 0, 0},
    {"MO pump to boil",      (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,7*60,0,0,0},                      11*60,  0,      0, 0, 0},
    {"Drain for Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,SPARGE,0,0,0},       10*60,  0,      0, 0, 1},
<<<<<<< HEAD
<<<<<<< HEAD
    {"Sparge",               NULL,                                (void *)vBrewMashPollFunction,     {5,2,0,1,3},                          5*60,   0,      0, 0, 1},
    {"Pump to boil",         (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,3*60,0,0,0},                      11*60,  0,      0, 0, 1},
    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         10,     0,      0, 0, 0},
>>>>>>> master
=======
    {"Sparge",               (void *)vBrewSpargeSetupFunction,    (void *)vBrewMashPollFunction,     {0,0,0,0,0},                          5*60,   0,      0, 0, 1},
    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         10,     0,      0, 0, 1},
    {"Pump to boil",         (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,4*60,0,0,0},                      11*60,  0,      0, 0, 1},
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
    {"Sparge",               (void *)vBrewSpargeSetupFunction,    (void *)vBrewMashPollFunction,     {0,0,0,0,0},                          5*60,   0,      0, 0, 1},
    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         10,     0,      0, 0, 1},
    {"Pump to boil",         (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,4*60,0,0,0},                      11*60,  0,      0, 0, 1},
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
    {"Fill+Heat:Clean ",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, CLEAN,0,0,0},40*60,  0,      0, 0, 0},
    {"BringToBoil",          (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {18,100,0,0,0},                       90*60,  0,      0, 0, 0},
    {"Pump to boil",         (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,30,0,0,0},                        11*60,  0,      0, 0, 1},
    {"Boil",                 (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {60,55,1,0,0},                        90*60,  0,      0, 0, 1},
    {"SettlingBeforeChill",  (void *)vBrewPreChillSetupFunction,  (void *)vBrewPreChillPollFunction,  {6*60,0,0,0,0},                       6*60,   0,      0, 0, 0},
    {"Chill",                (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,   {8,0,0,0,0},                          10*60,  0,      0, 0, 1},
    {"Waiting",              NULL,                                (void *)vBrewWaitingPollFunction,  {1,0,0,0,0},                          2,      0,      0, 0, 0},
    {NULL,                   NULL,                                NULL,                              {0,0,0,0,0},                          0,      0,      0, 0, 0}
};


<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

//
//// BREW CLEAN STEPS
//static struct BrewStep BrewClean[] = {
//
//    {"Waiting",              NULL,                                (void *)vBrewWaitingPollFunction , {3,0,0,0,0},                          20,     0,      0, 0, 0},
//    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 0},
//    {"Close D-Valves",       (void *)vBrewValvesSetupFunction,    NULL,                              {CLOSE,0,0,0,0},                      1,      0,      0, 0, 0},
//    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
//    {"Fill+Heat:Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, MASH_OUT,0,0,0},40*60,  0,      0, 0, 1},
//    {"Drain HLT for Mash",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,STRIKE,0,0,0},       5*60,   0,      0, 0, 1},
//    {"Fill+Heat:Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, MASH_OUT,0,0,0},40*60,  0,      0, 0, 1},
//    {"Mash",                 (void *)vBrewMashSetupFunction,      (void *)vBrewMashPollFunction,     {5,1,0,2,2},                          60*60,  0,      0, 0, 0},
//    {"Drain for Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,MASH_OUT,0,0,0},     5*60,   0,      0, 0, 1},
//    {"Fill+Heat:Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT,SPARGE,0,0,0},   40*60,  0,      0, 0, 1},
//    {"Mash out",             NULL,                                (void *)vBrewMashPollFunction,     {2,1,0,1,1},                         10*60,  0,      0, 0, 0},
//    {"MO pump to boil",      (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,5*60,0,0,0},                      11*60,  0,      0, 0, 0},
//    {"Bring To Boil",        (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {91,55,0,0,0},                        90*60,  0,      0, 0, 1},
//    {"Chill",                (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,   {4,0,0,0,0},                          10*60,  0,      0, 0, 1},
//    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
//    {"Drain for Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,SPARGE,0,0,0},       10*60,  0,      0, 0, 1},
//    {"Sparge",               NULL,                                (void *)vBrewMashPollFunction,     {2,1,0,1,1},                          5*60,   0,      0, 0, 1},
//    {"Pump to boil",         (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,3*60,0,0,0},                      11*60,  0,      0, 0, 1},
//    {"BringToBoil",          (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {90,100,0,0,0},                       90*60,  0,      0, 0, 0},
//    {"Boil",                 (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {91,55,1,0,0},                        90*60,  0,      0, 0, 1},
//    {"Chill",                (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,   {4,0,0,0,0},                          10*60,  0,      0, 0, 1},
//    {"Waiting",              NULL,                                (void *)vBrewWaitingPollFunction,  {1,0,0,0,0},                          2,      0,      0, 0, 0},
//    {NULL,                   NULL,                                NULL,                              {0,0,0,0,0},                          0,      0,      0, 0, 0}
//};
//
//
//
//
//
//static struct BrewStep BrewTest[] = {
//    //TEXT                       SETUP                                POLL                                   PARMS            TIMEOUT   START    ELAPSED COMPLETE WAIT
//    {"Waiting1",               NULL,                               (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 0},
//    {"Close D-Valves",        (void *)vBrewValvesSetupFunction,    NULL,                               {CLOSE,0,0,0,0},        1,      0,      0, 0, 1},
//    //{"Open BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                               {OPEN,0,0,0,0},                      5,      0,      0, 0, 0},
//    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 1},
//    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
//    {"Lower Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {DN_INC,0,0,0,0},                         25,     0,      0, 0, 1},
//    {"Waiting1",               NULL,                               (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 0},
//    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 1},
//   // {"Open BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                               {OPEN,0,0,0,0},                      5,      0,      0, 0, 0},
//  //  {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
//
//   // {"Mash",                  (void *)vBrewMashSetupFunction,      (void *)vBrewMashPollFunction,      {5,0,0,1,2},            6*60,  0,      0, 0, 0},
// //   {"Boil",                  (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,1,0,0},          90*60,  0,      0, 0, 1},
////    {"MO pump to boil",       (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{3,1*60,0,0,0},         11*60,  0,      0, 0, 0}, // have to do a couple of stop/starts for pump etc
////    {"BringToBoil",           (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,0,0,0},          90*60,  0,      0, 0, 0},
////    {"Boil",                  (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,1,0,0},          90*60,  0,      0, 0, 1},
//   // {"Chill",                 (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,    {5,0,0,0,0},            0,      0,      0, 0, 1},
////    {"Raise Crane",           (void *)vBrewCraneSetupFunction,     NULL,                               {UP,0,0,0,0},           25,     0,      0, 0, 0},
////    {"Waiting2",              NULL,                                (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 1},
////    {"Waiting3",              NULL,                                (void *)vBrewWaitingPollFunction ,  {3,0,0,0,0},            20,     0,      0, 0, 1},
//    {"Waiting4",              NULL,                                (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 1},
//    {NULL,                    NULL,                                NULL,                               {0,0,0,0,0},            0,      0,      0, 0, 0}
//};
=======
// BREW CLEAN STEPS
static struct BrewStep BrewClean[] = {

    {"Waiting",              NULL,                                (void *)vBrewWaitingPollFunction , {3,0,0,0,0},                          20,     0,      0, 0, 0},
    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 0},
    {"Close D-Valves",       (void *)vBrewValvesSetupFunction,    NULL,                              {CLOSE,0,0,0,0},                      1,      0,      0, 0, 0},
    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
    {"Fill+Heat:Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, MASH_OUT,0,0,0},40*60,  0,      0, 0, 1},
    {"Drain HLT for Mash",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,STRIKE,0,0,0},       5*60,   0,      0, 0, 1},
    {"Fill+Heat:Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, MASH_OUT,0,0,0},40*60,  0,      0, 0, 1},
    {"Mash",                 (void *)vBrewMashSetupFunction,      (void *)vBrewMashPollFunction,     {5,1,0,2,2},                          60*60,  0,      0, 0, 0},
    {"Drain for Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,MASH_OUT,0,0,0},     5*60,   0,      0, 0, 1},
    {"Fill+Heat:Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT,SPARGE,0,0,0},   40*60,  0,      0, 0, 1},
    {"Mash out",             NULL,                                (void *)vBrewMashPollFunction,     {2,1,0,1,1},                         10*60,  0,      0, 0, 0},
    {"MO pump to boil",      (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,5*60,0,0,0},                      11*60,  0,      0, 0, 0},
    {"Bring To Boil",        (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {91,55,0,0,0},                        90*60,  0,      0, 0, 1},
    {"Chill",                (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,   {4,0,0,0,0},                          10*60,  0,      0, 0, 1},
    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
    {"Drain for Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,SPARGE,0,0,0},       10*60,  0,      0, 0, 1},
    {"Sparge",               NULL,                                (void *)vBrewMashPollFunction,     {2,1,0,1,1},                          5*60,   0,      0, 0, 1},
    {"Pump to boil",         (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,3*60,0,0,0},                      11*60,  0,      0, 0, 1},
    {"BringToBoil",          (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {90,100,0,0,0},                       90*60,  0,      0, 0, 0},
    {"Boil",                 (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {91,55,1,0,0},                        90*60,  0,      0, 0, 1},
    {"Chill",                (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,   {4,0,0,0,0},                          10*60,  0,      0, 0, 1},
    {"Waiting",              NULL,                                (void *)vBrewWaitingPollFunction,  {1,0,0,0,0},                          2,      0,      0, 0, 0},
    {NULL,                   NULL,                                NULL,                              {0,0,0,0,0},                          0,      0,      0, 0, 0}
};


=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

//
//// BREW CLEAN STEPS
//static struct BrewStep BrewClean[] = {
//
//    {"Waiting",              NULL,                                (void *)vBrewWaitingPollFunction , {3,0,0,0,0},                          20,     0,      0, 0, 0},
//    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 0},
//    {"Close D-Valves",       (void *)vBrewValvesSetupFunction,    NULL,                              {CLOSE,0,0,0,0},                      1,      0,      0, 0, 0},
//    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
//    {"Fill+Heat:Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, MASH_OUT,0,0,0},40*60,  0,      0, 0, 1},
//    {"Drain HLT for Mash",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,STRIKE,0,0,0},       5*60,   0,      0, 0, 1},
//    {"Fill+Heat:Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT, MASH_OUT,0,0,0},40*60,  0,      0, 0, 1},
//    {"Mash",                 (void *)vBrewMashSetupFunction,      (void *)vBrewMashPollFunction,     {5,1,0,2,2},                          60*60,  0,      0, 0, 0},
//    {"Drain for Mash Out",   (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,MASH_OUT,0,0,0},     5*60,   0,      0, 0, 1},
//    {"Fill+Heat:Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_FILL_HEAT,SPARGE,0,0,0},   40*60,  0,      0, 0, 1},
//    {"Mash out",             NULL,                                (void *)vBrewMashPollFunction,     {2,1,0,1,1},                         10*60,  0,      0, 0, 0},
//    {"MO pump to boil",      (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,5*60,0,0,0},                      11*60,  0,      0, 0, 0},
//    {"Bring To Boil",        (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {91,55,0,0,0},                        90*60,  0,      0, 0, 1},
//    {"Chill",                (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,   {4,0,0,0,0},                          10*60,  0,      0, 0, 1},
//    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
//    {"Drain for Sparge",     (void *)vBrewHLTSetupFunction,       NULL,                              {HLT_STATE_DRAIN,SPARGE,0,0,0},       10*60,  0,      0, 0, 1},
//    {"Sparge",               NULL,                                (void *)vBrewMashPollFunction,     {2,1,0,1,1},                          5*60,   0,      0, 0, 1},
//    {"Pump to boil",         (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{0,3*60,0,0,0},                      11*60,  0,      0, 0, 1},
//    {"BringToBoil",          (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {90,100,0,0,0},                       90*60,  0,      0, 0, 0},
//    {"Boil",                 (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,    {91,55,1,0,0},                        90*60,  0,      0, 0, 1},
//    {"Chill",                (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,   {4,0,0,0,0},                          10*60,  0,      0, 0, 1},
//    {"Waiting",              NULL,                                (void *)vBrewWaitingPollFunction,  {1,0,0,0,0},                          2,      0,      0, 0, 0},
//    {NULL,                   NULL,                                NULL,                              {0,0,0,0,0},                          0,      0,      0, 0, 0}
//};
//
//
//
//
//
//static struct BrewStep BrewTest[] = {
//    //TEXT                       SETUP                                POLL                                   PARMS            TIMEOUT   START    ELAPSED COMPLETE WAIT
//    {"Waiting1",               NULL,                               (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 0},
//    {"Close D-Valves",        (void *)vBrewValvesSetupFunction,    NULL,                               {CLOSE,0,0,0,0},        1,      0,      0, 0, 1},
//    //{"Open BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                               {OPEN,0,0,0,0},                      5,      0,      0, 0, 0},
//    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 1},
//    {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
//    {"Lower Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {DN_INC,0,0,0,0},                         25,     0,      0, 0, 1},
<<<<<<< HEAD
<<<<<<< HEAD
    {"Waiting1",               NULL,                               (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 0},

   // {"Open BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                               {OPEN,0,0,0,0},                      5,      0,      0, 0, 0},
  //  {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},

   // {"Mash",                  (void *)vBrewMashSetupFunction,      (void *)vBrewMashPollFunction,      {5,0,0,1,2},            6*60,  0,      0, 0, 0},
 //   {"Boil",                  (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,1,0,0},          90*60,  0,      0, 0, 1},
//    {"MO pump to boil",       (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{3,1*60,0,0,0},         11*60,  0,      0, 0, 0}, // have to do a couple of stop/starts for pump etc
//    {"BringToBoil",           (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,0,0,0},          90*60,  0,      0, 0, 0},
//    {"Boil",                  (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,1,0,0},          90*60,  0,      0, 0, 1},
    {"Chill",                 (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,    {5,0,0,0,0},            0,      0,      0, 0, 1},
//    {"Raise Crane",           (void *)vBrewCraneSetupFunction,     NULL,                               {UP,0,0,0,0},           25,     0,      0, 0, 0},
//    {"Waiting2",              NULL,                                (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 1},
//    {"Waiting3",              NULL,                                (void *)vBrewWaitingPollFunction ,  {3,0,0,0,0},            20,     0,      0, 0, 1},
    {"Waiting4",              NULL,                                (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 1},
    {NULL,                    NULL,                                NULL,                               {0,0,0,0,0},            0,      0,      0, 0, 0}
};
>>>>>>> master
=======
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
//    {"Waiting1",               NULL,                               (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 0},
//    {"Raise Crane",          (void *)vBrewCraneSetupFunction,     NULL,                              {UP,0,0,0,0},                         25,     0,      0, 0, 1},
//   // {"Open BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                               {OPEN,0,0,0,0},                      5,      0,      0, 0, 0},
//  //  {"Close BoilValve",      (void *)vBrewBoilValveSetupFunction, NULL,                              {CLOSE,0,0,0,0},                      5,      0,      0, 0, 1},
//
//   // {"Mash",                  (void *)vBrewMashSetupFunction,      (void *)vBrewMashPollFunction,      {5,0,0,1,2},            6*60,  0,      0, 0, 0},
// //   {"Boil",                  (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,1,0,0},          90*60,  0,      0, 0, 1},
////    {"MO pump to boil",       (void *)vBrewPumpToBoilSetupFunction,(void *)vBrewPumpToBoilPollFunction,{3,1*60,0,0,0},         11*60,  0,      0, 0, 0}, // have to do a couple of stop/starts for pump etc
////    {"BringToBoil",           (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,0,0,0},          90*60,  0,      0, 0, 0},
////    {"Boil",                  (void *)vBrewBoilSetupFunction,      (void *)vBrewBoilPollFunction ,     {91,55,1,0,0},          90*60,  0,      0, 0, 1},
//   // {"Chill",                 (void *)vBrewChillSetupFunction,     (void *)vBrewChillPollFunction ,    {5,0,0,0,0},            0,      0,      0, 0, 1},
////    {"Raise Crane",           (void *)vBrewCraneSetupFunction,     NULL,                               {UP,0,0,0,0},           25,     0,      0, 0, 0},
////    {"Waiting2",              NULL,                                (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 1},
////    {"Waiting3",              NULL,                                (void *)vBrewWaitingPollFunction ,  {3,0,0,0,0},            20,     0,      0, 0, 1},
//    {"Waiting4",              NULL,                                (void *)vBrewWaitingPollFunction ,  {2,0,0,0,0},            20,     0,      0, 0, 1},
//    {NULL,                    NULL,                                NULL,                               {0,0,0,0,0},            0,      0,      0, 0, 0}
//};
<<<<<<< HEAD
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c





// Creates a function that takes a single element array of BrewStep Pointers(?)
// note how calling each element (although above 0) shows each of their names.

//void iTest1(struct BrewStep (*ThisBrew)[1])
//{
//  static char buf[50];
//  sprintf(buf, "calling setup function of %s, %s, %s\r\n", ThisBrew[0]->pcStepName, ThisBrew[1]->pcStepName, ThisBrew[2]->pcStepName);
//  vConsolePrint(buf);
//ThisBrew[2]->func((void *)ThisBrew[2]->iFuncParams);
//
//}

// creates a pointer to a single elemented BrewStep Array.. points it to the
// statically allocated Brew array above, then calls uTest1 with the pointer as
// the argument.

//int iTest(int i){
//  struct BrewStep (*MyBrew)[1];
//  vConsolePrint("iTest\r\n");
//
//  MyBrew = &Brew;
//  iTest1(MyBrew);
//  MyBrew[2]->uTimeout = 1;
//  return 1;
//
//}


