/*
 * stir.h
 *
 *  Created on: Dec 15, 2012
 *      Author: brad
 */

#ifndef STIR_H_
#define STIR_H_

//---STIR CONTROL PORTS/PINS


#define STIR_PORT PORTU
#define STIR_PIN PCF_PIN0

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
typedef enum
{
  STIR_DRIVING = 1,
  STIR_STOPPED = 2
} StirState;

void vStirInit(void); // initialises the output and semaphores for applet.
void vStir( StirState state ); // gives the command to the output
StirState xGetStirState(void); //returns the current state of the stirrer
<<<<<<< HEAD
=======
void vStirInit(void);
void vStirApplet(int init);
int iStirKey(int xx, int yy);
void vStir( int state );

#define DRIVING 1
#define STOPPED -1
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c


#endif /* STIR_H_ */
