///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 18 Feb 2012
//
// RCS $Date$
// RCS $Revision$
// RCS $Source$
// RCS $Log$///////////////////////////////////////////////////////////////////////////////

#ifndef CRANE_H
#define CRANE_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


//---CRANE LIMITS PORT/PINS
//#define CRANE_UPPER_LIMIT_PIN GPIO_Pin_14
<<<<<<< HEAD
//#define CRANE_UPPER_LIMIT_PIN GPIO_Pin_8
//#define CRANE_UPPER_LIMIT_PORT GPIOC

//now on I2C
#define CRANE_UPPER_LIMIT_PIN 1
#define CRANE_UPPER_LIMIT_PORT PORTV

#define CRANE_LOWER_LIMIT_PIN 2
#define CRANE_LOWER_LIMIT_PORT PORTV

//#define CRANE_LOWER_LIMIT_PIN GPIO_Pin_12
//#define CRANE_LIMIT_PORT GPIOB
=======
#define CRANE_UPPER_LIMIT_PIN GPIO_Pin_12
#define CRANE_UPPER_LIMIT_PORT GPIOA

#define CRANE_LOWER_LIMIT_PIN GPIO_Pin_12
#define CRANE_LIMIT_PORT GPIOB
>>>>>>> master

// Crane outputs
#define CRANE_PORT PORTU
#define CRANE_PIN1 PCF_PIN6
#define CRANE_PIN2 PCF_PIN7


void vCraneInit(void);
void vCraneRun(uint16_t speed);
void vTaskCrane(void * pvParameters);
void vCraneApplet(int init);
int iCraneKey(int x, int y);

// Directions
<<<<<<< HEAD
#define UP 10
#define DN 20
#define DN_INC 30
=======
#define UP 1
#define DN 2
#define DN_INC 3
>>>>>>> master
#define STOP -1

// Crane States
#define TOP 1
#define BOTTOM 2
#define DRIVING_UP 3
#define DRIVING_DOWN 4
#define DRIVING_DOWN_INC 5
#define STOPPED -1

extern xTaskHandle xCraneTaskHandle, xCraneAppletDisplayHandle;
extern xQueueHandle xCraneQueue;

#endif

