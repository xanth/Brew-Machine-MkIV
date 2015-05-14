/*
 * console.h
 *
 *  Created on: 11/11/2013
 *      Author: brad
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

void vConsolePrintTask( void * pvParameters);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
void vConsolePrint(const char * format);
=======
void vConsolePrint(const char * format, ...);
>>>>>>> master
=======
void vConsolePrint(const char * format);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
=======
void vConsolePrint(const char * format);
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
extern xQueueHandle xPrintQueue;

#endif /* CONSOLE_H_ */
