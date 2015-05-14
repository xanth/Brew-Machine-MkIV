/*
 * mill.h
 *
 *  Created on: Dec 13, 2012
 *      Author: brad
 */

#ifndef MILL_H_
#define MILL_H_

#define MILL_PORT GPIOE
#define MILL_PIN GPIO_Pin_6
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c
typedef enum
{
  MILL_DRIVING=1,
  MILL_STOPPED=0
}MillState;

<<<<<<< HEAD
=======
>>>>>>> master
=======
>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

void vMillInit(void);
void vMillApplet(int init);
int iMillKey(int xx, int yy);
<<<<<<< HEAD
<<<<<<< HEAD
void vMill( MillState state );



=======
void vMill( int state );

#define MILL_DRIVING 1
#define MILL_STOPPED -1
>>>>>>> master
=======
void vMill( MillState state );



>>>>>>> db059f7f6dbb785acc267ce99d8605bfef31246c

#endif /* MILL_H_ */
