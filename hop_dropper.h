/*
 * hop_dropper.h
 *
 *  Created on: Dec 14, 2012
 *      Author: brad
 */

#ifndef HOP_DROPPER_H_
#define HOP_DROPPER_H_







#define HOP_DROPPER_LIMIT_PORT GPIOA
#define HOP_DROPPER_LIMIT_PIN GPIO_Pin_11

#define HOP_DROPPER_DRIVE_PIN PCF_PIN5
#define HOP_DROPPER_DRIVE_PORT PORTU

void vHopDropperApplet(int init);
int iHopDropperKey(int xx, int yy);
void vHopsInit(void);
void vTaskHops(void * pvParameters);

#endif /* HOP_DROPPER_H_ */
