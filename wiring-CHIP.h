/*
 * wiring-CHIP.h
 * An implementation of GPIO subsystem access for CHIP.  Based on the
 * interface developed for the fantastic package called "wiring-Pi" for the Raspberry Pi.
 *
 *  Created on: Jul 31, 2016
 *      Author: kolban
 */

#ifndef WIRING_CHIP_H_
#define WIRING_CHIP_H_

#define INPUT  (1)
#define OUTPUT (2)
#define HIGH   (1)
#define LOW    (0)

int wiringCHIPSetup();
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
int digitalRead(int pin);

int wiringCHIPI2CSetup(int bus, int devId);
int wiringCHIPI2CRead(int fd);
int wiringCHIPI2CReadReg8(int fd, int reg);
int wiringCHIPI2CReadReg16(int fd, int reg);
int wiringCHIPI2CWrite(int fd, int data);
int wiringCHIPI2CWriteReg8(int fd, int reg, int data);
int wiringCHIPI2CWriteReg16(int fd, int reg, int data);
#endif /* WIRING_CHIP_H_ */
