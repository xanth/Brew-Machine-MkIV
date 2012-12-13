/*
 * valves.h
 *
 *  Created on: Dec 13, 2012
 *      Author: brad
 */

#ifndef VALVES_H_
#define VALVES_H_

#define HLT_VALVE 0
#define MASH_VALVE 1
#define BOIL_VALVE 2
#define INLET_VALVE 3

#define TOGGLE 255
#define OPEN 0
#define CLOSED 1


#define HLT_VALVE_PORT GPIOB
#define MASH_VALVE_PORT GPIOB
#define INLET_VALVE_PORT GPIOB
#define BOIL_VALVE_PORT GPIOE

#define HLT_VALVE_PIN GPIO_Pin_0
#define MASH_VALVE_PIN GPIO_Pin_1
#define INLET_VALVE_PIN GPIO_Pin_5
#define BOIL_VALVE_PIN GPIO_Pin_0



void vValvesInit(void);
void vValvesApplet(int init);
int iValvesKey(int xx, int yy);
void vValveActuate(unsigned char valve, unsigned char state);



#endif /* VALVES_H_ */