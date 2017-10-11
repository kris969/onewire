#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <stdbool.h>
#include <unistd.h>
#include "battery.h"

#include "wiring-CHIP.h"

// GPIOs on CHIP are mapped to base addresses.  These addresses changed between
// Kernel versions 4.3 and 4.4 so we keep two mappings and dynamically determine
// which kernel we are actually running upon.
static int gpioMapping44[] = {1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023};
static int gpioMapping43[] = {408, 409, 410, 411, 412, 413, 414, 415};

struct _fd2Address {
  int fd;
  int address;
};
static struct _fd2Address fd2address[64];
static int nextFD2Address = 0;


static int checkValidPin(int pin) {
  if (pin < 0 || pin > 7) {
    return -1;
  }
  return 0;
} // End of checkValidPin

static int findfd(int fd) {
  int i;
  for (i=0; i<nextFD2Address; i++) {
    if (fd2address[i].fd == fd) {
      return i;
    }
  }
  return -1;
}

static int findAddressFromFD(int fd) {
  int i = findfd(fd);
  if (fd == -1) {
    return -1;
  }
  return fd2address[i].address;
}


/**
 * GPIOs on CHIP are mapped to addresses.  So to work with pin 0, we don't actually work with "0" but
 * instead the address that 0 maps to.  Unfortunately, the mappings also vary by kernel version with
 * 4.3 being different to 4.4.  As such, mapping tables are produced that are used to index the pin
 * numbers being sought and the choice of table is based on the version of the Kernel in use.
 *
 * @param pin The pin to map to the base address.
 * @return The base address that maps to the pin.
 */
static int pinToBase(int pin) {
  struct utsname utsnameBuf;
  int rc;

  if (checkValidPin(pin) == -1) {
    return -1;
  }

  rc = uname(&utsnameBuf);
  if (rc == -1) {
    return -1;
  }
  if (strncmp(utsnameBuf.release, "4.4", 3) == 0) {
    return gpioMapping44[pin];
  } else {
    return gpioMapping43[pin];
  }
} // End of pinToBase


/**
 * Export a GPIO
 * @param pin The GPIO pin to export (0-7).
 *
 * Design:
 * A GPIO pin is exported by writing its "base number" into the file found at:
 *
 * /sys/class/gpio/export
 */
static void exportGPIO(int pin) {
  int base;
  FILE *file;

  base = pinToBase(pin);
  file = fopen("/sys/class/gpio/export", "w");
  if (file != NULL) {
    fprintf(file, "%d", base);
    fclose(file);
  } else {
    perror("export");
  }
} // End of exportGPIO


/**
 * Get the value from the pin.  The pin should previously have been set
 * to input mode with:
 *
 * ```
 * pinMode(<pin>, INPUT);
 * ```
 *
 * @param pin The pin to read (0-7).
 * @return The value read from the pin.  This will be either HIGH or LOW.
 *
 * Design:
 * The value of the pin can be retrieved from the file:
 *
 * /sys/class/gpio/gpio<baseNumber>/value
 *
 */
static int getValue(int pin) {
  char fileName[256];
  char value;
  FILE *file;
  int base;

  if (checkValidPin(pin) == -1) {
    return -1;
  }
  base = pinToBase(pin);

  sprintf(fileName, "/sys/class/gpio/gpio%d/value", base);

  file = fopen(fileName, "r");
  if (file == NULL) {
    perror("getValue");
    return -1;
  }
  fread(&value, 1, 1, file);
  fclose(file);
  // We get a value from the file which is either '1' or '0'.  We map that to a return value of
  // HIGH or LOW.
  if (value == '1') {
    return HIGH;
  }
  if (value == '0') {
    return LOW;
  }
  return -1;
} // End of getValue

/**
 * Set the value from the pin.  The pin should previously have been set
 * to output mode with:
 *
 * ```
 * pinMode(<pin>, OUTPUT);
 * ```
 *
 * @param pin The pin to read (0-7).
 * @param value The value to set the pin.  Should be HIGH or LOW.
 *
 * Design:
 * The value of the pin can be set by writing to the file:
 *
 * /sys/class/gpio/gpio<baseNumber>/value
 *
 */
static void setValue(int pin, int value) {
  char fileName[256];
  char cvalue;
  FILE *file;
  int base;

  if (checkValidPin(pin) == -1) {
    return;
  }

  if (value == HIGH) {
    cvalue = '1';
  } else {
    cvalue = '0';
  }

  base = pinToBase(pin);

  sprintf(fileName, "/sys/class/gpio/gpio%d/value", base);

  file = fopen(fileName, "w");
  if (file == NULL) {
    perror("getValue");
    return;
  }
  fwrite(&cvalue, 1, 1, file);
  fclose(file);
} // End of setValue


/**
 * Set the direction of a GPIO pin:
 * @param pin The GPIO pin to change (0-7).
 * @param direction The direction of GPIO access, either INPUT or OUTPUT.
 *
 * Design
 * Assuming that the pin is exported, we set the direction by writing either
 * "in" or "out" into the file found at:
 *
 * /sys/class/gpio/gpio<BaseNumber>/direction
 *
 * We open the file for writing and then write the control string into the file
 * as a function of the direction.
 */
static void setDirection(int pin, int direction) {
  char fileName[256];
  int base;
  FILE *file;

  if (checkValidPin(pin) == -1) {
    return;
  }
  if (direction != INPUT && direction != OUTPUT) {
    return;
  }
  base = pinToBase(pin);

  sprintf(fileName, "/sys/class/gpio/gpio%d/direction", base);
  file = fopen(fileName, "w");
  if (file != NULL) {
    if (direction == INPUT) {
      fprintf(file, "in");
    } else if (direction == OUTPUT) {
      fprintf(file, "out");
    }
    fclose(file);
  } else {
    perror("export");
  }
} // End of setDirection


int wiringCHIPSetup() {
  return 0;
} // End of wiringCHIPSetup


/**
 * Set the mode of the pin.
 * @param pin The pin to be set. (0-7)
 * @param mode The mode of the pin.  Should be one of either INPUT or OUTPUT.
 */
void pinMode(int pin, int mode) {
  if (checkValidPin(pin) == -1) {
    return;
  }
  if (mode != INPUT && mode != OUTPUT) {
    return;
  }
  exportGPIO(pin);
  setDirection(pin, mode);
} // End of pinMode


/**
 * Write a value to the pin.
 * @param pin The pin to be written to. (0-7)
 * @param value The value to be written to the pin.  Either HIGH or LOW.
 */
void digitalWrite(int pin, int value) {
  if (checkValidPin(pin) == -1) {
    return;
  }
  if (value != HIGH && value != LOW) {
    return;
  }
  setValue(pin, value);
} // End of digitalWrite


/**
 * Read a value from the pin.
 * @param pin The pin to be read from. (0-7)
 * @return The value to be read from the pin.  Will return either HIGH or LOW.
 */
int digitalRead(int pin) {
  if (checkValidPin(pin) == -1) {
    return -1;
  }
  int value = getValue(pin);
  return value;
} // End of digitalRead


int wiringCHIPI2CSetup(int bus, int devId) {
  char fileName[256];
  sprintf(fileName, "/dev/i2c-%d", bus);
  int fd = open(fileName, O_RDWR);
  if (fd == -1) {
    perror("wiringCHIPI2CSetup");
    return -1;
  }
  int rc = ioctl(fd, I2C_SLAVE, devId);
  if (rc == -1) {
    perror("wiringCHIPI2CSetup");
    int savedErrno = errno;
    close(fd);
    errno = savedErrno;
    return rc;
  }
  int i = findfd(fd);
  if (fd != -1) {
    fd2address[i].address = devId;
  } else {
    fd2address[nextFD2Address].address = devId;
    fd2address[nextFD2Address].fd = fd;
    nextFD2Address++;
  }
  return fd;
} // End of wiringCHIPI2CSetup


int wiringCHIPI2CRead(int fd) {
  unsigned char c;
  int rc = read(fd, &c, 1);
  if (rc != 1) {
    return -1;
  }
  return (int)c;
} // End of wiringCHIPI2CRead


int wiringCHIPI2CReadReg8(int fd, int reg) {
  struct i2c_msg msgs[2];
  unsigned char msg1;
  unsigned char msg2;
  struct i2c_rdwr_ioctl_data ioctlData;
  ioctlData.nmsgs = 2;
  ioctlData.msgs = msgs;
  msgs[0].addr = findAddressFromFD(fd);
  msgs[0].flags = 0;
  msgs[0].len = 1;
  msg1 = reg;
  msgs[0].buf = &msg1;

  msgs[1].addr = findAddressFromFD(fd);
  msgs[1].flags = I2C_M_RD;
  msgs[1].len = 1;
  msg2 = reg;
  msgs[1].buf = &msg2;

  int rc = ioctl(fd, I2C_RDWR, &ioctlData);
  if (rc == -1) {
    perror("wiringCHIPI2CReadReg8:");
  }
  if (rc == -1) {
    return -1;
  }

  return msg2;
} // End of wiringCHIPI2CReadReg8


int wiringCHIPI2CReadReg16(int fd, int reg) {
  struct i2c_msg msgs[2];
  unsigned char msg1;
  short msg2;
  struct i2c_rdwr_ioctl_data ioctlData;
  ioctlData.nmsgs = 2;
  ioctlData.msgs = msgs;
  msgs[0].addr = findAddressFromFD(fd);
  msgs[0].flags = 0;
  msgs[0].len = 1;
  msg1 = reg;
  msgs[0].buf = &msg1;

  msgs[1].addr = findAddressFromFD(fd);
  msgs[1].flags = 0;
  msgs[1].len = 2;
  msg2 = reg;
  msgs[1].buf = (unsigned char *)&msg2;

  int rc = ioctl(fd, I2C_RDWR, &ioctlData);
  if (rc == -1) {
    return -1;
  }
  return msg2;
} // End of wiringCHIPI2CReadReg16


int wiringCHIPI2CWrite(int fd, int data) {
  unsigned char c = data;
  int rc = write(fd, &c, 1);
  rc=rc; // pour éviter le warning
  return 0;
} // End of wiringCHIPI2CWrite


int wiringCHIPI2CWriteReg8(int fd, int reg, int data) {
  printf("fd=%d\n", fd);
  unsigned char c[2];
  c[0] = reg;
  c[1] = data;
  int rc = write(fd, c, 2);
  if (rc != 2) {
    return -1;
  }
  return 0;
} // End of wiringCHIPI2CWriteReg8


int wiringCHIPI2CWriteReg16(int fd, int reg, int data) {
  unsigned char c[3];
  c[0] = reg;
  c[1] = (data & 0xff00) >> 8;
  c[2] = (data & 0xff);
  int rc = write(fd, c, 3);
  if (rc != 3) {
    return -1;
  }
  return 0;
} // End of wiringCHIPI2CWriteReg16

